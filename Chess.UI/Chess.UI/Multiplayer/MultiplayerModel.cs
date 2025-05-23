using Chess.UI.Multiplayer;
using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.ChessLogicAPI;

namespace Chess.UI.Models
{
    public record NetworkAdapter
    {
        public string Name { get; set; }
        public int ID { get; set; }
    }


    public enum MultiplayerMode
    {
        None = 0,
        Init = 1,
        Server = 2,
        Client = 3
    }


    public class MultiplayerModel : IMultiplayerModel
    {

        private List<NetworkAdapter> mAdapters = new();


        public MultiplayerModel()
        {
            ChessLogicAPI.StartedMultiplayer();
        }


        public void Init()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as CommunicationLayer;
            logicCommunication.ConnectionStatusEvent += HandleConnectionStatusUpdates;
            logicCommunication.ClientRequestedConnection += ClientRequestedConnection;

            SetNetworkAdapters();
        }


        private void HandleConnectionStatusUpdates(ConnectionStatusEvent connectionStatusEvent)
        {
            ConnectionState connectionState = connectionStatusEvent.ConnectionState;

            if (connectionState == ConnectionState.Error)
            {
                string errorMessage = connectionStatusEvent.ErrorMessage;
                OnConnectionErrorOccured?.Invoke(errorMessage);
                return;
            }

            OnConnectionStatusChanged?.Invoke(connectionState);
        }


        private bool SetNetworkAdapters()
        {
            mAdapters.Clear();
            int adapterCount = ChessLogicAPI.GetNetworkAdapterCount();

            for (uint i = 0; i < adapterCount; ++i)
            {
                ChessLogicAPI.NetworkAdapter adapter;
                ChessLogicAPI.GetNetworkAdapterAtIndex(i, out adapter);

                NetworkAdapter networkAdapter = new();
                networkAdapter.Name = adapter.name;
                networkAdapter.ID = adapter.id;

                if (networkAdapter.ID == 0 && networkAdapter.Name == null)
                    continue;

                mAdapters.Add(networkAdapter);
            }

            bool result = mAdapters.Count > 0;
            return result;
        }


        public List<NetworkAdapter> GetNetworkAdapters()
        {
            return mAdapters;
        }


        public int GetSelectedNetworkAdapterID()
        {
            return ChessLogicAPI.GetSavedAdapterID();
        }


        public void ChangeNetworkAdapter(int ID)
        {
            ChessLogicAPI.ChangeCurrentAdapter(ID);
        }


        public string GetRemotePlayerName()
        {
            return ChessLogicAPI.GetRemotePlayerName();
        }


        public void SetLocalPlayerName(string name)
        {
            ChessLogicAPI.SetLocalPlayerName(name);
        }


        public void StartGameServer()
        {
            ChessLogicAPI.StartRemoteDiscovery(true);
        }


        public void StartGameClient()
        {
            ChessLogicAPI.StartRemoteDiscovery(false);
        }


        public void ConnectToHost()
        {

        }


        public void ResetToInit()
        {
            ChessLogicAPI.StoppedMultiplayer();
        }


        public void StartMultiplerGame(MultiplayerMode mode)
        {
            switch (mode)
            {
                case MultiplayerMode.Client:
                    {
                        ChessLogicAPI.StartMultiplayerGame(false);
                        break;
                    }
                case MultiplayerMode.Server:
                    {
                        ChessLogicAPI.StartMultiplayerGame(true);
                        break;
                    }
                default: break;
            }
        }

        public void AcceptConnectionRequest()
        {
            ChessLogicAPI.ApproveConnectionRequest();
        }


        public void RejectConnectionRequest()
        {
            ChessLogicAPI.RejectConnectionRequest();
        }


        public void ClientRequestedConnection(string clientName)
        {
            OnClientRequestedConnection?.Invoke(clientName);
        }


        public event Action<string> OnConnectionErrorOccured;
        public event Action<ConnectionState> OnConnectionStatusChanged;
        public event Action<string> OnClientRequestedConnection;
    }
}

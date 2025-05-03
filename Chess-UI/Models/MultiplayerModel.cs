using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;

namespace Chess_UI.Models
{
    public class NetworkAdapter
    {
        public string Name { get; set; }
        public int ID { get; set; }
    }


    public class MultiplayerModel
    {
        private List<NetworkAdapter> mAdapters = new();


        public void Init()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as ChessLogicCommunicationLayer;
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


        public void ClientRequestedConnection(string clientName)
        {
            OnClientRequestedConnection?.Invoke(clientName);
        }


        public delegate void ConnectionErrorOccured(string message);
        public event ConnectionErrorOccured OnConnectionErrorOccured;

        public delegate void ConnectionStatusChanged(ConnectionState newState);
        public event ConnectionStatusChanged OnConnectionStatusChanged;

        public delegate void ClientRequestedConnectionHandler(string clientName);
        public event ClientRequestedConnectionHandler OnClientRequestedConnection;
    }
}

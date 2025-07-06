using Chess.UI.Multiplayer;
using Chess.UI.Services;
using System;
using System.Collections.Generic;
using static Chess.UI.Services.EngineAPI;


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

        private readonly List<NetworkAdapter> _adapters = [];


        public void Init()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as CommunicationLayer;
            logicCommunication.ConnectionStatusEvent += HandleConnectionStatusUpdates;
            logicCommunication.PlayerChanged += HandlePlayerChanged;
            logicCommunication.MultiPlayerChosenByRemote += HandleLocalPlayerChosenByRemote;
        }


        public void StartMultiplayer()
        {
            EngineAPI.StartedMultiplayer();
            SetNetworkAdapters();
        }


        public void HandlePlayerChanged(PlayerColor player)
        {
            OnPlayerChanged?.Invoke(player);
        }



        private void HandleConnectionStatusUpdates(ConnectionStatusEvent connectionStatusEvent)
        {
            ConnectionState connectionState = connectionStatusEvent.ConnectionState;

            if (connectionState == ConnectionState.Error)
            {
                OnConnectionErrorOccured?.Invoke(connectionStatusEvent.errorMessage);
                return;
            }

            OnConnectionStatusChanged?.Invoke(connectionState, connectionStatusEvent.remoteName);
        }


        private bool SetNetworkAdapters()
        {
            _adapters.Clear();
            int adapterCount = EngineAPI.GetNetworkAdapterCount();

            for (uint i = 0; i < adapterCount; ++i)
            {
                EngineAPI.GetNetworkAdapterAtIndex(i, out EngineAPI.NetworkAdapter adapter);

                NetworkAdapter networkAdapter = new()
                {
                    Name = adapter.name,
                    ID = adapter.id
                };

                if (networkAdapter.ID == 0 && networkAdapter.Name == null)
                    continue;

                _adapters.Add(networkAdapter);
            }

            bool result = _adapters.Count > 0;
            return result;
        }


        public List<NetworkAdapter> GetNetworkAdapters()
        {
            return _adapters;
        }


        public int GetSelectedNetworkAdapterID()
        {
            return EngineAPI.GetSavedAdapterID();
        }


        public void ChangeNetworkAdapter(int ID)
        {
            EngineAPI.ChangeCurrentAdapter(ID);
        }


        public void SetLocalPlayerName(string name)
        {
            EngineAPI.SetLocalPlayerName(name);
        }


        public void StartGameServer()
        {
            EngineAPI.StartRemoteDiscovery(true);
        }


        public void StartGameClient()
        {
            EngineAPI.StartRemoteDiscovery(false);
        }


        public void ConnectToHost()
        {
            EngineAPI.SendConnectionRequestToHost();
        }


        public void ResetToInit()
        {
            EngineAPI.StoppedMultiplayer();
        }


        public void SetLocalPlayerColor(EngineAPI.PlayerColor color)
        {
            EngineAPI.SetLocalPlayer((int)color);
        }


        public void SetPlayerReady(bool flag)
        {
            EngineAPI.SetLocalPlayerReady(flag);
        }


        public void StartMultiplerGame()
        {
            EngineAPI.StartMultiplayerGame();
        }


        public void AnswerConnectionInvitation(bool accepted)
        {
            EngineAPI.AnswerConnectionInvitation(accepted);
        }


        public void HandleLocalPlayerChosenByRemote(PlayerColor local)
        {
            OnMultiplayerPlayerSetFromRemote?.Invoke(local);
        }



        public event Action<string> OnConnectionErrorOccured;
        public event Action<ConnectionState, string> OnConnectionStatusChanged;
        public event Action<string> OnClientRequestedConnection;
        public event Action<PlayerColor> OnPlayerChanged;
        public event Action<PlayerColor> OnMultiplayerPlayerSetFromRemote;

    }
}

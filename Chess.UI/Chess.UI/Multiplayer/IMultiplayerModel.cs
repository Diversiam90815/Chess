using Chess.UI.Models;
using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Multiplayer
{
    public interface IMultiplayerModel
    {
        void Init();

        void ResetToInit();

        void StartGameServer();

        void StartGameClient();

        void StartMultiplerGame(MultiplayerMode mode);

        void StartMultiplayer();

        void SetLocalPlayerName(string playerName);

        int GetSelectedNetworkAdapterID();

        void ChangeNetworkAdapter(int adapterID);

        List<Models.NetworkAdapter> GetNetworkAdapters();

        void ConnectToHost();

        void AnswerConnectionInvitation(bool accept);

        void SetLocalPlayerColor(EngineAPI.PlayerColor color);
        
        void SetPlayerReady(bool ready);


        event Action<string> OnConnectionErrorOccured;

        event Action<Services.EngineAPI.ConnectionState, string> OnConnectionStatusChanged;

        event Action<string> OnClientRequestedConnection;

        event Action<Services.EngineAPI.PlayerColor> OnPlayerChanged;
    }
}

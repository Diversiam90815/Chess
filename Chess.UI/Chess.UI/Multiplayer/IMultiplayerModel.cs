using Chess_UI.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.Multiplayer
{
    public interface IMultiplayerModel
    {
        void ResetToInit();

        void StartGameServer();

        void StartGameClient();

        void StartMultiplerGame(MultiplayerMode mode);

        void SetLocalPlayerName(string playerName);

        int GetSelectedNetworkAdapterID();

        void ChangeNetworkAdapter(int adapterID);

        List<NetworkAdapter> GetNetworkAdapters();

        void ConnectToHost();

        void AcceptConnectionRequest();

        void RejectConnectionRequest();

        event Action<string> OnConnectionErrorOccured;

        event Action<Services.ChessLogicAPI.ConnectionState> OnConnectionStatusChanged;

        event Action<string> OnClientRequestedConnection;
    }
}

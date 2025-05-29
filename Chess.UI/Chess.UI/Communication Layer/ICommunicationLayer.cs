using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.ChessLogicAPI;


namespace Chess.UI.Communication_Layer.Interfaces
{
    public interface ICommunicationLayer
    {
        // Initialization methods
        void Init();
        void Deinit();

        // Event handlers for communication from native code
        void DelegateHandler(int message, nint data);

        // Events for notifying UI components
        event Action<PlayerColor> PlayerChanged;
        event Action<GameState> GameStateChanged;
        event Action<string> MoveHistoryUpdated;
        event Action<PlayerCapturedPiece> PlayerCapturedPieceEvent;
        event Action<Services.ChessLogicAPI.Score> PlayerScoreUpdated;
        event Action<EndGameState> EndGameStateEvent;
        event Action<ConnectionStatusEvent> ConnectionStatusEvent;
    }
}

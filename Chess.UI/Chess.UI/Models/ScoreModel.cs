using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.ChessLogicAPI;

namespace Chess.UI.Models
{
    public class ScoreModel
    {

        public Dictionary<PieceTypeInstance, int> whiteCapturedPieces = new Dictionary<PieceTypeInstance, int>
    {
        { PieceTypeInstance.Pawn, 0 },
        { PieceTypeInstance.Bishop, 0 },
        { PieceTypeInstance.Knight, 0 },
        { PieceTypeInstance.Rook, 0 },
        { PieceTypeInstance.Queen, 0 }
    };

        public Dictionary<PieceTypeInstance, int> blackCapturedPieces = new Dictionary<PieceTypeInstance, int>
    {
        { PieceTypeInstance.Pawn, 0 },
        { PieceTypeInstance.Bishop, 0 },
        { PieceTypeInstance.Knight, 0 },
        { PieceTypeInstance.Rook, 0 },
        { PieceTypeInstance.Queen, 0 }
    };


        public ScoreModel()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as ChessLogicCommunicationLayer;
            logicCommunication.PlayerCapturedPieceEvent += OnPlayerCapturedPiece;
            logicCommunication.PlayerScoreUpdated += HandlePlayerScoreUpdated;

        }


        private void OnPlayerCapturedPiece(PlayerCapturedPiece captureEvent)
        {
            PlayerCapturedPiece?.Invoke(captureEvent);
        }


        private void HandlePlayerScoreUpdated(Score score)
        {
            PlayerScoreUpdated?.Invoke(score);
        }


        public delegate void PlayerCapturedPieceHandler(PlayerCapturedPiece captureEvent);
        public event PlayerCapturedPieceHandler PlayerCapturedPiece;

        public delegate void PlayerScoreUpdatedHandler(Score score);
        public event PlayerScoreUpdatedHandler PlayerScoreUpdated;

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;

namespace Chess_UI.Score
{
    public interface IScoreModel
    {
        // Properties
        Dictionary<PieceTypeInstance, int> WhiteCapturedPieces { get; }
        Dictionary<PieceTypeInstance, int> BlackCapturedPieces { get; }

        // Events
        event Action<PlayerCapturedPiece> PlayerCapturedPiece;
        event Action<Services.ChessLogicAPI.Score> PlayerScoreUpdated;
    }
}

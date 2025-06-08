using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.Models.Interfaces
{
    public interface IMoveModel
    {
        List<PossibleMoveInstance> PossibleMoves { get; }

        event Action PossibleMovesCalculated;
        event Action<PlayerColor> PlayerChanged;
        event Action GameStateInitSucceeded;
        event Action<EndGameState> GameOverEvent;
        event Action NewBoardFromBackendEvent;
        event Action PawnPromotionEvent;

        void SetPromotionPieceType(PieceTypeInstance pieceType);
    }
}
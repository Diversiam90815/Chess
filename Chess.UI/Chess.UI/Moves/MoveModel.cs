using Chess.UI.Models.Interfaces;
using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.ChessLogicAPI;


namespace Chess.UI.Moves
{
    public class MoveModel : IMoveModel
    {
        private List<PossibleMoveInstance> possibleMoves = [];

        public List<PossibleMoveInstance> PossibleMoves => possibleMoves;


        public MoveModel()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as CommunicationLayer;
            logicCommunication.GameStateChanged += HandleGameStateChanged;
            logicCommunication.PlayerChanged += HandlePlayerChanged;
            logicCommunication.EndGameStateEvent += HandleEndGameState;
        }


        private void HandleGameStateChanged(GameState gameState)
        {
            switch (gameState)
            {
                case GameState.InitSucceeded:
                    {
                        HandleInitSucceeded();
                        break;
                    }
                case GameState.WaitingForInput:
                    {
                        NewBoardFromBackendEvent?.Invoke();
                        break;
                    }
                case GameState.WaitingForTarget:
                    {
                        HandleWaitingForTarget();
                        break;
                    }
                case GameState.ValidatingMove:
                    {
                        break;
                    }
                case GameState.ExecutingMove:
                    {
                        NewBoardFromBackendEvent?.Invoke();
                        break;
                    }
                case GameState.GameOver:
                    {
                        break;
                    }
                case GameState.PawnPromotion:
                    {
                        PawnPromotionEvent?.Invoke();
                        break;
                    }
                default:
                    break;
            }
        }


        public void HandleInitSucceeded()
        {
            GameStateInitSucceeded?.Invoke();
        }


        public void HandleEndGameState(EndGameState endgame)
        {
            GameOverEvent?.Invoke(endgame);
        }


        private void HandleWaitingForTarget()
        {
            Logger.LogInfo("Due to delegate message WaitingForTarget we start getting the moves!");

            PossibleMoves.Clear();

            int numMoves = GetNumPossibleMoves();
            for (int i = 0; i < numMoves; i++)
            {
                if (GetPossibleMoveAtIndex((uint)i, out var move))
                {
                    PossibleMoves.Add(move);
                }
            }
            PossibleMovesCalculated?.Invoke();
        }


        public void SetPromotionPieceType(PieceTypeInstance pieceType)
        {
            Logger.LogInfo("Promoting to " + pieceType.ToString());
            OnPawnPromotionChosen(pieceType);
        }


        public void HandlePlayerChanged(PlayerColor player)
        {
            PlayerChanged?.Invoke(player);
        }


        public event Action PossibleMovesCalculated;
        public event Action<PlayerColor> PlayerChanged;
        public event Action GameStateInitSucceeded;
        public event Action<EndGameState> GameOverEvent;
        public event Action NewBoardFromBackendEvent;
        public event Action PawnPromotionEvent;
    }
}

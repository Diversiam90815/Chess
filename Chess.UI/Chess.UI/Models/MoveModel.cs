using Chess.UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.ChessLogicAPI;


namespace Chess.UI.Models
{
    public class MoveModel
    {
        public List<PossibleMoveInstance> PossibleMoves = [];


        public MoveModel()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as ChessLogicCommunicationLayer;
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

            int numMoves = ChessLogicAPI.GetNumPossibleMoves();
            for (int i = 0; i < numMoves; i++)
            {
                if (ChessLogicAPI.GetPossibleMoveAtIndex((uint)i, out var move))
                {
                    PossibleMoves.Add(move);
                }
            }
            PossibleMovesCalculated?.Invoke();
        }


        public void SetPromotionPieceType(PieceTypeInstance pieceType)
        {
            Logger.LogInfo("Promoting to " + pieceType.ToString());
            ChessLogicAPI.OnPawnPromotionChosen(pieceType);
        }


        public void HandlePlayerChanged(PlayerColor player)
        {
            PlayerChanged?.Invoke(player);
        }


        public delegate void PossibleMovesCalculatedDelegate();
        public event PossibleMovesCalculatedDelegate PossibleMovesCalculated;

        public delegate void PlayerChangedHandler(PlayerColor newPlayer);
        public event PlayerChangedHandler PlayerChanged;

        public delegate void PawnPromotionEventhander();
        public event PawnPromotionEventhander PawnPromotionEvent;

        public delegate void GameOverEventHandler(EndGameState endgame);
        public event GameOverEventHandler GameOverEvent;

        public delegate void GameStateInitSucceededHandler();
        public event GameStateInitSucceededHandler GameStateInitSucceeded;

        public delegate void NewBoardFromBackendHandler();
        public event NewBoardFromBackendHandler NewBoardFromBackendEvent;
    }
}

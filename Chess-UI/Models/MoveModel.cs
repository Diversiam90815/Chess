using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;


namespace Chess_UI.Models
{
    public class MoveModel
    {
        public List<PossibleMoveInstance> PossibleMoves;

        //public List<string> MoveHistory = new();

        public MoveModel()
        {
            var logicCommunication = App.Current.ChessLogicCommunication as ChessLogicCommunicationLayer;
            logicCommunication.GameStateChanged += HandleGameStateChanged;
        }


        private void HandleGameStateChanged(GameState gameState)
        {
            switch (gameState)
            {
                case GameState.WaitingForInput:
                    {

                        break;
                    }
                case GameState.MoveInitiated:
                    {
                        HandleInitiatedMove();
                        break;
                    }
                case GameState.ValidatingMove:
                    {
                        break;
                    }
                case GameState.GameOver:
                    {
                        break;
                    }
                default:
                    break;
            }
        }


        private void HandleInitiatedMove()
        {
            Logger.LogInfo("Due to delegate message initiateMove we start getting the moves!");

            PossibleMoves = new List<PossibleMoveInstance>();
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


        //private void UpdateMoveHistory(string moveNotation)
        //{
        //    MoveHistory.Add(moveNotation);
        //}


        public void SetPromotionPieceType(PieceTypeInstance pieceType)
        {
            // Call onPawnPromotionChosen via API
        }


        public delegate void PossibleMovesCalculatedDelegate();
        public event PossibleMovesCalculatedDelegate PossibleMovesCalculated;
    }
}

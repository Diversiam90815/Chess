using Chess_UI.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Configuration.ChessLogicAPI;

namespace Chess_UI.Configuration
{
    public class Controller
    {

        public Controller()
        {
            SetLogicAPIDelegate();
        }


        static APIDelegate Delegate = null;

        public List<PossibleMoveInstance> PossibleMoves;


        private void SetLogicAPIDelegate()
        {
            if (Delegate == null)
            {
                Delegate = new APIDelegate(DelegateHandler);
                SetDelegate(Delegate);
            }
        }


        public void DelegateHandler(int message, nint data)
        {
            DelegateMessage delegateMessage = (DelegateMessage)message;
            switch (delegateMessage)
            {
                case DelegateMessage.PlayerHasWon:
                    {
                        HandleWinner(data);
                        break;
                    }

                case DelegateMessage.InitiateMove:
                    {
                        // Engine has finished calculating possible moves
                        HandleInitiatedMove();
                        break;
                    }

                case DelegateMessage.PlayerScoreUpdate:
                    {
                        HandlePlayerScoreUpdate(data);
                        break;
                    }
                case DelegateMessage.MoveExecuted:
                    {
                        // Move is completed, so we reload the board
                        // And update move history
                        HandleExecutedMove(data);
                        break;
                    }
                default: break;
            }
        }


        private void HandleWinner(nint data)
        {
            int player = Marshal.ReadInt32(data);
            PlayerColor winner = (PlayerColor)player;

            // set winner through event trigger
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
        }


        private void HandlePlayerScoreUpdate(nint data)
        {
            Score score = (Score)Marshal.PtrToStructure(data, typeof(Score));

            PlayerColor player = score.player;
            int scoreValue = score.score;

            // trigger event for score change
        }


        private void HandleExecutedMove(nint data)
        {
            Logger.LogInfo("Due to delegate message moveExecuted, we react to the execution of the move and start updating the board!");
            string notation = Marshal.PtrToStringUTF8(data);
            ExecutedMove?.Invoke(notation);
        }


        public int[] GetBoardStateFromNative()
        {
            int[] board = new int[64]; // pre-allocated array

            ChessLogicAPI.GetBoardState(board);
            return board;
        }


        #region ViewModel Delegates

        // Define the delegate
        public delegate void ExecutedMoveHandler(string moveNotation);

        // define the event
        public event ExecutedMoveHandler ExecutedMove;

        #endregion

    }
}

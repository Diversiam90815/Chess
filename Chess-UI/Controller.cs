using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.ChessLogicAPI;

namespace Chess_UI
{
    public class Controller
    {
        public Controller()
        {

        }

        static APIDelegate mDelegate = null;

        private void SetLogicAPIDelegate()
        {
            if (mDelegate == null)
            {
                mDelegate = new APIDelegate(DelegateHandler);
                ChessLogicAPI.SetDelegate(mDelegate);
            }


        }

        public void DelegateHandler(int message, IntPtr data)
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
                        HandleInitiatedMove();
                        break;
                    }

                case DelegateMessage.PlayerScoreUpdate:
                    {
                        HandlePlayerScoreUpdate(data);
                        break;
                    }
                default: break;
            }
        }


        private void HandleWinner(IntPtr data)
        {
            int player = Marshal.ReadInt32(data);
            PlayerColor winner = (PlayerColor)player;

            // set winner through event trigger
        }


        private void HandleInitiatedMove()
        {

        }

        private void HandlePlayerScoreUpdate(IntPtr data)
        {
            Score score = (Score)Marshal.PtrToStructure(data, typeof(Score));

            PlayerColor player = score.player;
            int scoreValue = score.score;

            // trigger event for score change
        }

    }
}

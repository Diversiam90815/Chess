using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;

namespace Chess_UI.Services
{
    public class ChessLogicCommunicationLayer
    {
        private GCHandle _delegateHandle;


        public void Init()
        {
            ChessLogicAPI.SetUnvirtualizedAppDataPath(Project.AppDataDirectory);
            ChessLogicAPI.Init();
            SetDelegate();
        }


        public void Deinit()
        {
            ChessLogicAPI.Deinit();
            _delegateHandle.Free();
        }


        private void SetDelegate()
        {
            APIDelegate mDelegate = new(DelegateHandler);
            _delegateHandle = GCHandle.Alloc(mDelegate);
            ChessLogicAPI.SetDelegate(mDelegate);
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
                case DelegateMessage.PlayerScoreUpdate:
                    {
                        HandlePlayerScoreUpdate(data);
                        break;
                    }
                case DelegateMessage.PlayerChanged:
                    {
                        HandlePlayerChanged(data);
                        break;
                    }
                case DelegateMessage.GameStateChanged:
                    {
                        HandleGameStateChanges(data);
                        break;
                    }
                case DelegateMessage.MoveHistoryAdded:
                    {
                        HandleMoveNotationAdded(data);
                        break;
                    }
                case DelegateMessage.PlayerCapturedPiece:
                    {
                        HandlePlayerCapturedPiece(data);
                        break;
                    }

                default: break;
            }
        }


        private void HandlePlayerScoreUpdate(nint data)
        {
            Score score = (Score)Marshal.PtrToStructure(data, typeof(Score));
            PlayerScoreUpdated?.Invoke(score);
        }


        private void HandleMoveNotationAdded(nint data)
        {
            string notation = Marshal.PtrToStringUTF8(data);
            MoveHistoryUpdated?.Invoke(notation);
        }


        private void HandlePlayerChanged(nint data)
        {
            Logger.LogInfo("Due to delegate message PlayerChanged, we react to setting the current player.");
            int iPlayer = Marshal.ReadInt32(data);
            PlayerColor player = (PlayerColor)iPlayer;
            PlayerChanged?.Invoke(player);
        }


        private void HandleWinner(nint data)
        {
            int player = Marshal.ReadInt32(data);
            PlayerColor winner = (PlayerColor)player;

            // set winner through event trigger
        }


        private void HandlePlayerCapturedPiece(nint data)
        {
            PlayerCapturedPiece capturedEvent = (PlayerCapturedPiece)Marshal.PtrToStructure(data, typeof(PlayerCapturedPiece));
            PlayerCapturedPieceEvent?.Invoke(capturedEvent);
        }


        private void HandleGameStateChanges(nint data)
        {
            int iState = Marshal.ReadInt32(data);
            GameState state = (GameState)iState;
            GameStateChanged?.Invoke(state);
        }


        #region ViewModel Delegates

        // Define the delegate
        public delegate void PlayerChangedHandler(PlayerColor player);
        public delegate void GameStateChangedHandler(GameState state);
        public delegate void MoveHistoryUpdatedHandler(string moveNotation);
        public delegate void PlayerCaputeredPieceHandler(PlayerCapturedPiece capturedPiece);
        public delegate void PlayerScoreUpdatedHandler(Score score);

        // define the event
        public event PlayerChangedHandler PlayerChanged;
        public event GameStateChangedHandler GameStateChanged;
        public event MoveHistoryUpdatedHandler MoveHistoryUpdated;
        public event PlayerCaputeredPieceHandler PlayerCapturedPieceEvent;
        public event PlayerScoreUpdatedHandler PlayerScoreUpdated;

        #endregion

    }
}

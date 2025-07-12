using Chess.UI.Communication_Layer.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.Services
{
    public class CommunicationLayer : ICommunicationLayer
    {
        private GCHandle _delegateHandle;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void APIDelegate(int message, nint data);


        public enum DelegateMessage
        {
            EndGameState = 1,
            PlayerScoreUpdated = 2,
            PlayerCapturedPiece = 3,
            PlayerChanged = 4,
            GameStateChanged = 5,
            MoveHistoryUpdated = 6,
            ConnectionStateChanged = 7,
            MultiplayerPlayerChosen = 8,
        }


        public void Init()
        {
            EngineAPI.SetUnvirtualizedAppDataPath(Project.AppDataDirectory);
            EngineAPI.Init();
            SetDelegate();
        }


        public void Deinit()
        {
            EngineAPI.Deinit();
            _delegateHandle.Free();
        }


        private void SetDelegate()
        {
            APIDelegate mDelegate = new(DelegateHandler);
            _delegateHandle = GCHandle.Alloc(mDelegate);
            EngineAPI.SetDelegate(mDelegate);
        }


        public void DelegateHandler(int message, nint data)
        {
            DelegateMessage delegateMessage = (DelegateMessage)message;
            switch (delegateMessage)
            {
                case DelegateMessage.EndGameState:
                    {
                        HandleEndGameState(data);
                        break;
                    }
                case DelegateMessage.PlayerScoreUpdated:
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
                case DelegateMessage.MoveHistoryUpdated:
                    {
                        HandleMoveHistoryUpdated(data);
                        break;
                    }
                case DelegateMessage.PlayerCapturedPiece:
                    {
                        HandlePlayerCapturedPiece(data);
                        break;
                    }
                case DelegateMessage.ConnectionStateChanged:
                    {
                        HandleConnectionStatusChanged(data);
                        break;
                    }
                case DelegateMessage.MultiplayerPlayerChosen:
                    {
                        HandlePlayerChosenForMultiplayerByRemote(data);
                        break;
                    }

                default: break;
            }
        }


        private void HandlePlayerScoreUpdate(nint data)
        {
            EngineAPI.Score score = (EngineAPI.Score)Marshal.PtrToStructure(data, typeof(EngineAPI.Score));
            PlayerScoreUpdated?.Invoke(score);
        }


        private void HandleMoveHistoryUpdated(nint data)
        {
            MoveHistoryEvent moveHistoryEvent = (MoveHistoryEvent)Marshal.PtrToStructure(data, typeof(MoveHistoryEvent));
            MoveHistoryUpdated?.Invoke(moveHistoryEvent);
        }


        private void HandlePlayerChanged(nint data)
        {
            int iPlayer = Marshal.ReadInt32(data);
            PlayerColor player = (PlayerColor)iPlayer;
            PlayerChanged?.Invoke(player);
        }


        private void HandleEndGameState(nint data)
        {
            int endGame = Marshal.ReadInt32(data);
            EndGameState state = (EndGameState)endGame;
            EndGameStateEvent?.Invoke(state);
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


        private void HandleConnectionStatusChanged(nint data)
        {
            ConnectionStatusEvent connectionEvent = (ConnectionStatusEvent)Marshal.PtrToStructure(data, typeof(ConnectionStatusEvent));
            ConnectionStatusEvent?.Invoke(connectionEvent);
        }


        private void HandlePlayerChosenForMultiplayerByRemote(nint data)
        {
            int iPlayer = Marshal.ReadInt32(data);
            PlayerColor player = (PlayerColor)iPlayer;
            MultiPlayerChosenByRemote?.Invoke(player);
        }


        #region ViewModel Delegates

        public event Action<PlayerColor> PlayerChanged;
        public event Action<GameState> GameStateChanged;
        public event Action<MoveHistoryEvent> MoveHistoryUpdated;
        public event Action<PlayerCapturedPiece> PlayerCapturedPieceEvent;
        public event Action<EngineAPI.Score> PlayerScoreUpdated;
        public event Action<EndGameState> EndGameStateEvent;
        public event Action<ConnectionStatusEvent> ConnectionStatusEvent;
        public event Action<PlayerColor> MultiPlayerChosenByRemote;

        #endregion

    }
}

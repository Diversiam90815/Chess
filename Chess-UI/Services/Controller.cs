using Chess_UI.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;

namespace Chess_UI.Services
{
	public class Controller
	{

		public Controller()
		{
			SetLogicAPIDelegate();
		}


		static APIDelegate Delegate = null;

		public List<PossibleMoveInstance> PossibleMoves;

		public List<string> MoveHistory = new();


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
						// And update move history (put the move history into an other delelate message later!)
						HandleExecutedMove();
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


		private void HandleGameStateChanges(nint data)
		{
			int iState = Marshal.ReadInt32(data);
			GameState state = (GameState)iState;
			//GameStateChanged?.Invoke(state);

			switch (state)
			{
				case GameState.MoveInitiated:
					{
						MoveInitiated?.Invoke();
						break;
					}
				case GameState.WaitingForInput:
					{
						HandleInitiatedMove();
                        //MoveWaitingForInput?.Invoke();
						break;
					}
				case GameState.WaitingForTarget:
					{
						MoveWaitingForTarget?.Invoke();
						break;
					}
				case GameState.PawnPromotion:
					{
						MovePawnPromotionEvent?.Invoke();
						break;
					}
				case GameState.GameOver:
					{
						GameEndStateEvent?.Invoke();
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


		private void HandleExecutedMove()
		{
			Logger.LogInfo("Due to delegate message moveExecuted, we react to the execution of the move and start updating the board!");
			ExecutedMove?.Invoke();
		}


		private void HandleMoveNotationAdded(nint data)
		{
			string notation = Marshal.PtrToStringUTF8(data);
			MoveHistory.Add(notation);
			MoveHistoryUpdated?.Invoke();
		}


		private void HandlePlayerChanged(nint data)
		{
			Logger.LogInfo("Due to delegate message PlayerChanged, we react to setting the current player.");
			int iPlayer = Marshal.ReadInt32(data);
			PlayerColor player = (PlayerColor)iPlayer;
			PlayerChanged?.Invoke(player);
		}


		public int[] GetBoardStateFromNative()
		{
			int[] board = new int[64]; // pre-allocated array

			ChessLogicAPI.GetBoardState(board);
			return board;
		}


		public void SetPromotionPieceType(PieceTypeInstance pieceType)
		{
			// Call onPawnPromotionChosen via API
		}


		#region ViewModel Delegates

		// Define the delegate
		public delegate void ExecutedMoveHandler();
		public delegate void PossibleMovesCalculatedHandler();
		public delegate void PlayerChangedHandler(PlayerColor player);
		//public delegate void GameStateChangedHandler(GameState state);
		public delegate void MoveHistoryUpdatedHandler();
		public delegate void PlayerCaputeredPieceHandler(PlayerCapturedPiece capturedPiece);
		public delegate void PlayerScoreUpdatedHandler(Score score);

		public delegate void MoveInitiatedHandler();
		public delegate void MoveWaitingForInputHandler();
		public delegate void MoveWaitingForTargetHandler();
		public delegate void MovePawnPromotionHandler();
		public delegate void GameEndStateHandler();


		// define the event
		public event ExecutedMoveHandler ExecutedMove;
		public event PossibleMovesCalculatedHandler PossibleMovesCalculated;
		public event PlayerChangedHandler PlayerChanged;
		//public event GameStateChangedHandler GameStateChanged;
		public event MoveHistoryUpdatedHandler MoveHistoryUpdated;
		public event PlayerCaputeredPieceHandler PlayerCapturedPieceEvent;
		public event PlayerScoreUpdatedHandler PlayerScoreUpdated;

		public event MoveInitiatedHandler MoveInitiated;
		public event MoveWaitingForInputHandler MoveWaitingForInput;
		public event MoveWaitingForTargetHandler MoveWaitingForTarget;
		public event MovePawnPromotionHandler MovePawnPromotionEvent;
		public event GameEndStateHandler GameEndStateEvent;

		#endregion

	}
}

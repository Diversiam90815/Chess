using Chess.UI.Services;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using static Chess.UI.Services.Images;
using static Chess.UI.Services.ChessLogicAPI;
using System.Collections.ObjectModel;
using System;
using Microsoft.UI.Composition.Interactions;
using Windows.UI.Popups;
using Microsoft.UI.Xaml;
using System.Threading.Tasks;
using System.Collections.Generic;
using Chess.UI.Models;
using System.Xml.Linq;


namespace Chess.UI.ViewModels
{
	public class ChessBoardViewModel : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		private readonly DispatcherQueue DispatcherQueue;

		public ObservableCollection<BoardSquare> Board { get; set; }

		public event Func<EndGameState, Task> ShowEndGameDialog;

		public event Func<Task<PieceTypeInstance?>> ShowPawnPromotionDialogRequested;

		public ScoreViewModel ScoreViewModel { get; }
		public MoveHistoryViewModel MoveHistoryViewModel { get; set; }

		private readonly ThemeManager themeManager;

		private MoveModel MoveModel;

		private BoardModel BoardModel;


		public ChessBoardViewModel(DispatcherQueue dispatcherQueue, ThemeManager themeManager)
		{
			this.DispatcherQueue = dispatcherQueue;
			this.themeManager = themeManager;

			MoveHistoryViewModel = new(DispatcherQueue);

			ScoreViewModel = new(DispatcherQueue);
			MoveModel = new();
			BoardModel = new();

			MoveModel.PossibleMovesCalculated += OnHighlightPossibleMoves;
			MoveModel.PlayerChanged += OnHandlePlayerChanged;
			MoveModel.GameStateInitSucceeded += OnGameStateInitSucceeded;
			MoveModel.GameOverEvent += OnEndGameState;
			MoveModel.NewBoardFromBackendEvent += OnBoardFromBackendUpdated;

			this.themeManager.PropertyChanged += OnThemeManagerPropertyChanged;

			MoveModel.PawnPromotionEvent += OnPromotionPiece;

			this.CurrentBoardTheme = themeManager.CurrentBoardTheme;

			Board = new ObservableCollection<BoardSquare>();

			for (int i = 0; i < ChessCoordinate.GetNumBoardSquares() ; i++)
			{
				Board.Add(new(dispatcherQueue, themeManager));
			}
		}


		public void LoadBoardFromNative()
		{
			var boardState = BoardModel.GetBoardStateFromNative();

			for (int i = 0; i < boardState.Length; i++)
			{
				int encoded = boardState[i];

				// Decode color and piece
				int colorVal = (encoded >> 4) & 0xF;    // top 8 bits
				int pieceVal = encoded & 0xF;          // bottom 8 bits

				PositionInstance enginePos = ChessCoordinate.FromIndex(i);  // Get engine pos from index
				PositionInstance displayPos = ChessCoordinate.ToDisplayCoordinates(enginePos); // Convert it to the UI pos

				var square = new BoardSquare(
					x: displayPos.x,
					y: displayPos.y,
					(PieceTypeInstance)pieceVal,
					(PlayerColor)colorVal,
					DispatcherQueue,
					themeManager
				);

				// Calculate the index in the UI board array
				int displayIndex = ChessCoordinate.ToIndex(displayPos, true);

				DispatcherQueue.TryEnqueue(() =>
				{
					Board[displayIndex] = square;
					OnPropertyChanged(nameof(Board));
				});
			}
		}


		private void OnBoardFromBackendUpdated()
		{
			DispatcherQueue.TryEnqueue(() =>
			{
				LoadBoardFromNative();
			});
		}


		public void ResetGame()
		{
			ChessLogicAPI.ResetGame();
			MoveHistoryViewModel.ClearMoveHistory();

			Board.Clear();
			for (int i = 0; i < ChessCoordinate.GetNumBoardSquares(); i++)
			{
				Board.Add(new BoardSquare(DispatcherQueue, themeManager));
			}
			ScoreViewModel.ReinitScoreValues();
		}


		public void StartGame()
		{
			ChessLogicAPI.StartGame();  // Start the game and thus the StateMachine
		}


		public void OnGameStateInitSucceeded()
		{
			// Once the board is ready calculated, we load it from native
			LoadBoardFromNative();
		}


		private void OnThemeManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			if (e.PropertyName == nameof(ThemeManager.CurrentBoardTheme))
			{
				UpdateBoardTheme(this.themeManager.CurrentBoardTheme);
			}
		}


		private void UpdateBoardTheme(Images.BoardTheme boardTheme)
		{
			CurrentBoardTheme = boardTheme;
		}


		private async void OnPromotionPiece()
		{
			var promotionPiece = await RequestPawnPromotionAsync();
			if (promotionPiece.HasValue)
			{
				MoveModel.SetPromotionPieceType(promotionPiece.Value);
			}
			else
			{
				// Pawn Promotion has been cancelled
				ResetHighlightsOnBoard();
			}
		}


		public void HandleSquareClick(BoardSquare square)
		{
			PositionInstance enginePos = ChessCoordinate.FromDisplayCoordinates(square.pos);

			Logger.LogInfo($"Square (UI) X{square.pos.x}-Y{square.pos.y} clicked => (Engine) X{enginePos.x}-Y{enginePos.y}!");

			ChessLogicAPI.OnSquareSelected(enginePos);
		}


		public void OnHighlightPossibleMoves()
		{
			ResetHighlightsOnBoard();

			foreach (var pm in MoveModel.PossibleMoves)
			{
				PositionInstance displayPos = ChessCoordinate.ToDisplayCoordinates(pm.end);

				int index = ChessCoordinate.ToIndex(displayPos, true);
				BoardSquare square = Board[index];

				if (square != null)
				{
					square.IsHighlighted = true;
				}
			}
		}


		public void ResetHighlightsOnBoard()
		{
			foreach (var square in Board)
			{
				square.IsHighlighted = false;
			}
		}


		public void UndoLastMove()
		{
			ChessLogicAPI.UndoMove();
			LoadBoardFromNative();
			MoveHistoryViewModel.RemoveLastMove();
		}


		private Task<PieceTypeInstance?> RequestPawnPromotionAsync()
		{
			if (ShowPawnPromotionDialogRequested != null)
			{
				return ShowPawnPromotionDialogRequested.Invoke();
			}
			return null;
		}


		private void OnEndGameState(EndGameState state)
		{
			ShowEndGameDialog?.Invoke(state);
		}


		private void OnHandlePlayerChanged(PlayerColor player)
		{
			CurrentPlayer = player;
		}


		private PlayerColor currentPlayer;
		public PlayerColor CurrentPlayer
		{
			get => currentPlayer;
			set
			{
				if (value != currentPlayer)
				{
					currentPlayer = value;
				}
			}
		}


		public Images.BoardTheme CurrentBoardTheme;


		public ImageSource BoardBackgroundImage
		{
			get
			{
				return Images.GetImage(CurrentBoardTheme);
			}
		}


		protected void OnPropertyChanged([CallerMemberName] string name = null)
		{
			DispatcherQueue.TryEnqueue(() =>
			{
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
			});
		}
	}
}

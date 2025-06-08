using Chess.UI.Services;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using static Chess.UI.Images.ImageServices;
using static Chess.UI.Services.EngineAPI;
using System.Collections.ObjectModel;
using System;
using Microsoft.UI.Composition.Interactions;
using Windows.UI.Popups;
using Microsoft.UI.Xaml;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Xml.Linq;
using Chess.UI.Themes;
using Chess.UI.Board;
using Chess.UI.Moves;
using Chess.UI.Coordinates;
using Chess.UI.Images;
using Chess.UI.Wrappers;
using Chess.UI.Themes.Interfaces;
using Chess.UI.Services.Interfaces;
using Microsoft.Extensions.DependencyInjection;
using Chess.UI.Models.Interfaces;


namespace Chess.UI.ViewModels
{
    public class ChessBoardViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        public ObservableCollection<BoardSquare> Board { get; set; }

        public event Func<EndGameState, Task> ShowEndGameDialog;

        public event Func<Task<PieceTypeInstance?>> ShowPawnPromotionDialogRequested;

        public ScoreViewModel _scoreViewModel { get; }

        public MoveHistoryViewModel _moveHistoryViewModel { get; set; }

        private readonly IThemeManager _themeManager;

        private IMoveModel _moveModel;

        private IBoardModel _boardModel;

        private readonly IChessCoordinate _coordinate;

        private readonly IImageService _imageServices;


        public ChessBoardViewModel(IDispatcherQueueWrapper dispatcherQueue)
        {
            _dispatcherQueue = dispatcherQueue;

            _moveHistoryViewModel = App.Current.Services.GetService<MoveHistoryViewModel>();
            _scoreViewModel = App.Current.Services.GetService<ScoreViewModel>();
            _moveModel = App.Current.Services.GetService<IMoveModel>();
            _boardModel = App.Current.Services.GetService<IBoardModel>();
            _coordinate = App.Current.Services.GetService<IChessCoordinate>();
            _imageServices = App.Current.Services.GetService<IImageService>();
            _themeManager = App.Current.Services.GetService<IThemeManager>();

            _moveModel.PossibleMovesCalculated += OnHighlightPossibleMoves;
            _moveModel.PlayerChanged += OnHandlePlayerChanged;
            _moveModel.GameStateInitSucceeded += OnGameStateInitSucceeded;
            _moveModel.GameOverEvent += OnEndGameState;
            _moveModel.NewBoardFromBackendEvent += OnBoardFromBackendUpdated;

            _themeManager.PropertyChanged += OnThemeManagerPropertyChanged;

            _moveModel.PawnPromotionEvent += OnPromotionPiece;

            this.CurrentBoardTheme = _themeManager.CurrentBoardTheme;

            Board = new ObservableCollection<BoardSquare>();

            for (int i = 0; i < _coordinate.GetNumBoardSquares(); i++)
            {
                Board.Add(new());
            }
        }


        public void InitializeBoardFromNative()
        {
            var boardState = _boardModel.GetBoardStateFromNative();

            for (int i = 0; i < _coordinate.GetNumBoardSquares(); i++)
            {
                BoardSquare square = _boardModel.DecodeBoardState(i, boardState);

                // Calculate the index in the UI board array
                int displayIndex = _coordinate.ToIndex(square.pos, true);

                _dispatcherQueue.TryEnqueue(() =>
                {
                    Board[displayIndex] = square;
                    OnPropertyChanged(nameof(Board));
                });
            }
        }


        public void UpdateBoardFromNative()
        {
            ResetHighlightsOnBoard();

            var (changedSquares, newBoardState) = _boardModel.UpdateBoardState();

            foreach (var kvp in changedSquares)
            {
                int boardIndex = kvp.Key;      // The index on the chess board
                int encodedState = kvp.Value;  // The encoded state value

                BoardSquare square = _boardModel.DecodeBoardState(boardIndex, newBoardState);

                // Calculate the index in the UI board array
                int displayIndex = _coordinate.ToIndex(square.pos, true);

                _dispatcherQueue.TryEnqueue(() =>
                {
                    Board[displayIndex] = square;
                    OnPropertyChanged(nameof(Board));
                });
            }
        }


        private void OnBoardFromBackendUpdated()
        {
            _dispatcherQueue.TryEnqueue(() =>
            {
                UpdateBoardFromNative();
            });
        }


        public void ResetGame()
        {
            EngineAPI.ResetGame();

            Board.Clear();
            for (int i = 0; i < _coordinate.GetNumBoardSquares(); i++)
            {
                Board.Add(new BoardSquare());
            }
            _scoreViewModel.ReinitScoreValues();
        }


        public void StartGame()
        {
            EngineAPI.StartGame();  // Start the game and thus the StateMachine
        }


        public void OnGameStateInitSucceeded()
        {
            // Once the board is ready calculated, we load it from native
            InitializeBoardFromNative();
        }


        private void OnThemeManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(ThemeManager.CurrentBoardTheme))
            {
                UpdateBoardTheme(_themeManager.CurrentBoardTheme);
            }
        }


        private void UpdateBoardTheme(ImageServices.BoardTheme boardTheme)
        {
            CurrentBoardTheme = boardTheme;
        }


        private async void OnPromotionPiece()
        {
            var promotionPiece = await RequestPawnPromotionAsync();
            if (promotionPiece.HasValue)
            {
                _moveModel.SetPromotionPieceType(promotionPiece.Value);
            }
            else
            {
                // Pawn Promotion has been cancelled
                ResetHighlightsOnBoard();
            }
        }


        public void HandleSquareClick(BoardSquare square)
        {
            PositionInstance enginePos = _coordinate.FromDisplayCoordinates(square.pos);

            Logger.LogInfo($"Square (UI) X{square.pos.x}-Y{square.pos.y} clicked => (Engine) X{enginePos.x}-Y{enginePos.y}!");

            EngineAPI.OnSquareSelected(enginePos);
        }


        public void OnHighlightPossibleMoves()
        {
            ResetHighlightsOnBoard();

            foreach (var pm in _moveModel.PossibleMoves)
            {
                PositionInstance displayPos = _coordinate.ToDisplayCoordinates(pm.end);

                int index = _coordinate.ToIndex(displayPos, true);
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
            EngineAPI.UndoMove();
            UpdateBoardFromNative();
            _moveHistoryViewModel.RemoveLastMove();
        }


        private Task<PieceTypeInstance?> RequestPawnPromotionAsync()
        {
            if (ShowPawnPromotionDialogRequested != null)
            {
                return ShowPawnPromotionDialogRequested.Invoke();
            }
            return Task.FromResult<PieceTypeInstance?>(null);
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
                    OnPropertyChanged();
                }
            }
        }


        public ImageServices.BoardTheme CurrentBoardTheme;


        public ImageSource BoardBackgroundImage
        {
            get
            {
                return _imageServices.GetImage(CurrentBoardTheme);
            }
        }


        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            _dispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}

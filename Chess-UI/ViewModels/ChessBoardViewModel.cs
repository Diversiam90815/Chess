using Chess_UI.Services;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using static Chess_UI.Services.Images;
using static Chess_UI.Services.ChessLogicAPI;
using System.Collections.ObjectModel;
using System;
using Microsoft.UI.Composition.Interactions;
using Windows.UI.Popups;
using Microsoft.UI.Xaml;
using System.Threading.Tasks;
using System.Collections.Generic;


namespace Chess_UI.ViewModels
{
    public class ChessBoardViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        private const int MovesMaxColumns = 3;

        private Controller Controller;

        public ObservableCollection<ObservableCollection<string>> MoveHistoryColumns { get; } = [];

        public ObservableCollection<BoardSquare> Board { get; set; }

        public event Func<GameState, Task> ShowGameStateDialogRequested;

        public event Func<Task<PieceTypeInstance?>> ShowPawnPromotionDialogRequested;

        public ScoreViewModel ScoreViewModel { get; }

        private readonly ThemeManager themeManager;


        public ChessBoardViewModel(DispatcherQueue dispatcherQueue, Controller controller, ThemeManager themeManager)
        {
            this.DispatcherQueue = dispatcherQueue;
            this.Controller = controller;
            this.themeManager = themeManager;

            ScoreViewModel = new(DispatcherQueue, controller);

            Controller.ExecutedMove += OnExecutedMove;
            Controller.PossibleMovesCalculated += OnHighlightPossibleMoves;
            Controller.PlayerChanged += OnHandlePlayerChanged;
            Controller.GameStateChanged += OnHandleGameStateChanged;
            Controller.MoveHistoryUpdated += OnHandleMoveHistoryUpdated;
            Controller.PlayerCapturedPieceEvent += ScoreViewModel.OnPlayerCapturedPiece;
            Controller.PlayerScoreUpdated += ScoreViewModel.OnPlayerScoreUpdated;
            this.themeManager.PropertyChanged += OnThemeManagerPropertyChanged;

            ChessLogicAPI.StartGame();

            Board = new ObservableCollection<BoardSquare>();

            for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
            {
                Board.Add(new(dispatcherQueue, themeManager));
            }

            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add(new ObservableCollection<string>());
            }

            LoadBoardFromNative();
        }


        public void LoadBoardFromNative()
        {
            var boardState = Controller.GetBoardStateFromNative();

            for (int i = 0; i < boardState.Length; i++)
            {
                int encoded = boardState[i];

                // Decode color and piece
                int colorVal = (encoded >> 4) & 0xF;    // top 8 bits
                int pieceVal = encoded & 0xF;          // bottom 8 bits

                // The engine’s row=0 is bottom, row=7 is top,
                // but the default i/8 loop is top..down. So flip:
                int rowFromTop = i / BOARD_SIZE;   // 0..7 (top..bottom)
                int col = i % BOARD_SIZE;
                int rowUI = 7 - rowFromTop;   // invert the row


                var square = new BoardSquare(
                    x: col,
                    y: rowUI,
                    (PieceTypeInstance)pieceVal,
                    (PlayerColor)colorVal,
                    DispatcherQueue,
                    themeManager
                );

                // Now compute where in Board[] it should go, so that rowUI=7 is stored first and rowUI=0 last row
                int index = (7 - rowUI) * BOARD_SIZE + col;
                Board[index] = square;
            }
        }


        public void ResetGame()
        {
            ChessLogicAPI.ResetGame();
            ClearMoveHistory();
        }


        public void StartGame()
        {
            ChessLogicAPI.StartGame();
            LoadBoardFromNative();
        }


        public void AddMove(string move)
        {
            // Find the column with the least number of moves
            var minColumn = MoveHistoryColumns.OrderBy(col => col.Count).First();
            minColumn.Add(move);
        }


        public void ClearMoveHistory()
        {
            MoveHistoryColumns.Clear();
            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add(new ObservableCollection<string>());
            }
        }


        private void OnThemeManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(ThemeManager.CurrentBoardTheme))
            {
                UpdateBoardTheme(this.themeManager.CurrentBoardTheme);
            }
        }


        private void UpdateBoardTheme(Services.BoardTheme boardTheme)
        {
            CurrentBoardTheme = boardTheme.BoardThemeID;
        }


        public async void HandleSquareClick(BoardSquare square)
        {
            int engineX = square.pos.x;
            int engineY = 7 - square.pos.y; // invert back

            Logger.LogInfo($"Square (UI) X{square.pos.x}-Y{square.pos.y} clicked => (Engine) X{engineX}-Y{engineY}!");

            switch (CurrentMoveState)
            {
                // The user is picking the start of a move
                case MoveState.NoMove:
                    {
                        Logger.LogInfo("Move State is NoMove and we start to initialize the move now!");

                        if (square.piece == PieceTypeInstance.DefaultType)
                            return;

                        CurrentPossibleMove = new PossibleMoveInstance
                        {
                            start = new PositionInstance(engineX, engineY)
                        };
                        CurrentMoveState = MoveState.InitiateMove;

                        ChessLogicAPI.HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());

                        // The engine will calculate possible moves 
                        // and eventually call back "delegateMessage::initiateMove"

                        break;
                    }

                // The user is picking the end of a move
                case MoveState.InitiateMove:
                    {
                        Logger.LogInfo("Move has already been initiated, and we start validating the move now!");

                        if (CurrentPossibleMove != null)
                        {
                            var move = CurrentPossibleMove.Value;
                            move.end = new PositionInstance(engineX, engineY);
                            CurrentPossibleMove = move;

                            if (CheckForValidMove())
                            {
                                Logger.LogInfo("The move has been validated, so we start the execution now!");

                                //Checking for a pawn promotion
                                if ((CurrentPossibleMove.Value.type & MoveTypeInstance.MoveType_PawnPromotion) == MoveTypeInstance.MoveType_PawnPromotion)
                                {
                                    // Await user's promotion piece selection
                                    var promotionPiece = await RequestPawnPromotionAsync();

                                    if (promotionPiece.HasValue)
                                    {
                                        CurrentPossibleMove = CurrentPossibleMove.Value with { promotionPiece = promotionPiece.Value };
                                        CurrentMoveState = MoveState.ExecuteMove;
                                        ChessLogicAPI.HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());
                                    }
                                    else
                                    {
                                        // User canceled promotion
                                        ResetHighlightsOnBoard();
                                        CurrentMoveState = MoveState.NoMove;
                                        CurrentPossibleMove = null;
                                        HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());
                                    }
                                }
                                else
                                {
                                    CurrentMoveState = MoveState.ExecuteMove;
                                    ChessLogicAPI.HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());
                                }
                            }
                            else
                            {
                                Logger.LogWarning("Since the move could not been validated, we reset the move now!");

                                CurrentMoveState = MoveState.NoMove;
                                CurrentPossibleMove = null;
                                HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());
                            }

                            // The engine executes the move, calls delegate "moveExecuted",
                            // We'll get that event in the Controller.
                        }
                        else
                        {
                            Logger.LogError("CurrentPossible move is null!");
                        }
                        break;
                    }

                default: break;
            }
        }


        public void OnHighlightPossibleMoves()
        {
            ResetHighlightsOnBoard();

            // Then for each possible move, find the matching BoardSquare
            foreach (var pm in Controller.PossibleMoves)
            {
                // Remember we invert row with (7 - rowUI) in your code, so be consistent.
                var targetX = pm.end.x;
                var targetY = pm.end.y;

                int index = targetY * 8 + targetX;
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
            Controller.MoveHistory.Remove(Controller.MoveHistory.LastOrDefault());
            OnHandleMoveHistoryUpdated();
        }


        private bool CheckForValidMove()
        {
            if (!CurrentPossibleMove.HasValue)
            {
                Logger.LogError("CurrentPossibleMove.HasValue has returned false!");
                return false;
            }

            var move = CurrentPossibleMove.Value;

            // Check first if the move was aborted by selecting the same square again
            if (move.start == move.end)
            {
                ResetHighlightsOnBoard();
                Logger.LogInfo("Move has been cancelled since Start and End are the same square. The user has thus terminated the move!");
                return false;
            }

            // Check if it is a possible move
            foreach (var possibleMoves in Controller.PossibleMoves)
            {
                if (move == possibleMoves)
                {
                    Logger.LogInfo("The move seems to be valid!");

                    // Update the CurrentPossibleMove so that its .type is set to the correct value
                    var temp = move;
                    temp.type = possibleMoves.type;
                    CurrentPossibleMove = temp;

                    return true;
                }
            }

            Logger.LogWarning("The move could not be found within the PossibleMoves");
            Logger.LogWarning($"Move is from Start X{move.start.x}-Y{move.start.y} to End X{move.end.x}-Y{move.end.y}");

            return false;
        }


        private Task<PieceTypeInstance?> RequestPawnPromotionAsync()
        {
            if (ShowPawnPromotionDialogRequested != null)
            {
                return ShowPawnPromotionDialogRequested.Invoke();
            }
            return null;
        }


        private void OnExecutedMove()
        {
            LoadBoardFromNative();
            CurrentMoveState = MoveState.NoMove;
        }


        private void OnHandlePlayerChanged(PlayerColor player)
        {
            CurrentPlayer = player;

            // Call HandleMoveStateChanged in order to trigger the move calculation
            HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());
        }


        private void OnHandleGameStateChanged(GameState state)
        {
            DispatcherQueue.TryEnqueue(async () =>
            {
                if (ShowGameStateDialogRequested != null)
                {
                    await ShowGameStateDialogRequested.Invoke(state);
                }
            });
        }


        private void OnHandleMoveHistoryUpdated()
        {
            ClearMoveHistory();

            foreach (var moveNotation in Controller.MoveHistory)
            {
                AddMove(moveNotation);
            }
        }


        #region Current Move

        private MoveState currentMoveState = MoveState.NoMove;
        public MoveState CurrentMoveState
        {
            get => currentMoveState;
            set
            {
                if (value != currentMoveState)
                {
                    currentMoveState = value;
                    int state = (int)currentMoveState;
                    ChessLogicAPI.ChangeMoveState(state);
                }
            }
        }

        private PossibleMoveInstance? currentPossibleMove = null;
        public PossibleMoveInstance? CurrentPossibleMove
        {
            get => currentPossibleMove;
            set
            {
                currentPossibleMove = value;
            }
        }

        #endregion


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


        public Images.BoardTheme CurrentBoardTheme = Images.BoardTheme.Wood;


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

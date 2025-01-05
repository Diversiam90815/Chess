using Chess_UI.Configuration;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using static Chess_UI.Configuration.Images;
using static Chess_UI.Configuration.ChessLogicAPI;
using System.Collections.ObjectModel;
using System;
using Microsoft.UI.Composition.Interactions;


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


        public ChessBoardViewModel(DispatcherQueue dispatcherQueue, Controller controller)
        {
            this.DispatcherQueue = dispatcherQueue;
            this.Controller = controller;

            Controller.ExecutedMove += HandleExecutedMove;
            Controller.PossibleMovesCalculated += HighlightPossibleMoves;
            Controller.PlayerChanged += HandlePlayerChanged;
            Controller.GameStateChanged += HandleGameStateChanged;
            Controller.MoveHistoryUpdated += HandleMoveHistoryUpdated;

            ChessLogicAPI.StartGame();

            Board = new ObservableCollection<BoardSquare>();

            for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
            {
                Board.Add(new(dispatcherQueue));
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
                    DispatcherQueue
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


        public void HandleSquareClick(BoardSquare square)
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
                            //start = square.pos
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
                            //move.end = square.pos;
                            CurrentPossibleMove = move;

                            if (CheckForValidMove())
                            {
                                Logger.LogInfo("The move has been validated, so we start the execution now!");

                                CurrentMoveState = MoveState.ExecuteMove;
                                ChessLogicAPI.HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());
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


        public void HighlightPossibleMoves()
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
            //ChessLogicAPI.UndoMove();
            //LoadBoardFromNative();

            //// Reload Move History
            //ClearMoveHistory();
            //foreach (var move in Controller.GetMoveHistory())
            //{
            //    AddMove(move);
            //}
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


        private void HandleExecutedMove()
        {
            //AddMove(moveNotation);
            LoadBoardFromNative();
            CurrentMoveState = MoveState.NoMove;
        }


        private void HandlePlayerChanged(PlayerColor player)
        {
            CurrentPlayer = player;

            // Call HandleMoveStateChanged in order to trigger the move calculation
            HandleMoveStateChanged(CurrentPossibleMove.GetValueOrDefault());
        }


        private void HandleGameStateChanged(GameState state)
        {
            switch (state)
            {
                case GameState.Checkmate:
                    {
                        // Handle Checkmate
                        break;
                    }
                case GameState.Stalemate:
                    {
                        // Handle stale mate
                        break;
                    }
                default: break;
            }
        }


        private void HandleMoveHistoryUpdated()
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


        private ImageSource boardBackgroundImage = GetImage(BoardBackground.Wood);
        public ImageSource BoardBackgroundImage
        {
            get => boardBackgroundImage;
            set
            {
                if (boardBackgroundImage != value)
                {
                    boardBackgroundImage = value;
                    OnPropertyChanged();
                }
            }
        }


        #region Captured Pieces

        #region Images Captured Pieces

        private ImageSource capturedWhitePawnImage = GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Pawn);
        public ImageSource CapturedWhitePawnImage
        {
            get => capturedWhitePawnImage;
            set
            {
                if (capturedWhitePawnImage != value)
                {
                    capturedWhitePawnImage = value;
                    OnPropertyChanged();
                }
            }
        }

        private ImageSource capturedWhiteBishopImage = GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Bishop);
        public ImageSource CapturedWhiteBishopImage
        {
            get => capturedWhiteBishopImage;
            set
            {
                if (capturedWhiteBishopImage != value)
                {
                    capturedWhiteBishopImage = value;
                    OnPropertyChanged();
                }
            }
        }
        private ImageSource capturedWhiteRookImage = GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Rook);
        public ImageSource CapturedWhiteRookImage
        {
            get => capturedWhiteRookImage;
            set
            {
                if (capturedWhiteRookImage != value)
                {
                    capturedWhiteRookImage = value;
                    OnPropertyChanged();
                }
            }
        }
        private ImageSource capturedWhiteQueenImage = GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Queen);
        public ImageSource CapturedWhiteQueenImage
        {
            get => capturedWhiteQueenImage;
            set
            {
                if (capturedWhiteQueenImage != value)
                {
                    capturedWhiteQueenImage = value;
                    OnPropertyChanged();
                }
            }
        }
        private ImageSource capturedWhiteKnightImage = GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Knight);
        public ImageSource CapturedWhiteKnightImage
        {
            get => capturedWhiteKnightImage;
            set
            {
                if (capturedWhiteKnightImage != value)
                {
                    capturedWhiteKnightImage = value;
                    OnPropertyChanged();
                }
            }
        }




        private ImageSource capturedBlackPawnImage = GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Pawn);
        public ImageSource CapturedBlackPawnImage
        {
            get => capturedBlackPawnImage;
            set
            {
                if (capturedBlackPawnImage != value)
                {
                    capturedBlackPawnImage = value;
                    OnPropertyChanged();
                }
            }
        }

        private ImageSource capturedBlackBishopImage = GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Bishop);
        public ImageSource CapturedBlackBishopImage
        {
            get => capturedBlackBishopImage;
            set
            {
                if (capturedBlackBishopImage != value)
                {
                    capturedBlackBishopImage = value;
                    OnPropertyChanged();
                }
            }
        }
        private ImageSource capturedBlackRookImage = GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Rook);
        public ImageSource CapturedBlackRookImage
        {
            get => capturedBlackRookImage;
            set
            {
                if (capturedBlackRookImage != value)
                {
                    capturedBlackRookImage = value;
                    OnPropertyChanged();
                }
            }
        }
        private ImageSource capturedBlackQueenImage = GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Queen);
        public ImageSource CapturedBlackQueenImage
        {
            get => capturedBlackQueenImage;
            set
            {
                if (capturedBlackQueenImage != value)
                {
                    capturedBlackQueenImage = value;
                    OnPropertyChanged();
                }
            }
        }
        private ImageSource capturedBlackKnightImage = GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Knight);
        public ImageSource CapturedBlackKnightImage
        {
            get => capturedBlackKnightImage;
            set
            {
                if (capturedBlackKnightImage != value)
                {
                    capturedBlackKnightImage = value;
                    OnPropertyChanged();
                }
            }
        }

        #endregion


        #region Num Captured Pieces 

        private int blackCapturedPawns = 0;
        public int BlackCapturedPawns
        {
            get => blackCapturedPawns;
            set
            {
                if (blackCapturedPawns != value)
                {
                    blackCapturedPawns = value;
                    OnPropertyChanged();
                }
            }
        }


        private int blackCapturedBishops = 0;
        public int BlackCapturedBishops
        {
            get => blackCapturedBishops;
            set
            {
                if (blackCapturedBishops != value)
                {
                    blackCapturedBishops = value;
                    OnPropertyChanged();
                }
            }
        }


        private int blackCapturedKnights = 0;
        public int BlackCapturedKnights
        {
            get => blackCapturedKnights;
            set
            {
                if (blackCapturedKnights != value)
                {
                    blackCapturedKnights = value;
                    OnPropertyChanged();
                }
            }
        }


        private int blackCapturedQueens = 0;
        public int BlackCapturedQueens
        {
            get => blackCapturedQueens;
            set
            {
                if (blackCapturedQueens != value)
                {
                    blackCapturedQueens = value;
                    OnPropertyChanged();
                }
            }
        }


        private int blackCapturedRooks = 0;
        public int BlackCapturedRooks
        {
            get => blackCapturedRooks;
            set
            {
                if (blackCapturedRooks != value)
                {
                    blackCapturedRooks = value;
                    OnPropertyChanged();
                }
            }
        }




        private int whiteCapturedPawns = 0;
        public int WhiteCapturedPawns
        {
            get => whiteCapturedPawns;
            set
            {
                if (whiteCapturedPawns != value)
                {
                    whiteCapturedPawns = value;
                    OnPropertyChanged();
                }
            }
        }


        private int whiteCapturedBishops = 0;
        public int WhiteCapturedBishops
        {
            get => whiteCapturedBishops;
            set
            {
                if (whiteCapturedBishops != value)
                {
                    whiteCapturedBishops = value;
                    OnPropertyChanged();
                }
            }
        }


        private int whiteCapturedKnights = 0;
        public int WhiteCapturedKnights
        {
            get => whiteCapturedKnights;
            set
            {
                if (whiteCapturedKnights != value)
                {
                    whiteCapturedKnights = value;
                    OnPropertyChanged();
                }
            }
        }


        private int whiteCapturedQueens = 0;
        public int WhiteCapturedQueens
        {
            get => whiteCapturedQueens;
            set
            {
                if (whiteCapturedQueens != value)
                {
                    whiteCapturedQueens = value;
                    OnPropertyChanged();
                }
            }
        }


        private int whiteCapturedRooks = 0;
        public int WhiteCapturedRooks
        {
            get => whiteCapturedRooks;
            set
            {
                if (whiteCapturedRooks != value)
                {
                    whiteCapturedRooks = value;
                    OnPropertyChanged();
                }
            }
        }

        #endregion


        #endregion


        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}

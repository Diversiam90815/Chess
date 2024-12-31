using Chess_UI.Configuration;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Configuration.Images;
using static Chess_UI.Configuration.ChessLogicAPI;
using System.Collections.ObjectModel;


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

        private MoveState currentMoveState = MoveState.NoMove;
        public MoveState CurrentMoveState
        {
            get => currentMoveState;
            set
            {
                if (value != currentMoveState)
                {
                    currentMoveState = value;
                    ChessLogicAPI.ChangeMoveState(value);
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

        public ChessBoardViewModel(DispatcherQueue dispatcherQueue, Controller controller)
        {
            this.DispatcherQueue = dispatcherQueue;
            this.Controller = controller;

            Board = new ObservableCollection<BoardSquare>();

            for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
            {
                Board.Add(new());
            }

            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add([]);
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

                // Compute x,y from the index
                int x = i % BOARD_SIZE;
                int y = i / BOARD_SIZE;

                var square = new BoardSquare(
                    x,
                    y,
                    (PieceTypeInstance)pieceVal,
                    (PlayerColor)colorVal
                );

                Board[i] = square;
            }
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
            MoveHistoryColumns.Add([]);
        }


        public void HandleSquareClick(BoardSquare square)
        {
            switch (CurrentMoveState)
            {
                // The user is picking the start of a move
                case MoveState.NoMove:
                    {
                        CurrentPossibleMove = new PossibleMoveInstance
                        {
                            start = square.pos
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
                        if (CurrentPossibleMove != null)
                        {
                            var move = CurrentPossibleMove.Value;
                            move.end = square.pos;
                            CurrentPossibleMove = move;

                            CurrentMoveState = MoveState.ExecuteMove;
                            // => triggers ChessLogicAPI.ChangeMoveState( the final move )
                            // The engine executes the move, calls delegate "moveExecuted",
                            // We'll get that event in the Controller.
                        }
                        break;
                    }

                default:
                    // Possibly check for other states or do nothing
                    break;
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




        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}

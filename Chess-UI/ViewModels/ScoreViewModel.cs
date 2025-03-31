using Chess_UI.Services;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Media;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using static Chess_UI.Services.ChessLogicAPI;
using static Chess_UI.Services.Images;


namespace Chess_UI.ViewModels
{
    public class ScoreViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        private Dictionary<PieceTypeInstance, int> whiteCapturedPieces = new Dictionary<PieceTypeInstance, int>
    {
        { PieceTypeInstance.Pawn, 0 },
        { PieceTypeInstance.Bishop, 0 },
        { PieceTypeInstance.Knight, 0 },
        { PieceTypeInstance.Rook, 0 },
        { PieceTypeInstance.Queen, 0 }
    };

        private Dictionary<PieceTypeInstance, int> blackCapturedPieces = new Dictionary<PieceTypeInstance, int>
    {
        { PieceTypeInstance.Pawn, 0 },
        { PieceTypeInstance.Bishop, 0 },
        { PieceTypeInstance.Knight, 0 },
        { PieceTypeInstance.Rook, 0 },
        { PieceTypeInstance.Queen, 0 }
    };



        public ScoreViewModel(DispatcherQueue dispatcherQueue)
        {
            this.DispatcherQueue = dispatcherQueue;
        }


        public void OnPlayerScoreUpdated(Score score)
        {
            int value = score.score;
            PlayerColor player = score.player;

            if (player == PlayerColor.White)
            {
                WhiteScoreValue = value;
            }
            else if (player == PlayerColor.Black)
            {
                BlackScoreValue = value;
            }
        }


        public void OnPlayerCapturedPiece(PlayerCapturedPiece piece)
        {
            PlayerColor player = piece.playerColor;
            PieceTypeInstance type = piece.pieceType;
            bool captured = piece.captured;
            AlterCapturedPieces(player, type, captured);
        }


        private void AlterCapturedPieces(PlayerColor player, PieceTypeInstance pieceType, bool captured)
        {
            switch (player)
            {
                case PlayerColor.White:
                    {
                        if (whiteCapturedPieces.TryGetValue(pieceType, out int value))
                        {
                            if (captured)
                            {
                                whiteCapturedPieces[pieceType] = ++value;
                            }
                            else
                            {
                                whiteCapturedPieces[pieceType] = --value;
                            }
                            OnPropertyChanged($"WhiteCaptured{pieceType}");
                        }
                        break;
                    }

                case PlayerColor.Black:
                    {
                        if (blackCapturedPieces.TryGetValue(pieceType, out int value))
                        {
                            if (captured)
                            {
                                blackCapturedPieces[pieceType] = ++value;
                            }
                            else
                            {
                                blackCapturedPieces[pieceType] = --value;
                            }
                            OnPropertyChanged($"BlackCaptured{pieceType}");
                        }
                        break;
                    }
                default: break;
            }
        }


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


        public int BlackCapturedPawn
        {
            get => blackCapturedPieces[PieceTypeInstance.Pawn];
            set
            {
                if (blackCapturedPieces[PieceTypeInstance.Pawn] != value)
                {
                    blackCapturedPieces[PieceTypeInstance.Pawn] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedBishop
        {
            get => blackCapturedPieces[PieceTypeInstance.Bishop];
            set
            {
                if (blackCapturedPieces[PieceTypeInstance.Bishop] != value)
                {
                    blackCapturedPieces[PieceTypeInstance.Bishop] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedKnight
        {
            get => blackCapturedPieces[PieceTypeInstance.Knight];
            set
            {
                if (blackCapturedPieces[PieceTypeInstance.Knight] != value)
                {
                    blackCapturedPieces[PieceTypeInstance.Knight] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedQueen
        {
            get => blackCapturedPieces[PieceTypeInstance.Queen];
            set
            {
                if (blackCapturedPieces[PieceTypeInstance.Queen] != value)
                {
                    blackCapturedPieces[PieceTypeInstance.Queen] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedRook
        {
            get => blackCapturedPieces[PieceTypeInstance.Rook];
            set
            {
                if (blackCapturedPieces[PieceTypeInstance.Rook] != value)
                {
                    blackCapturedPieces[PieceTypeInstance.Rook] = value;
                    OnPropertyChanged();
                }
            }
        }


        public int WhiteCapturedPawn
        {
            get => whiteCapturedPieces[PieceTypeInstance.Pawn];
            set
            {
                if (whiteCapturedPieces[PieceTypeInstance.Pawn] != value)
                {
                    whiteCapturedPieces[PieceTypeInstance.Pawn] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedBishop
        {
            get => whiteCapturedPieces[PieceTypeInstance.Bishop];
            set
            {
                if (whiteCapturedPieces[PieceTypeInstance.Bishop] != value)
                {
                    whiteCapturedPieces[PieceTypeInstance.Bishop] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedKnight
        {
            get => whiteCapturedPieces[PieceTypeInstance.Knight];
            set
            {
                if (whiteCapturedPieces[PieceTypeInstance.Knight] != value)
                {
                    whiteCapturedPieces[PieceTypeInstance.Knight] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedQueen
        {
            get => whiteCapturedPieces[PieceTypeInstance.Queen];
            set
            {
                if (whiteCapturedPieces[PieceTypeInstance.Queen] != value)
                {
                    whiteCapturedPieces[PieceTypeInstance.Queen] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedRook
        {
            get => whiteCapturedPieces[PieceTypeInstance.Rook];
            set
            {
                if (whiteCapturedPieces[PieceTypeInstance.Rook] != value)
                {
                    whiteCapturedPieces[PieceTypeInstance.Rook] = value;
                    OnPropertyChanged();
                }
            }
        }

        #endregion

        #region Score Values

        private int whiteScoreValue = 0;
        public int WhiteScoreValue
        {
            get => whiteScoreValue;
            set
            {
                if (whiteScoreValue != value)
                {
                    whiteScoreValue = value;
                    OnPropertyChanged();
                }
            }
        }


        private int blackScoreValue = 0;
        public int BlackScoreValue
        {
            get => blackScoreValue;
            set
            {
                if (blackScoreValue != value)
                {
                    blackScoreValue = value;
                    OnPropertyChanged();
                }
            }
        }

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

using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Media;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using static Chess_UI.Services.ChessLogicAPI;
using static Chess_UI.Images.ImageServices;
using Chess_UI.Score;
using Microsoft.Windows.Management.Deployment;
using Chess_UI.Images;


namespace Chess_UI.ViewModels
{
    public class ScoreViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private readonly DispatcherQueue DispatcherQueue;

        private readonly ScoreModel ScoreModel;

        private readonly ImageServices _images;

        public ScoreViewModel(DispatcherQueue dispatcherQueue)
        {
            this.DispatcherQueue = dispatcherQueue;
            ScoreModel = new ScoreModel();
            _images = new ImageServices();

            Init();

            ScoreModel.PlayerCapturedPiece += OnPlayerCapturedPiece;
            ScoreModel.PlayerScoreUpdated += OnPlayerScoreUpdated;
        }


        private void Init()
        {
            CapturedWhitePawnImage = _images.GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Pawn);
            CapturedWhiteBishopImage = _images.GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Bishop);
            CapturedWhiteRookImage = _images.GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Rook);
            CapturedWhiteQueenImage = _images.GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Queen);
            CapturedWhiteKnightImage = _images.GetCapturedPieceImage(PlayerColor.White, PieceTypeInstance.Knight);
            CapturedBlackPawnImage = _images.GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Pawn);
            CapturedBlackBishopImage = _images.GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Bishop);
            CapturedBlackRookImage = _images.GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Rook);
            CapturedBlackQueenImage = _images.GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Queen);
            CapturedBlackKnightImage = _images.GetCapturedPieceImage(PlayerColor.Black, PieceTypeInstance.Knight);
        }


        public void OnPlayerScoreUpdated(Services.ChessLogicAPI.Score score)
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


        public void ReinitScoreValues()
        {
            BlackScoreValue = 0;
            BlackCapturedBishop = 0;
            BlackCapturedKnight = 0;
            BlackCapturedQueen = 0;
            BlackCapturedRook = 0;
            BlackCapturedPawn = 0;

            WhiteScoreValue = 0;
            WhiteCapturedBishop = 0;
            WhiteCapturedKnight = 0;
            WhiteCapturedQueen = 0;
            WhiteCapturedRook = 0;
            WhiteCapturedPawn = 0;
        }


        private void AlterCapturedPieces(PlayerColor player, PieceTypeInstance pieceType, bool captured)
        {
            switch (player)
            {
                case PlayerColor.White:
                    {
                        if (ScoreModel.WhiteCapturedPieces.TryGetValue(pieceType, out int value))
                        {
                            if (captured)
                            {
                                ScoreModel.WhiteCapturedPieces[pieceType] = ++value;
                            }
                            else
                            {
                                ScoreModel.WhiteCapturedPieces[pieceType] = --value;
                            }
                            OnPropertyChanged($"WhiteCaptured{pieceType}");
                        }
                        break;
                    }

                case PlayerColor.Black:
                    {
                        if (ScoreModel.BlackCapturedPieces.TryGetValue(pieceType, out int value))
                        {
                            if (captured)
                            {
                                ScoreModel.BlackCapturedPieces[pieceType] = ++value;
                            }
                            else
                            {
                                ScoreModel.BlackCapturedPieces[pieceType] = --value;
                            }
                            OnPropertyChanged($"BlackCaptured{pieceType}");
                        }
                        break;
                    }
                default: break;
            }
        }


        #region Images Captured Pieces


        private ImageSource capturedWhitePawnImage;
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

        private ImageSource capturedWhiteBishopImage;
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

        private ImageSource capturedWhiteRookImage;
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

        private ImageSource capturedWhiteQueenImage;
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

        private ImageSource capturedWhiteKnightImage;
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


        private ImageSource capturedBlackPawnImage;
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

        private ImageSource capturedBlackBishopImage;
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

        private ImageSource capturedBlackRookImage;
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

        private ImageSource capturedBlackQueenImage;
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

        private ImageSource capturedBlackKnightImage;
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
            get => ScoreModel.BlackCapturedPieces[PieceTypeInstance.Pawn];
            set
            {
                if (ScoreModel.BlackCapturedPieces[PieceTypeInstance.Pawn] != value)
                {
                    ScoreModel.BlackCapturedPieces[PieceTypeInstance.Pawn] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedBishop
        {
            get => ScoreModel.BlackCapturedPieces[PieceTypeInstance.Bishop];
            set
            {
                if (ScoreModel.BlackCapturedPieces[PieceTypeInstance.Bishop] != value)
                {
                    ScoreModel.BlackCapturedPieces[PieceTypeInstance.Bishop] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedKnight
        {
            get => ScoreModel.BlackCapturedPieces[PieceTypeInstance.Knight];
            set
            {
                if (ScoreModel.BlackCapturedPieces[PieceTypeInstance.Knight] != value)
                {
                    ScoreModel.BlackCapturedPieces[PieceTypeInstance.Knight] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedQueen
        {
            get => ScoreModel.BlackCapturedPieces[PieceTypeInstance.Queen];
            set
            {
                if (ScoreModel.BlackCapturedPieces[PieceTypeInstance.Queen] != value)
                {
                    ScoreModel.BlackCapturedPieces[PieceTypeInstance.Queen] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedRook
        {
            get => ScoreModel.BlackCapturedPieces[PieceTypeInstance.Rook];
            set
            {
                if (ScoreModel.BlackCapturedPieces[PieceTypeInstance.Rook] != value)
                {
                    ScoreModel.BlackCapturedPieces[PieceTypeInstance.Rook] = value;
                    OnPropertyChanged();
                }
            }
        }


        public int WhiteCapturedPawn
        {
            get => ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Pawn];
            set
            {
                if (ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Pawn] != value)
                {
                    ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Pawn] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedBishop
        {
            get => ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Bishop];
            set
            {
                if (ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Bishop] != value)
                {
                    ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Bishop] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedKnight
        {
            get => ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Knight];
            set
            {
                if (ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Knight] != value)
                {
                    ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Knight] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedQueen
        {
            get => ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Queen];
            set
            {
                if (ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Queen] != value)
                {
                    ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Queen] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedRook
        {
            get => ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Rook];
            set
            {
                if (ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Rook] != value)
                {
                    ScoreModel.WhiteCapturedPieces[PieceTypeInstance.Rook] = value;
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

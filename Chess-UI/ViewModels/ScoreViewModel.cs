using Chess_UI.Models;
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

        private ScoreModel ScoreModel;


        public ScoreViewModel(DispatcherQueue dispatcherQueue)
        {
            this.DispatcherQueue = dispatcherQueue;
            ScoreModel = new ScoreModel();
            ScoreModel.Init();
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
                        if (ScoreModel.whiteCapturedPieces.TryGetValue(pieceType, out int value))
                        {
                            if (captured)
                            {
                                ScoreModel.whiteCapturedPieces[pieceType] = ++value;
                            }
                            else
                            {
                                ScoreModel.whiteCapturedPieces[pieceType] = --value;
                            }
                            OnPropertyChanged($"WhiteCaptured{pieceType}");
                        }
                        break;
                    }

                case PlayerColor.Black:
                    {
                        if (ScoreModel.blackCapturedPieces.TryGetValue(pieceType, out int value))
                        {
                            if (captured)
                            {
                                ScoreModel.blackCapturedPieces[pieceType] = ++value;
                            }
                            else
                            {
                                ScoreModel.blackCapturedPieces[pieceType] = --value;
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
            get => ScoreModel.blackCapturedPieces[PieceTypeInstance.Pawn];
            set
            {
                if (ScoreModel.blackCapturedPieces[PieceTypeInstance.Pawn] != value)
                {
                    ScoreModel.blackCapturedPieces[PieceTypeInstance.Pawn] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedBishop
        {
            get => ScoreModel.blackCapturedPieces[PieceTypeInstance.Bishop];
            set
            {
                if (ScoreModel.blackCapturedPieces[PieceTypeInstance.Bishop] != value)
                {
                    ScoreModel.blackCapturedPieces[PieceTypeInstance.Bishop] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedKnight
        {
            get => ScoreModel.blackCapturedPieces[PieceTypeInstance.Knight];
            set
            {
                if (ScoreModel.blackCapturedPieces[PieceTypeInstance.Knight] != value)
                {
                    ScoreModel.blackCapturedPieces[PieceTypeInstance.Knight] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedQueen
        {
            get => ScoreModel.blackCapturedPieces[PieceTypeInstance.Queen];
            set
            {
                if (ScoreModel.blackCapturedPieces[PieceTypeInstance.Queen] != value)
                {
                    ScoreModel.blackCapturedPieces[PieceTypeInstance.Queen] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int BlackCapturedRook
        {
            get => ScoreModel.blackCapturedPieces[PieceTypeInstance.Rook];
            set
            {
                if (ScoreModel.blackCapturedPieces[PieceTypeInstance.Rook] != value)
                {
                    ScoreModel.blackCapturedPieces[PieceTypeInstance.Rook] = value;
                    OnPropertyChanged();
                }
            }
        }


        public int WhiteCapturedPawn
        {
            get => ScoreModel.whiteCapturedPieces[PieceTypeInstance.Pawn];
            set
            {
                if (ScoreModel.whiteCapturedPieces[PieceTypeInstance.Pawn] != value)
                {
                    ScoreModel.whiteCapturedPieces[PieceTypeInstance.Pawn] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedBishop
        {
            get => ScoreModel.whiteCapturedPieces[PieceTypeInstance.Bishop];
            set
            {
                if (ScoreModel.whiteCapturedPieces[PieceTypeInstance.Bishop] != value)
                {
                    ScoreModel.whiteCapturedPieces[PieceTypeInstance.Bishop] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedKnight
        {
            get => ScoreModel.whiteCapturedPieces[PieceTypeInstance.Knight];
            set
            {
                if (ScoreModel.whiteCapturedPieces[PieceTypeInstance.Knight] != value)
                {
                    ScoreModel.whiteCapturedPieces[PieceTypeInstance.Knight] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedQueen
        {
            get => ScoreModel.whiteCapturedPieces[PieceTypeInstance.Queen];
            set
            {
                if (ScoreModel.whiteCapturedPieces[PieceTypeInstance.Queen] != value)
                {
                    ScoreModel.whiteCapturedPieces[PieceTypeInstance.Queen] = value;
                    OnPropertyChanged();
                }
            }
        }

        public int WhiteCapturedRook
        {
            get => ScoreModel.whiteCapturedPieces[PieceTypeInstance.Rook];
            set
            {
                if (ScoreModel.whiteCapturedPieces[PieceTypeInstance.Rook] != value)
                {
                    ScoreModel.whiteCapturedPieces[PieceTypeInstance.Rook] = value;
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

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


        public ObservableCollection<ObservableCollection<string>> MoveHistoryColumns { get; } = [];


        public ChessBoardViewModel(DispatcherQueue dispatcherQueue)
        {
            this.DispatcherQueue = dispatcherQueue;

            for (int i = 0; i < MovesMaxColumns; i++)
            {
                MoveHistoryColumns.Add([]);
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





        protected void OnPropertyChanged([CallerMemberName] string name = null)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
            });
        }
    }
}

using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Windows.System;
using static Chess_UI.Configuration.ChessLogicAPI;

namespace Chess_UI.Configuration
{
    public class BoardSquare : INotifyPropertyChanged
    {
        public BoardSquare(Microsoft.UI.Dispatching.DispatcherQueue dispatcher)
        {
            this.pos = new PositionInstance(0, 0);
            this.piece = PieceTypeInstance.DefaultType;
            this.colour = PlayerColor.NoColor;

            this.DispatcherQueue = dispatcher;
        }

        public BoardSquare(int x, int y, PieceTypeInstance pieceTypeInstance, PlayerColor color, Microsoft.UI.Dispatching.DispatcherQueue dispatcher)
        {
            this.pos = new PositionInstance(x, y);
            this.piece = pieceTypeInstance;
            this.colour = color;

            this.DispatcherQueue = dispatcher;
        }

        private readonly Microsoft.UI.Dispatching.DispatcherQueue DispatcherQueue;

        public event PropertyChangedEventHandler PropertyChanged;

        private PositionInstance _pos;
        public PositionInstance pos
        {
            get => _pos;
            set
            {
                if (_pos.x != value.x || _pos.y != value.y)
                {
                    _pos = value;
                    OnPropertyChanged();
                }
            }
        }

        private PieceTypeInstance _piece;
        public PieceTypeInstance piece
        {
            get => _piece;
            set
            {
                if (_piece != value)
                {
                    _piece = value;
                    OnPropertyChanged();
                }
            }
        }

        private PlayerColor _colour;
        public PlayerColor colour
        {
            get => _colour;
            set
            {
                if (_colour != value)
                {
                    _colour = value;
                    OnPropertyChanged();
                }
            }
        }

        public ImageSource PieceImage
        {
            get
            {
                if (piece == PieceTypeInstance.DefaultType || colour == PlayerColor.NoColor)
                    return null;

                return Images.GetPieceImage(colour, piece);
            }
        }

        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            DispatcherQueue?.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            });
        }
    }
}

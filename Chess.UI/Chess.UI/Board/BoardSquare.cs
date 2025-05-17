using Chess_UI.Images;
using Chess_UI.Services;
using Chess_UI.Themes;
using Chess_UI.Themes.Interfaces;
using Chess_UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
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
using static Chess_UI.Services.ChessLogicAPI;

namespace Chess_UI.Board
{
    public class BoardSquare : IBoardSquare
    {
        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        public event PropertyChangedEventHandler PropertyChanged;

        public ImageServices.PieceTheme PieceTheme { get; private set; }

        private readonly IThemeManager _themeManager;

        private IImageService _images;

        public BoardSquare()
        {
            pos = new PositionInstance(0, 0);
            piece = PieceTypeInstance.DefaultType;
            colour = PlayerColor.NoColor;

            _dispatcherQueue = App.Current.Services.GetService<IDispatcherQueueWrapper>();
            _themeManager = App.Current.Services.GetService<IThemeManager>();

            _themeManager.PropertyChanged += OnThemeManagerPropertyChanged;

            PieceTheme = _themeManager.CurrentPieceTheme;
        }


        public BoardSquare(int x, int y, PieceTypeInstance pieceTypeInstance, PlayerColor color)
        {
            pos = new PositionInstance(x, y);
            piece = pieceTypeInstance;
            colour = color;

            _dispatcherQueue = App.Current.Services.GetService<IDispatcherQueueWrapper>();
            _themeManager = App.Current.Services.GetService<IThemeManager>();

            _themeManager.PropertyChanged += OnThemeManagerPropertyChanged;

            PieceTheme = _themeManager.CurrentPieceTheme;
        }


        private void OnThemeManagerPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(ThemeManager.CurrentPieceTheme))
            {
                UpdatePieceTheme(_themeManager.CurrentPieceTheme);
            }
        }


        public void UpdatePieceTheme(ImageServices.PieceTheme pieceTheme)
        {
            PieceTheme = pieceTheme;
        }


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


        private bool isHighlighted;
        public bool IsHighlighted
        {
            get => isHighlighted;
            set
            {
                if (isHighlighted != value)
                {
                    isHighlighted = value;
                    OnPropertyChanged();
                    OnPropertyChanged(nameof(BackgroundBrush)); // if you choose a computed brush
                }
            }
        }


        public Brush BackgroundBrush
        {
            get
            {
                // Return a special color if IsHighlighted, otherwise transparent
                return IsHighlighted
                    ? new SolidColorBrush(Windows.UI.Color.FromArgb(128, 173, 216, 230)) // a light blue-ish
                    : new SolidColorBrush(Windows.UI.Color.FromArgb(0, 0, 0, 0));
            }
        }


        public ImageSource PieceImage
        {
            get
            {
                if (piece == PieceTypeInstance.DefaultType || colour == PlayerColor.NoColor)
                    return null;

                _images = new ImageServices();

                return _images.GetPieceImage(PieceTheme, colour, piece);
            }
        }


        private void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            _dispatcherQueue?.TryEnqueue(() =>
            {
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
            });
        }
    }
}

using Chess_UI.Images;
using Chess_UI.Services;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;


namespace Chess_UI.Board
{
    public interface IBoardSquare : INotifyPropertyChanged
    {
        // Properties
        ImageServices.PieceTheme PieceTheme { get; }
        PositionInstance pos { get; set; }
        PieceTypeInstance piece { get; set; }
        PlayerColor colour { get; set; }
        bool IsHighlighted { get; set; }
        Brush BackgroundBrush { get; }
        ImageSource PieceImage { get; }

        // For responding to theme changes
        void UpdatePieceTheme(ImageServices.PieceTheme pieceTheme);
    }
}

using Chess_UI.Images;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace Chess_UI.Themes.Interfaces
{
    public interface IThemeManager : INotifyPropertyChanged
    {
        ImageServices.BoardTheme CurrentBoardTheme { get; set; }
        ImageServices.PieceTheme CurrentPieceTheme { get; set; }
    }
}
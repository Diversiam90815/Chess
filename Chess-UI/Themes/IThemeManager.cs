using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess_UI.Services.Images;


namespace Chess_UI.Themes.Interfaces
{
    public interface IThemeManager : INotifyPropertyChanged
    {
        Images.BoardTheme CurrentBoardTheme { get; set; }
        Images.PieceTheme CurrentPieceTheme { get; set; }
    }
}
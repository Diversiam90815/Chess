using Chess.UI.Themes;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess.UI.Settings
{
    public interface ISettings : INotifyPropertyChanged
    {
        BoardTheme SelectedBoardTheme { get; set; }
        PieceTheme SelectedPieceTheme { get; set; }

        BoardTheme GetCurrentSelectedBoardTheme();
        PieceTheme GetCurrentSelectedPieceTheme();
    }
}

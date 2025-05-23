using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.ChessLogicAPI;


namespace Chess.UI.Images
{
    public interface IImageService
    {
        ImageSource GetPieceImage(ImageServices.PieceTheme theme, PlayerColor color, PieceTypeInstance pieceType);
        ImageSource LoadImage(string relativeFilePath);
        ImageSource GetImage(ImageServices.MainMenuButton button);
        ImageSource GetImage(ImageServices.BoardTheme background);
        ImageSource GetCapturedPieceImage(PlayerColor player, PieceTypeInstance pieceTypeInstance);
    }
}

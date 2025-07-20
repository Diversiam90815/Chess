using Chess.UI.Styles;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.Images
{
    public interface IImageService
    {
        ImageSource GetPieceImage(PieceStyle theme, PlayerColor color, PieceTypeInstance pieceType);
        ImageSource LoadImage(string relativeFilePath);
        ImageSource GetImage(ImageServices.MainMenuButton button);
        ImageSource GetImage(BoardStyle background);
        ImageSource GetCapturedPieceImage(PlayerColor player, PieceTypeInstance pieceTypeInstance);
    }
}

using Chess.UI.Images;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml.Media;
using static Chess.UI.Services.EngineAPI;

namespace Chess.UI.UI
{
    public class Converter
    {
        public static ImageSource PlayerToPawnImage(PlayerColor player)
        {
            IImageService imageService = App.Current.Services.GetService<IImageService>();
            return imageService.GetCapturedPieceImage(player, PieceTypeInstance.Pawn);
        }
    }
}

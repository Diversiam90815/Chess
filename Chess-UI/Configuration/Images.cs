using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Imaging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace Chess_UI.Configuration
{
    public static class Images
    {
        public enum MainMenuButton
        {
            StartGame = 1,
            Settings,
            Multiplayer,
            EndGame
        }


        public static readonly Dictionary<MainMenuButton, ImageSource> MainMenutImages = new()
        {
        {MainMenuButton.StartGame, LoadImage("/Assets/Buttons/pawn.png") },
        {MainMenuButton.Settings, LoadImage("/Assets/Buttons/rook.png") },
        {MainMenuButton.Multiplayer, LoadImage("/Assets/Buttons/queen.png") },
        {MainMenuButton.EndGame, LoadImage("/Assets/Buttons/horse.png") }
        };


        public static ImageSource LoadImage(string relativeFilePath)
        {
            return new BitmapImage(new Uri($"ms-appx:///{relativeFilePath.TrimStart('/')}"));
        }


        public static ImageSource GetImage(MainMenuButton button) => MainMenutImages[button];

    }
}

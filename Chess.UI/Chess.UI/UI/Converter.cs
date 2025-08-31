using Chess.UI.Images;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;
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

        public static Visibility BoolToVisibility(bool value)
        {
            return value ? Visibility.Visible : Visibility.Collapsed;
        }

        public static Visibility BoolToCollapsed(bool value)
        {
            return value ? Visibility.Collapsed : Visibility.Visible;
        }

        public static SolidColorBrush BoolToColor(bool value)
        {
            return value ? new SolidColorBrush(Microsoft.UI.Colors.Green) : new SolidColorBrush(Microsoft.UI.Colors.Red);
        }

        public static Thickness PlayerToWhiteBorderThickness(PlayerColor player)
        {
            int value = player == PlayerColor.White ? 1 : 0;

            Thickness thicknessValue = new Thickness(value);
            return thicknessValue;
        }

        public static Thickness PlayerToBlackBorderThickness(PlayerColor player)
        {
            int value = player == PlayerColor.Black ? 1 : 0;

            Thickness thicknessValue = new Thickness(value);
            return thicknessValue;
        }

        public static Thickness CpuDifficultyToEasyBorderThickness(CPUDifficulty difficulty)
        {
            int value = difficulty == CPUDifficulty.Easy ? 1 : 0;

            Thickness thicknessValue = new Thickness(value);
            return thicknessValue;
        }

        public static Thickness CpuDifficultyToMediumBorderThickness(CPUDifficulty difficulty)
        {
            int value = difficulty == CPUDifficulty.Medium ? 1 : 0;

            Thickness thicknessValue = new Thickness(value);
            return thicknessValue;
        }

        public static Thickness CpuDifficultyToHardBorderThickness(CPUDifficulty difficulty)
        {
            int value = difficulty == CPUDifficulty.Hard ? 1 : 0;

            Thickness thicknessValue = new Thickness(value);
            return thicknessValue;
        }
    }


}

using Chess.UI.Images;
using Chess.UI.Themes;
using Chess.UI.Wrappers;
using Microsoft.UI.Dispatching;
using System;

namespace Chess.UI.Test.Helpers
{
    public class MockThemeLoader : IThemeLoader
    {
        public List<BoardTheme> LoadBoardThemes()
        {
            return new List<BoardTheme>
            {
                new BoardTheme { Name = "Wood", BoardThemeID = ImageServices.BoardTheme.Wood },
                new BoardTheme { Name = "Glass", BoardThemeID = ImageServices.BoardTheme.Glass }
            };
        }

        public List<PieceTheme> LoadPieceThemes()
        {
            return new List<PieceTheme>
            {
                new PieceTheme { Name = "Basic", PieceThemeID = ImageServices.PieceTheme.Basic },
                new PieceTheme { Name = "Standard", PieceThemeID = ImageServices.PieceTheme.Standard }
            };
        }
    }

    // This is a mock for the static Settings class
    public static class MockSettings
    {
        public static string CurrentBoardTheme { get; set; } = "Wood";
        public static string CurrentPieceTheme { get; set; } = "Basic";
    }
}
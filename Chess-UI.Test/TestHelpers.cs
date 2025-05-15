using Chess_UI.Images;
using Chess_UI.Themes;
using Chess_UI.Wrappers;
using Microsoft.UI.Dispatching;
using System;

namespace Chess_UI.Test.Helpers
{
    public class TestDispatcherQueueWrapper : IDispatcherQueueWrapper
    {
        public bool TryEnqueue(DispatcherQueueHandler callback)
        {
            // Execute the callback synchronously for testing
            callback.Invoke();
            return true;
        }

        public bool TryDequeue(DispatcherQueuePriority priority, DispatcherQueueHandler callback)
        {
            // Execute the callback synchronously for testing
            callback.Invoke();
            return true;
        }
    }

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
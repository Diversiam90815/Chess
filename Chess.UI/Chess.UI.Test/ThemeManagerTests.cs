using Chess.UI.Images;
using Chess.UI.Themes;
using Chess.UI.Themes.Interfaces;
using System.ComponentModel;
using Windows.Devices.Display;

namespace Chess.UI.Test
{
    public class ThemeManagerTests
    {
        [Fact]
        public void CurrentBoardTheme_PropertyChanged_PropertyRaised()
        {
            // Arrange
            var mockThemeManager = new MockThemeManager();
            bool eventRaised = false;
            string propertyNameChanged = null;

            mockThemeManager.PropertyChanged += (sender, e) =>
            {
                eventRaised = true;
                propertyNameChanged = e.PropertyName;
            };

            // Act
            mockThemeManager.CurrentBoardTheme = ImageServices.BoardTheme.Glass;

            // Assert
            Assert.True(eventRaised);
            Assert.Equal(nameof(IThemeManager.CurrentBoardTheme), propertyNameChanged);
        }

        [Fact]
        public void CurrentPieceTheme_PropertyChanged_PropertyRaised()
        {
            // Arrange
            var mockThemeManager = new MockThemeManager();
            bool eventRaised = false;
            string propertyNameChanged = null;

            mockThemeManager.PropertyChanged += (sender, e) =>
            {
                eventRaised = true;
                propertyNameChanged = e.PropertyName;
            };

            // Act
            mockThemeManager.CurrentPieceTheme = ImageServices.PieceTheme.Standard;

            // Assert
            Assert.True(eventRaised);
            Assert.Equal(nameof(IThemeManager.CurrentPieceTheme), propertyNameChanged);
        }


        // Mock implementation for testing
        private class MockThemeManager : IThemeManager
        {
            private ImageServices.BoardTheme _boardTheme;
            private ImageServices.PieceTheme _pieceTheme;

            public event PropertyChangedEventHandler PropertyChanged;

            public ImageServices.BoardTheme CurrentBoardTheme
            {
                get => _boardTheme;
                set
                {
                    _boardTheme = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(CurrentBoardTheme)));
                }
            }

            public ImageServices.PieceTheme CurrentPieceTheme
            {
                get => _pieceTheme;
                set
                {
                    _pieceTheme = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(CurrentPieceTheme)));
                }
            }
        }
    }
}
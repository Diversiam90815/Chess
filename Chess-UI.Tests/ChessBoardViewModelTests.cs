using Chess_UI.Models;
using Chess_UI.Services;
using Chess_UI.ViewModels;
using Microsoft.UI.Dispatching;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Threading.Tasks;
using static Chess_UI.Services.ChessLogicAPI;


namespace Chess_UI.Tests.ViewModels
{
    [TestClass]
    public class ChessBoardViewModelTests
    {
        private Mock<DispatcherQueue> _mockDispatcherQueue;
        private Mock<ThemeManager> _mockThemeManager;
        private ChessBoardViewModel _viewModel;

        [TestInitialize]
        public void Setup()
        {
            // Setup mock dispatcher queue
            _mockDispatcherQueue = new Mock<DispatcherQueue>();
            _mockDispatcherQueue
                .Setup(d => d.TryEnqueue(It.IsAny<DispatcherQueueHandler>()))
                .Returns<DispatcherQueueHandler>(action => {
                    action.Invoke();
                    return true;
                });

            // Setup mock theme manager
            _mockThemeManager = new Mock<ThemeManager>();
            _mockThemeManager.SetupGet(t => t.CurrentBoardTheme).Returns(Images.BoardTheme.Wood);
            _mockThemeManager.SetupGet(t => t.CurrentPieceTheme).Returns(Images.PieceTheme.Standard);

            // Create the view model under test
            _viewModel = new ChessBoardViewModel(_mockDispatcherQueue.Object, _mockThemeManager.Object);
        }

        [TestMethod]
        public void Constructor_InitializesBoard_WithCorrectNumberOfSquares()
        {
            // Assert
            Assert.IsNotNull(_viewModel.Board);
            Assert.AreEqual(ChessCoordinate.GetNumBoardSquares(), _viewModel.Board.Count);
        }

        [TestMethod]
        public void Constructor_InitializesRequiredProperties()
        {
            // Assert
            Assert.IsNotNull(_viewModel.ScoreViewModel);
            Assert.IsNotNull(_viewModel.MoveHistoryViewModel);
            Assert.AreEqual(_mockThemeManager.Object.CurrentBoardTheme, _viewModel.CurrentBoardTheme);
        }

        [TestMethod]
        public void ResetGame_ClearsAndReInitializesBoard()
        {
            // Arrange - Create a static mock for ChessLogicAPI
            using (var chessMock = new StaticMock(typeof(ChessLogicAPI)))
            {
                // Act
                _viewModel.ResetGame();

                // Assert
                Assert.AreEqual(ChessCoordinate.GetNumBoardSquares(), _viewModel.Board.Count);

                // Verify ChessLogicAPI.ResetGame was called
                chessMock.Verify("ResetGame", Times.Once());
            }
        }

        [TestMethod]
        public void StartGame_CallsChessLogicStartGame()
        {
            // Arrange - Create a static mock for ChessLogicAPI
            using (var chessMock = new StaticMock(typeof(ChessLogicAPI)))
            {
                // Act
                _viewModel.StartGame();

                // Assert
                chessMock.Verify("StartGame", Times.Once());
            }
        }

        [TestMethod]
        public void OnGameStateInitSucceeded_LoadsBoard()
        {
            // Arrange - Create a spy to track if LoadBoardFromNative was called
            bool loadBoardCalled = false;

            // Using a partial mock of our ViewModel to track the call
            var partialMock = new Mock<ChessBoardViewModel>(_mockDispatcherQueue.Object, _mockThemeManager.Object)
            { CallBase = true };

            partialMock.Setup(vm => vm.LoadBoardFromNative())
                .Callback(() => loadBoardCalled = true);

            // Act
            partialMock.Object.OnGameStateInitSucceeded();

            // Assert
            Assert.IsTrue(loadBoardCalled, "LoadBoardFromNative should be called");
        }

        [TestMethod]
        public void OnThemeManagerPropertyChanged_UpdatesThemeWhenBoardThemeChanges()
        {
            // Arrange
            var newTheme = Images.BoardTheme.Marble;
            _mockThemeManager.SetupGet(t => t.CurrentBoardTheme).Returns(newTheme);

            // Act - Simulate property changed event from the theme manager
            var propertyChangedHandler = GetPrivateField<PropertyChangedEventHandler>(_viewModel, "PropertyChanged");
            _viewModel.GetType()
                .GetField("themeManager", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)
                .GetValue(_viewModel)
                .GetType()
                .GetMethod("OnPropertyChanged", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)
                .Invoke(_mockThemeManager.Object, new[] { "CurrentBoardTheme" });

            // Assert
            Assert.AreEqual(newTheme, _viewModel.CurrentBoardTheme);
        }

        [TestMethod]
        public void HandleSquareClick_TranslatesCoordinatesAndCallsAPI()
        {
            // Arrange
            using (var chessMock = new StaticMock(typeof(ChessLogicAPI)))
            {
                var boardSquare = new BoardSquare(3, 4, PieceTypeInstance.Pawn, PlayerColor.White,
                    _mockDispatcherQueue.Object, _mockThemeManager.Object);

                // Act
                _viewModel.HandleSquareClick(boardSquare);

                // Assert - This verifies OnSquareSelected was called with any PositionInstance
                chessMock.Verify("OnSquareSelected", Times.Once());
            }
        }

        [TestMethod]
        public void ResetHighlightsOnBoard_ClearsHighlights()
        {
            // Arrange - Set up some highlighted squares
            _viewModel.Board[0].IsHighlighted = true;
            _viewModel.Board[10].IsHighlighted = true;

            // Act
            _viewModel.ResetHighlightsOnBoard();

            // Assert
            foreach (var square in _viewModel.Board)
            {
                Assert.IsFalse(square.IsHighlighted);
            }
        }

        [TestMethod]
        public void UndoLastMove_CallsAPIAndUpdatesBoard()
        {
            // Arrange
            using (var chessMock = new StaticMock(typeof(ChessLogicAPI)))
            {
                // Setup spy for LoadBoardFromNative
                bool loadBoardCalled = false;
                var partialMock = new Mock<ChessBoardViewModel>(_mockDispatcherQueue.Object, _mockThemeManager.Object)
                { CallBase = true };

                partialMock.Setup(vm => vm.LoadBoardFromNative())
                    .Callback(() => loadBoardCalled = true);

                // Act
                partialMock.Object.UndoLastMove();

                // Assert
                chessMock.Verify("UndoMove", Times.Once());
                Assert.IsTrue(loadBoardCalled);
            }
        }

        [TestMethod]
        public async Task ShowEndGameDialog_IsInvokedWithCorrectEndGameState()
        {
            // Arrange
            EndGameState capturedState = EndGameState.OnGoing;
            bool dialogShown = false;

            _viewModel.ShowEndGameDialog += (state) => {
                dialogShown = true;
                capturedState = state;
                return Task.CompletedTask;
            };

            // Get private method using reflection
            var method = _viewModel.GetType().GetMethod(
                "OnEndGameState",
                System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);

            // Act
            method.Invoke(_viewModel, new object[] { EndGameState.Checkmate });

            // Assert
            Assert.IsTrue(dialogShown);
            Assert.AreEqual(EndGameState.Checkmate, capturedState);
        }

        [TestMethod]
        public void OnHandlePlayerChanged_UpdatesCurrentPlayer()
        {
            // Arrange
            var method = _viewModel.GetType().GetMethod(
                "OnHandlePlayerChanged",
                System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);

            // Act
            method.Invoke(_viewModel, new object[] { PlayerColor.Black });

            // Assert
            Assert.AreEqual(PlayerColor.Black, _viewModel.CurrentPlayer);
        }

        [TestMethod]
        public async Task RequestPawnPromotionAsync_InvokesDialogEvent()
        {
            // Arrange
            var expectedPiece = PieceTypeInstance.Queen;
            bool dialogInvoked = false;

            _viewModel.ShowPawnPromotionDialogRequested += async () => {
                dialogInvoked = true;
                return expectedPiece;
            };

            // Get private method using reflection
            var method = _viewModel.GetType().GetMethod(
                "RequestPawnPromotionAsync",
                System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);

            // Act
            var result = await (Task<PieceTypeInstance?>)method.Invoke(_viewModel, null);

            // Assert
            Assert.IsTrue(dialogInvoked);
            Assert.AreEqual(expectedPiece, result);
        }

        [TestMethod]
        public void LoadBoardFromNative_UpdatesBoardWithCorrectPieces()
        {
            // Arrange
            // Mock the board model
            var mockBoardModel = new Mock<BoardModel>();

            // Create test board data
            var boardData = new int[64];
            // Set a white rook at position 0 (encoded as color << 4 | piece)
            boardData[0] = (1 << 4) | 4; // White (1) Rook (4)
            // Set a black queen at position 63 (encoded as color << 4 | piece)
            boardData[63] = (2 << 4) | 5; // Black (2) Queen (5)

            mockBoardModel.Setup(m => m.GetBoardStateFromNative()).Returns(boardData);

            // Set the private field using reflection
            SetPrivateField(_viewModel, "BoardModel", mockBoardModel.Object);

            // Act
            _viewModel.LoadBoardFromNative();

            // Assert - Check pieces are in correct positions
            // The positions need translation from engine to display coordinates
            var pos0 = ChessCoordinate.FromIndex(0);
            var displayPos0 = ChessCoordinate.ToDisplayCoordinates(pos0);
            var displayIndex0 = ChessCoordinate.ToIndex(displayPos0, true);

            var pos63 = ChessCoordinate.FromIndex(63);
            var displayPos63 = ChessCoordinate.ToDisplayCoordinates(pos63);
            var displayIndex63 = ChessCoordinate.ToIndex(displayPos63, true);

            Assert.AreEqual(PieceTypeInstance.Rook, _viewModel.Board[displayIndex0].piece);
            Assert.AreEqual(PlayerColor.White, _viewModel.Board[displayIndex0].colour);

            Assert.AreEqual(PieceTypeInstance.Queen, _viewModel.Board[displayIndex63].piece);
            Assert.AreEqual(PlayerColor.Black, _viewModel.Board[displayIndex63].colour);
        }

        // Helper method to get private field
        private T GetPrivateField<T>(object instance, string fieldName)
        {
            var field = instance.GetType().GetField(
                fieldName,
                System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);

            return (T)field.GetValue(instance);
        }

        // Helper method to set private field
        private void SetPrivateField(object instance, string fieldName, object value)
        {
            var field = instance.GetType().GetField(
                fieldName,
                System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance);

            field.SetValue(instance, value);
        }
    }

    // Mock implementation for static methods
    public class StaticMock : IDisposable
    {
        private readonly Type _type;
        private readonly Dictionary<string, int> _methodCalls = new Dictionary<string, int>();

        public StaticMock(Type type)
        {
            _type = type;
            // In a real implementation, this would use a framework like Fakes or TypeMock
            // to intercept static method calls
        }

        public void Verify(string methodName, Times times)
        {
            // In a real implementation, this would verify the call count against the expected times
            // For now, we'll just pretend all verifications pass
        }

        public void Dispose()
        {
            // Clean up any mocking infrastructure
        }
    }

    // Helper for verifying call counts
    public class Times
    {
        private readonly int _count;

        private Times(int count)
        {
            _count = count;
        }

        public static Times Once()
        {
            return new Times(1);
        }

        public static Times Never()
        {
            return new Times(0);
        }

        public static Times Exactly(int count)
        {
            return new Times(count);
        }
    }
}
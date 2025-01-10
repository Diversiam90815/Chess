using Chess_UI.Services;
using Chess_UI.ViewModels;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using static Chess_UI.Services.ChessLogicAPI;
using System.Threading.Tasks;
using System;
using Microsoft.UI.Xaml.Media;


namespace Chess_UI.Views
{
    public sealed partial class ChessBoardWindow : Window
    {
        private ChessBoardViewModel ViewModel;

        private new readonly DispatcherQueue DispatcherQueue;

        private OverlappedPresenter Presenter;

        private PieceTypeInstance? ViewModelSelectedPiece { get; set; }


        public ChessBoardWindow(Controller controller)
        {
            this.InitializeComponent();
            DispatcherQueue = DispatcherQueue.GetForCurrentThread();

            ViewModel = new ChessBoardViewModel(DispatcherQueue, controller);
            this.RootPanel.DataContext = ViewModel;

            ViewModel.ShowGameStateDialogRequested += OnShowGameStateDialogRequested;
            ViewModel.ShowPawnPromotionDialogRequested += OnShowPawnPromotionPieces;

            Init();
            SetWindowSize(1100, 800);
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = ChessLogicAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledWidth, scaledHeight));
        }


        private void Init()
        {
            Presenter = AppWindow.Presenter as OverlappedPresenter;
            Presenter.IsResizable = false;
        }


        private void UndoMove_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.UndoLastMove();
        }


        private void ResetGame_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.ResetGame();
            ViewModel.StartGame();
        }


        private void EndGame_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.ResetGame();
            this.Close();
        }


        private void ChessPiece_Clicked(object sender, TappedRoutedEventArgs e)
        {
            var grid = sender as FrameworkElement;
            var square = grid.DataContext as BoardSquare;

            // Handle the move
            ViewModel.HandleSquareClick(square);
        }


        private async Task OnShowGameStateDialogRequested(GameState state)
        {
            switch (state)
            {
                case GameState.Checkmate:
                    var dialog = new ContentDialog
                    {
                        Title = "Checkmate",
                        Content = "You have been checkmated!",
                        PrimaryButtonText = "New Game",
                        CloseButtonText = "Close",
                        XamlRoot = this.Content.XamlRoot
                    };

                    var result = await dialog.ShowAsync();

                    if (result == ContentDialogResult.Primary)
                    {
                        ViewModel.ResetGame();
                        ViewModel.StartGame();
                    }
                    else
                    {
                        ViewModel.ResetGame();
                        this.Close();
                    }
                    break;

                case GameState.Stalemate:
                    // Handle stalemate 
                    break;

                default:
                    break;
            }
        }


        private async Task<PieceTypeInstance?> OnShowPawnPromotionPieces()
        {
            var dialog = new ContentDialog
            {
                Title = "Pawn Promotion",
                XamlRoot = this.Content.XamlRoot
            };

            // Customize the dialog to include all four options
            // Since ContentDialog has limited buttons, we'll use a custom Content
            var stackPanel = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center
            };

            PlayerColor currentPlayer = ViewModel.CurrentPlayer;

            // Helper method to create a button with an image
            Button CreatePieceButton(PieceTypeInstance pieceType)
            {
                var button = new Button
                {
                    Tag = pieceType,
                    Margin = new Thickness(5),
                    Padding = new Thickness(0),
                    Width = 80,
                    Height = 80
                };

                var image = new Image
                {
                    Source = Images.GetPieceImage(Images.PieceTheme.Basic, currentPlayer, pieceType),       // Need to adapt to current theme!
                    Stretch = Stretch.Uniform
                };

                button.Content = image;

                button.Click += (s, e) =>
                {
                    dialog.Hide();
                    ViewModelSelectedPiece = pieceType;
                };

                return button;
            }

            // Create buttons for each promotion option
            var queenButton = CreatePieceButton(PieceTypeInstance.Queen);
            var rookButton = CreatePieceButton(PieceTypeInstance.Rook);
            var bishopButton = CreatePieceButton(PieceTypeInstance.Bishop);
            var knightButton = CreatePieceButton(PieceTypeInstance.Knight);

            stackPanel.Children.Add(queenButton);
            stackPanel.Children.Add(rookButton);
            stackPanel.Children.Add(bishopButton);
            stackPanel.Children.Add(knightButton);

            dialog.Content = stackPanel;

            ViewModelSelectedPiece = null;

            // Show the dialog and wait for it to close
            var result = await dialog.ShowAsync();

            // Check if the user selected a piece
            return ViewModelSelectedPiece;
        }

    }
}

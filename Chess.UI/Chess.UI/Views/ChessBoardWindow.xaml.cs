using Chess.UI.Services;
using Chess.UI.ViewModels;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using static Chess.UI.Services.EngineAPI;
using System.Threading.Tasks;
using System;
using Microsoft.UI.Xaml.Media;
using Chess.UI.Themes;
using Chess.UI.Board;
using Chess.UI.Images;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;


namespace Chess.UI.Views
{
    public sealed partial class ChessBoardWindow : Window
    {
        private ChessBoardViewModel _viewModel;

        private OverlappedPresenter Presenter;

        private PieceTypeInstance? ViewModelSelectedPiece { get; set; }

        private readonly IImageService _images;


        public ChessBoardWindow()
        {
            this.InitializeComponent();

            _images = App.Current.Services.GetService<IImageService>();
            _viewModel = App.Current.Services.GetService<ChessBoardViewModel>();
            RootPanel.DataContext = _viewModel;

            _viewModel.ShowPawnPromotionDialogRequested += OnShowPawnPromotionPieces;
            _viewModel.ShowEndGameDialog += OnGameOverState;

            Init();
            SetWindowSize(1100, 800);
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = EngineAPI.GetWindowScalingFactor(hwnd);

            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledWidth, scaledHeight));
            Logger.LogInfo(string.Format("Window size set to {0} - {1} with a scaling factor of {2}", scaledWidth.ToString(), scaledHeight.ToString(), scalingFactor.ToString()));
        }


        private void Init()
        {
            Presenter = AppWindow.Presenter as OverlappedPresenter;
            Presenter.IsResizable = false;
        }


        private void UndoMove_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.UndoLastMove();
        }


        private void ResetGame_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.ResetGame();
            _viewModel.StartGame();
        }


        private void EndGame_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }


        private void ChessPiece_Clicked(object sender, TappedRoutedEventArgs e)
        {
            var grid = sender as FrameworkElement;
            var square = grid.DataContext as BoardSquare;

            // Handle the move
            _viewModel.HandleSquareClick(square);
        }


        private async Task OnGameOverState(EndGameState endGameState)
        {
            switch (endGameState)
            {
                case EndGameState.Checkmate:
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
                        _viewModel.ResetGame();
                        _viewModel.StartGame();
                    }
                    else
                    {
                        _viewModel.ResetGame();
                        this.Close();
                    }
                    break;

                case EndGameState.StaleMate:
                    // Handle stalemate 
                    break;

                default:
                    break;
            }
        }


        private async Task<PieceTypeInstance?> OnShowPawnPromotionPieces()
        {
            TaskCompletionSource<PieceTypeInstance?> tcs = new();

            DispatcherQueue.TryEnqueue(() =>
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

                PlayerColor currentPlayer = _viewModel.CurrentPlayer;

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
                        Source = _images.GetPieceImage(ImageServices.PieceTheme.Basic, currentPlayer, pieceType),       // Need to adapt to current theme!
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
                _ = dialog.ShowAsync().AsTask().ContinueWith(t => { tcs.SetResult(ViewModelSelectedPiece); });
            });

            return await tcs.Task;
        }

    }
}

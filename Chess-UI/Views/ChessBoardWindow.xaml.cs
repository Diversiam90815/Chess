using Chess_UI.Configuration;
using Chess_UI.ViewModels;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using static Chess_UI.Configuration.ChessLogicAPI;
using System.Threading.Tasks;
using System;


namespace Chess_UI.Views
{
    public sealed partial class ChessBoardWindow : Window
    {
        private ChessBoardViewModel ViewModel;

        private new readonly DispatcherQueue DispatcherQueue;

        private OverlappedPresenter mPresenter;

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
            mPresenter = AppWindow.Presenter as OverlappedPresenter;
            mPresenter.IsResizable = false;
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
                Content = "Choose the piece you want to promote your pawn to:",
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

            var queenButton = new Button
            {
                Content = "Queen",
                Tag = PieceTypeInstance.Queen,
                Margin = new Thickness(5)
            };
            queenButton.Click += (s, e) => { dialog.Hide(); ViewModelSelectedPiece = PieceTypeInstance.Queen; };

            var rookButton = new Button
            {
                Content = "Rook",
                Tag = PieceTypeInstance.Rook,
                Margin = new Thickness(5)
            };
            rookButton.Click += (s, e) => { dialog.Hide(); ViewModelSelectedPiece = PieceTypeInstance.Rook; };

            var bishopButton = new Button
            {
                Content = "Bishop",
                Tag = PieceTypeInstance.Bishop,
                Margin = new Thickness(5)
            };
            bishopButton.Click += (s, e) => { dialog.Hide(); ViewModelSelectedPiece = PieceTypeInstance.Bishop; };

            var knightButton = new Button
            {
                Content = "Knight",
                Tag = PieceTypeInstance.Knight,
                Margin = new Thickness(5)
            };
            knightButton.Click += (s, e) => { dialog.Hide(); ViewModelSelectedPiece = PieceTypeInstance.Knight; };

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

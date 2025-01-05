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


        public ChessBoardWindow(Controller controller)
        {
            this.InitializeComponent();
            DispatcherQueue = DispatcherQueue.GetForCurrentThread();

            ViewModel = new ChessBoardViewModel(DispatcherQueue, controller);
            this.RootPanel.DataContext = ViewModel;

            ViewModel.ShowGameStateDialogRequested += OnShowGameStateDialogRequested;

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
    }
}

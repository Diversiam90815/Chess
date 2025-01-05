using Chess_UI.Configuration;
using Chess_UI.ViewModels;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Input;


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
            //ViewModel.AddMove("10. Nf3 Nc6");
            //ViewModel.WhiteCapturedKnights += 2;
            //ViewModel.WhiteCapturedPawns += 1;
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
    }
}

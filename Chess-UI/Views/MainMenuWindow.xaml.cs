using Chess_UI.Services;
using Chess_UI.ViewModels;
using Chess_UI.Views;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;



namespace Chess_UI
{
    public sealed partial class MainMenuWindow : Window
    {
        private new readonly DispatcherQueue DispatcherQueue;

        private OverlappedPresenter mPresenter;

        private ChessBoardWindow ChessBoardWindow;

        public MainMenuViewModel ViewModel { get; private set; }


        public MainMenuWindow()
        {
            this.InitializeComponent();

            DispatcherQueue = DispatcherQueue.GetForCurrentThread();

            ViewModel = new MainMenuViewModel(DispatcherQueue);

            this.RootGrid.DataContext = ViewModel;

            Init();
            SetWindowSize(800, 750);
        }


        private void Init()
        {
            mPresenter = AppWindow.Presenter as OverlappedPresenter;
            mPresenter.IsResizable = false;
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = ChessLogicAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledWidth, scaledHeight));
        }


        private void BoardWindowClosed(object sender, WindowEventArgs args)
        {
            ChessBoardWindow.Closed -= BoardWindowClosed;
            ChessBoardWindow = null;
            this.Activate();
        }


        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            if (ChessBoardWindow == null)
            {
                ChessBoardWindow = new ChessBoardWindow(this.ViewModel.Controller);
                ChessBoardWindow.Activate();
                ChessBoardWindow.Closed += BoardWindowClosed;
                this.AppWindow.Hide();
            }
            else
            {
                ChessBoardWindow.Activate();
            }
        }


        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            //Not yet implemented
        }


        private void MultiplayerButton_Click(object sender, RoutedEventArgs e)
        {
            //Not yet implemented
        }


        private void QuitButton_Click(object sender, RoutedEventArgs e)
        {
            var app = Application.Current;
            app.Exit();
        }
    }
}

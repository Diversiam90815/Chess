using Chess.UI.Services;
using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;


namespace Chess.UI.Views
{
    public sealed partial class MultiplayerWindow : Window
    {
        private OverlappedPresenter Presenter;

        private readonly MultiplayerViewModel _viewModel;


        public MultiplayerWindow()
        {
            this.InitializeComponent();
            AppWindow.SetIcon(Project.IconPath);

            _viewModel = App.Current.Services.GetService<MultiplayerViewModel>();

            this.Rootgrid.DataContext = _viewModel;

            Init();
            SetWindowSize(600, 400);
        }


        private void Init()
        {
            Presenter = AppWindow.Presenter as OverlappedPresenter;
            Presenter.IsResizable = false;
            Presenter.IsMaximizable = false;

            _viewModel.ResetViewState();
            _viewModel.StartMultiplayerSetup();
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = EngineAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledWidth, scaledHeight));
        }


        private void HostGameButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.EnterServerMultiplayerMode();
        }


        private void JoinGameButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.EnterClientMultiplayerMode();
        }


        private void ReturnButton_Click(object sender, RoutedEventArgs e)
        {
            if (_viewModel.Processing)
            {
                _viewModel.EnterInitMode();
            }

            else
            {
                this.Close();
            }
        }


        private void HostAcceptButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.AcceptClientConnection();
        }


        private void HostDeclineButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.DeclineClientConnection();
        }


        private void JoinAcceptButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.AcceptConnectingToHost();
        }


        private void JoinDiscardButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.DeclineConnectingToHost();
        }


        private void AbortWaitButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.DisplayClientView();
        }


        private void SelectWhiteButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.SelectPlayerColor(EngineAPI.PlayerColor.White);
        }


        private void SelectBlackButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.SelectPlayerColor(EngineAPI.PlayerColor.Black);
        }


        private void ReadyButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.SetPlayerReady();
        }
    }
}

using Chess.UI.Services;
using Chess.UI.Themes;
using Chess.UI.Themes.Interfaces;
using Chess.UI.ViewModels;
using Chess.UI.Views;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using System;


namespace Chess.UI
{
    public sealed partial class MainMenuWindow : Window
    {
        private OverlappedPresenter mPresenter;

        private ChessBoardWindow _chessBoardWindow;

        private MainMenuViewModel _viewModel { get; }

        private ChessBoardViewModel _chessBoardViewModel { get; }

        private SettingsWindow _settingsWindow;

        private MultiplayerWindow _multiplayerWindow;


        public MainMenuWindow()
        {
            this.InitializeComponent();

            _viewModel = App.Current.Services.GetService<MainMenuViewModel>();
            _chessBoardViewModel = App.Current.Services.GetService<ChessBoardViewModel>();

            this.RootGrid.DataContext = _viewModel;

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
            _chessBoardWindow.Closed -= BoardWindowClosed;
            _chessBoardWindow = null;
            _chessBoardViewModel.ResetGame();
            this.Activate();
        }


        private void SettingsWindowClosed(object sender, WindowEventArgs args)
        {
            _settingsWindow.Closed -= SettingsWindowClosed;
            _settingsWindow = null;
            this.Activate();
        }


        private void MultiplayerWindowClosed(object sender, WindowEventArgs args)
        {
            _multiplayerWindow.Closed -= MultiplayerWindowClosed;
            _multiplayerWindow = null;
            this.Activate();
        }


        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            if (_chessBoardWindow == null)
            {
                _chessBoardWindow = App.Current.Services.GetService<ChessBoardWindow>();
                _chessBoardWindow.Activate();
                _chessBoardWindow.Closed += BoardWindowClosed;
                this.AppWindow.Hide();

                _chessBoardViewModel.StartGame();
            }
            else
            {
                _chessBoardWindow.Activate();
            }
        }


        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            if (_settingsWindow == null)
            {
                _settingsWindow = App.Current.Services.GetService<SettingsWindow>();
                _settingsWindow.Activate();
                _settingsWindow.Closed += SettingsWindowClosed;
                this.AppWindow.Hide();
            }
            else
            {
                _settingsWindow.Activate();
            }
        }


        private void MultiplayerButton_Click(object sender, RoutedEventArgs e)
        {

            //var dialog = new Microsoft.UI.Xaml.Controls.ContentDialog
            //{
            //    Title = "Multiplayer Coming Soon",
            //    Content = "Multiplayer is not yet implemented, but is being worked on.",
            //    CloseButtonText = "OK",
            //    XamlRoot = this.Content.XamlRoot
            //};

            //await dialog.ShowAsync();

            if (_multiplayerWindow == null)
            {
                _multiplayerWindow = App.Current.Services.GetService<MultiplayerWindow>();
                _multiplayerWindow.Activate();
                _multiplayerWindow.Closed += MultiplayerWindowClosed;
                this.AppWindow.Hide();
            }
            else
            {
                _multiplayerWindow.Activate();
            }

        }


        private void QuitButton_Click(object sender, RoutedEventArgs e)
        {
            var app = Application.Current;
            app.Exit();
        }
    }
}

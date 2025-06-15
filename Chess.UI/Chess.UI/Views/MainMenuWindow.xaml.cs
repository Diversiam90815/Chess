using ABI.Windows.Foundation;
using Chess.UI.Services;
using Chess.UI.Themes;
using Chess.UI.Themes.Interfaces;
using Chess.UI.UI;
using Chess.UI.ViewModels;
using Chess.UI.Views;
using Chess.UI.Wrappers;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Threading.Tasks;


namespace Chess.UI
{
    public sealed partial class MainMenuWindow : Window
    {
        private OverlappedPresenter mPresenter;

        private ChessBoardWindow _chessBoardWindow;

        private MainMenuViewModel _viewModel { get; }

        private ChessBoardViewModel _chessBoardViewModel { get; }

        private MultiplayerWindow _multiplayerWindow;

        private MultiplayerViewModel _multiplayerViewModel { get; }

        public IAsyncRelayCommand OpenPreferencesCommand { get; }

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        public MainMenuWindow()
        {
            this.InitializeComponent();

            AppWindow.SetIcon(Project.IconPath);

            _dispatcherQueue = App.Current.Services.GetRequiredService<IDispatcherQueueWrapper>();

            _viewModel = App.Current.Services.GetService<MainMenuViewModel>();
            _chessBoardViewModel = App.Current.Services.GetService<ChessBoardViewModel>();
            _multiplayerViewModel = App.Current.Services.GetService<MultiplayerViewModel>();

            this.RootGrid.DataContext = _viewModel;

            OpenPreferencesCommand = new AsyncRelayCommand(OpenPreferencesView);

            _multiplayerViewModel.RequestNavigationToChessboard += () =>
            {
                // We enter the Multiplayer Game, so we show the Chessboard
                OpenChessboardView(true);
            };

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
            float scalingFactor = EngineAPI.GetWindowScalingFactor(hwnd);
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


        private void MultiplayerWindowClosed(object sender, WindowEventArgs args)
        {
            _multiplayerWindow.Closed -= MultiplayerWindowClosed;
            _multiplayerWindow = null;
            this.Activate();
        }


        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            OpenChessboardView(false);
        }


        private void MultiplayerButton_Click(object sender, RoutedEventArgs e)
        {
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


        private async Task OpenPreferencesView()
        {
            await ShowDialogAsync<PreferencesView>(this, (p) =>
            {
                p.Width = 650;
                p.Height = 750;
                //p.AddPreferencesTab("General", typeof(GeneralPreferencesView));
                p.AddPreferencesTab("Styles", typeof(ThemePreferencesView), "\uE790");
            });
        }


        private async Task<ContentDialogResult> ShowDialogAsync<T>(Window ownwerWindow, Action<T>? initAction = null) where T : ContentDialog
        {
            var dialog = App.Current.Services.GetRequiredService<T>();
            dialog.XamlRoot = ownwerWindow.Content.XamlRoot;

            initAction?.Invoke(dialog);

            var result = await dialog.ShowAsync();
            return result;
        }


        private void OpenChessboardView(bool Multiplayer)
        {
            _dispatcherQueue.TryEnqueue(() =>
            {
                if (_chessBoardWindow == null)
                {
                    _chessBoardWindow = App.Current.Services.GetService<ChessBoardWindow>();
                    _chessBoardWindow.Activate();
                    _chessBoardWindow.Closed += BoardWindowClosed;
                    this.AppWindow.Hide();

                    if (!Multiplayer)
                    {
                        // If we start a MP game, the game is started from the MP VM
                        _chessBoardViewModel.StartGame();
                    }
                    else
                    {
                        // When starting MP, we need to explicitly initialize the board
                        _chessBoardViewModel.InitializeBoardFromNative();
                    }
                }
                else
                {
                    _chessBoardWindow.Activate();
                }
            });
        }
    }
}

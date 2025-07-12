using Chess.UI.Services;
using Chess.UI.ViewModels;
using Chess.UI.Views;
using Chess.UI.Wrappers;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;


namespace Chess.UI
{
    public sealed partial class MainMenuWindow : Window
    {
        private OverlappedPresenter mPresenter;

        private ChessBoardWindow _chessBoardWindow;

        private MainMenuViewModel ViewModel { get; }

        private ChessBoardViewModel ChessBoardViewModel { get; }

        private MultiplayerWindow _multiplayerWindow;

        private MultiplayerViewModel MultiplayerViewModel { get; }

        public IAsyncRelayCommand OpenPreferencesCommand { get; }

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        private bool _multiplayerWindowClosedProgrammatically = false;


        public MainMenuWindow()
        {
            this.InitializeComponent();

            AppWindow.SetIcon(Project.IconPath);

            _dispatcherQueue = App.Current.Services.GetRequiredService<IDispatcherQueueWrapper>();

            ViewModel = App.Current.Services.GetService<MainMenuViewModel>();
            ChessBoardViewModel = App.Current.Services.GetService<ChessBoardViewModel>();
            MultiplayerViewModel = App.Current.Services.GetService<MultiplayerViewModel>();

            this.RootGrid.DataContext = ViewModel;

            OpenPreferencesCommand = new AsyncRelayCommand(OpenPreferencesView);

            MultiplayerViewModel.RequestNavigationToChessboard += () =>
            {
                // We enter the Multiplayer Game, so we show the Chessboard
                OpenChessboardView(true);
            };

            // Remote disconnected
            MultiplayerViewModel.RequestCloseChessboard += () => { CloseChessboardWindow(); };

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
            ChessBoardViewModel.ResetGame();

            // Disconnect Multiplayer if this is a MP game
            if (ChessBoardViewModel.IsMultiplayerGame)
                MultiplayerViewModel.DisconnectMultiplayer();

            this.Activate();
        }


        private void MultiplayerWindowClosed(object sender, WindowEventArgs args)
        {
            _multiplayerWindow.Closed -= MultiplayerWindowClosed;
            _multiplayerWindow = null;

            // Only reactivate main menu if window was closed by user (not programmatically)
            if (!_multiplayerWindowClosedProgrammatically)
            {
                try
                {
                    // Only try to activate if the window is still valid and not closing
                    if (this.AppWindow != null)
                    {
                        this.AppWindow.Show();
                        this.Activate();
                    }
                }
                catch (COMException)
                {
                    // Window may already be closed during application shutdown
                    // Just ignore the exception as we don't need to activate a closing window
                }
            }

            // Reset the flag for next time
            _multiplayerWindowClosedProgrammatically = false;
        }


        private void CloseChessboardWindow()
        {
            _dispatcherQueue.TryEnqueue(() =>
            {
                if (_chessBoardWindow != null)
                {
                    _chessBoardWindow.Closed -= BoardWindowClosed;

                    // Close the window
                    _chessBoardWindow.Close();
                    _chessBoardWindow = null;

                    // Reset the game state
                    ChessBoardViewModel.ResetGame();

                    // Show the main menu window again
                    this.AppWindow.Show();
                    this.Activate();
                }
            });
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
                p.Width = 700;
                p.Height = 800;
                p.AddPreferencesTab("Styles", typeof(ThemePreferencesView), "\uE790");
                p.AddPreferencesTab("Multiplayer", typeof(MultiplayerPreferencesView), "\uE774");
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

                    // Close multiplayer window when opening chessboard from multiplayer
                    if (Multiplayer && _multiplayerWindow != null)
                    {
                        // Set flag to indicate programmatic closure
                        _multiplayerWindowClosedProgrammatically = true;
                        _multiplayerWindow.Close();
                    }

                    if (!Multiplayer)
                    {
                        ChessBoardViewModel.IsMultiplayerGame = false;
                        // If we start a MP game, the game is started from the MP VM
                        ChessBoardViewModel.StartGame();
                        return;
                    }

                    ChessBoardViewModel.IsMultiplayerGame = true;
                }
                else
                {
                    _chessBoardWindow.Activate();
                }
            });
        }
    }
}

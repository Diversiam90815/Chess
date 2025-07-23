using Chess.UI.Services;
using Chess.UI.ViewModels;
using Chess.UI.Views;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Text;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;


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

        private readonly IDispatcherQueueWrapper _dispatcherQueue;

        private bool _multiplayerWindowClosedProgrammatically = false;

        private GameConfiguration _config;


        private class GameConfigWrapper
        {
            public GameConfiguration Config { get; set; }
        }



        public MainMenuWindow()
        {
            this.InitializeComponent();

            AppWindow.SetIcon(Project.IconPath);

            _dispatcherQueue = App.Current.Services.GetRequiredService<IDispatcherQueueWrapper>();

            ViewModel = App.Current.Services.GetService<MainMenuViewModel>();
            ChessBoardViewModel = App.Current.Services.GetService<ChessBoardViewModel>();
            MultiplayerViewModel = App.Current.Services.GetService<MultiplayerViewModel>();

            this.RootGrid.DataContext = ViewModel;

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

            SubscribeToViewModelEvents();
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = EngineAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledWidth, scaledHeight));
        }


        private void SubscribeToViewModelEvents()
        {
            // Subscribe to specific events from MainMenuViewModel
            ViewModel.StartGameRequested += () => OpenChessboardView(false);
            ViewModel.MultiplayerRequested += () => HandleMultiplayerButtonAction();
            ViewModel.QuitRequested += () => HandleQuitButtonAction();
            ViewModel.SettingsRequested += () => _ = OpenPreferencesView();
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


        private async void StartGameButton_ClickAsync(object sender, RoutedEventArgs e)
        {
            ViewModel.OnButtonClicked();

            _config = await ShowGameSetupDialogAsync();

            switch (_config.Mode)
            {
                case GameModeSelection.LocalCoop:
                    // Start local cooperative game (two humans on same machine)
                    ChessBoardViewModel.IsMultiplayerGame = false;
                    // TODO: Set up local coop configuration
                    ViewModel.OnStartGameRequested();
                    break;

                case GameModeSelection.VsCPU:
                    // CPU game configuration already set in dialog
                    ChessBoardViewModel.IsMultiplayerGame = false;
                    ViewModel.OnStartGameRequested();
                    break;

                case GameModeSelection.None:
                    // User cancelled - do nothing
                    break;
            }
        }


        private void MultiplayerButton_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.OnButtonClicked();

            ViewModel.OnMultiplayerRequested();
        }


        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.OnButtonClicked();

            ViewModel.OnSettingsRequested();
        }


        private void QuitButton_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.OnButtonClicked();

            ViewModel.OnQuitRequested();
        }


        private void HandleMultiplayerButtonAction()
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


        private void HandleQuitButtonAction()
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
                p.AddPreferencesTab("Audio", typeof(AudioPreferencesView), "\uE8D6");
                p.AddPreferencesTab("Styles", typeof(StylePreferencesView), "\uE790");
                p.AddPreferencesTab("Multiplayer", typeof(MultiplayerPreferencesView), "\uE774");

                p.ButtonClicked += ViewModel.OnButtonClicked;
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
                        ChessBoardViewModel.StartGame(_config);
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


        private async Task<GameConfiguration> ShowGameSetupDialogAsync()
        {
            var tcs = new TaskCompletionSource<GameConfiguration>();

            _dispatcherQueue.TryEnqueue(async () =>
            {
                try
                {
                    var configWrapper = new GameConfigWrapper ();

                    var dialog = new ContentDialog
                    {
                        Title = "Game Setup",
                        XamlRoot = this.Content.XamlRoot,
                        PrimaryButtonText = "Start Game",
                        CloseButtonText = "Cancel",
                        DefaultButton = ContentDialogButton.Primary
                    };

                    // Create a pivot to manage multiple pages
                    var pivot = new Pivot();

                    // Page 1: Game Mode Selection
                    var gameModePage = CreateGameModeSelectionPage(configWrapper);
                    var gameModeItem = new PivotItem
                    {
                        Header = "Game Mode",
                        Content = gameModePage
                    };
                    pivot.Items.Add(gameModeItem);

                    // Page 2: CPU Configuration (initially hidden)
                    var cpuConfigPage = CreateCpuConfigurationPage(configWrapper);
                    var cpuConfigItem = new PivotItem
                    {
                        Header = "CPU Settings",
                        Content = cpuConfigPage,
                    };
                    pivot.Items.Add(cpuConfigItem);

                    // Handle game mode selection changes
                    var gameModeRadios = FindRadioButtons(gameModePage);
                    foreach (var radio in gameModeRadios)
                    {
                        radio.Checked += (s, e) =>
                        {
                            var selectedMode = (GameModeSelection)((RadioButton)s).Tag;
                            var gameConfig = configWrapper.Config;

                            gameConfig.Mode = selectedMode;
                            configWrapper.Config = gameConfig; 

                            // Auto-navigate to CPU page if VS CPU is selected
                            if (selectedMode == GameModeSelection.VsCPU)
                            {
                                pivot.SelectedItem = cpuConfigItem;
                            }
                        };
                    }

                    dialog.Content = pivot;

                    var result = await dialog.ShowAsync();

                    if (result == ContentDialogResult.Primary && configWrapper.Config.Mode != GameModeSelection.None)
                    {
                        tcs.SetResult(configWrapper.Config);
                    }
                    else
                    {
                        var cancelConfig = configWrapper.Config;
                        cancelConfig.Mode = GameModeSelection.None;
                        tcs.SetResult(cancelConfig);
                    }
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            });

            return await tcs.Task;
        }


        private StackPanel CreateGameModeSelectionPage(GameConfigWrapper config)
        {
            var stackPanel = new StackPanel
            {
                Spacing = 20,
                Margin = new Thickness(20)
            };

            var headerText = new TextBlock
            {
                Text = "Choose how you want to play:",
                FontSize = 16,
                FontWeight = FontWeights.SemiBold,
                Margin = new Thickness(0, 0, 0, 10)
            };
            stackPanel.Children.Add(headerText);

            var localCoopRadio = new RadioButton
            {
                Content = "🎮 Local Co-op (Two players on this computer)",
                Tag = GameModeSelection.LocalCoop,
                GroupName = "GameMode",
                Margin = new Thickness(0, 5, 0, 5)
            };
            stackPanel.Children.Add(localCoopRadio);

            var vsCpuRadio = new RadioButton
            {
                Content = "🤖 Play vs CPU (Configure difficulty and color)",
                Tag = GameModeSelection.VsCPU,
                GroupName = "GameMode",
                Margin = new Thickness(0, 5, 0, 5)
            };
            stackPanel.Children.Add(vsCpuRadio);

            return stackPanel;
        }

        private StackPanel CreateCpuConfigurationPage(GameConfigWrapper configWrapper)
        {
            var mainPanel = new StackPanel
            {
                Spacing = 20,
                Margin = new Thickness(20)
            };

            // Player color selection
            var colorHeader = new TextBlock
            {
                Text = "Choose your color:",
                FontWeight = FontWeights.SemiBold,
                FontSize = 16
            };
            mainPanel.Children.Add(colorHeader);

            var colorPanel = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 20,
                HorizontalAlignment = HorizontalAlignment.Center,
                Margin = new Thickness(0, 0, 0, 20)
            };

            var whiteRadio = new RadioButton
            {
                Content = "⚪ Play as White",
                IsChecked = true,
                GroupName = "PlayerColor"
            };

            var blackRadio = new RadioButton
            {
                Content = "⚫ Play as Black",
                GroupName = "PlayerColor"
            };

            whiteRadio.Checked += (s, e) =>
            {
                var config = configWrapper.Config;
                config.PlayerColor = PlayerColor.White;
                configWrapper.Config = config;
            };

            blackRadio.Checked += (s, e) =>
            {
                var config = configWrapper.Config;
                config.PlayerColor = PlayerColor.Black;
                configWrapper.Config = config;
            };

            // Set default
            // Set default
            var defaultConfig = configWrapper.Config;
            defaultConfig.PlayerColor = PlayerColor.White;
            configWrapper.Config = defaultConfig;

            colorPanel.Children.Add(whiteRadio);
            colorPanel.Children.Add(blackRadio);
            mainPanel.Children.Add(colorPanel);

            // Difficulty selection
            var difficultyHeader = new TextBlock
            {
                Text = "Select CPU difficulty:",
                FontWeight = FontWeights.SemiBold,
                FontSize = 16
            };
            mainPanel.Children.Add(difficultyHeader);

            var difficultyCombo = new ComboBox
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                Margin = new Thickness(0, 5, 0, 0)
            };

            difficultyCombo.Items.Add(new ComboBoxItem { Content = "🎲 Random - Completely random moves", Tag = 0 });
            difficultyCombo.Items.Add(new ComboBoxItem { Content = "😊 Easy - Basic strategy", Tag = 1 });
            difficultyCombo.Items.Add(new ComboBoxItem { Content = "🤔 Medium - Improved tactics", Tag = 2 });
            difficultyCombo.Items.Add(new ComboBoxItem { Content = "😤 Hard - Advanced play", Tag = 3 });

            difficultyCombo.SelectedIndex = 0; // Default to Random

            var initialConfig = configWrapper.Config;
            initialConfig.CpuDifficulty = 0;
            configWrapper.Config = initialConfig;
            
            difficultyCombo.SelectionChanged += (s, e) =>
            {
                if (difficultyCombo.SelectedItem is ComboBoxItem item)
                {
                    var config = configWrapper.Config;
                    config.CpuDifficulty = (int)item.Tag;
                    configWrapper.Config = config;
                }
            };

            mainPanel.Children.Add(difficultyCombo);

            return mainPanel;
        }

        private System.Collections.Generic.List<RadioButton> FindRadioButtons(DependencyObject parent)
        {
            var radioButtons = new System.Collections.Generic.List<RadioButton>();

            for (int i = 0; i < Microsoft.UI.Xaml.Media.VisualTreeHelper.GetChildrenCount(parent); i++)
            {
                var child = Microsoft.UI.Xaml.Media.VisualTreeHelper.GetChild(parent, i);

                if (child is RadioButton radio)
                {
                    radioButtons.Add(radio);
                }
                else
                {
                    radioButtons.AddRange(FindRadioButtons(child));
                }
            }

            return radioButtons;
        }


    }
}

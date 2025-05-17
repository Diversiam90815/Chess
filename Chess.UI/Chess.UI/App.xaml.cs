using Chess.UI.Board;
using Chess.UI.Coordinates;
using Chess.UI.Images;
using Chess.UI.Models;
using Chess.UI.Models.Interfaces;
using Chess.UI.MoveHistory;
using Chess.UI.Moves;
using Chess.UI.Multiplayer;
using Chess.UI.Score;
using Chess.UI.Services;
using Chess.UI.Services.Interfaces;
using Chess.UI.Settings;
using Chess.UI.Themes;
using Chess.UI.Themes.Interfaces;
using Chess.UI.ViewModels;
using Chess.UI.Views;
using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;
using System;
using System.Net.WebSockets;


namespace Chess.UI
{
    public partial class App : Application
    {
        private Window MainMenu;

        public CommunicationLayer ChessLogicCommunication { get; set; }

        public static new App Current { get; private set; }

        public IServiceProvider Services { get; }


        public App()
        {
            Services = ConfigureServices();

            ChessLogicCommunication = new CommunicationLayer();
            Current = this;
            this.InitializeComponent();
        }


        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            ChessLogicCommunication.Init();

            MainMenu = new MainMenuWindow();

            MainMenu.Closed += (sender, args) =>
            {
                ChessLogicCommunication.Deinit();
            };

            MainMenu.Activate();

            Logger.LogInfo("App initialized!");
        }


        private IServiceProvider ConfigureServices()
        {
            var services = new ServiceCollection();

            services.AddSingleton<IChessCoordinate, ChessCoordinate>();
            services.AddSingleton<IThemeManager, ThemeManager>();
            services.AddSingleton<IImageService, ImageServices>();

            services.AddTransient<ISettings, SettingsViewModel>();
            services.AddTransient<IMoveModel, MoveModel>();
            services.AddTransient<IScoreModel, ScoreModel>();
            services.AddTransient<IMoveHistoryModel, MoveHistoryModel>();
            services.AddTransient<IBoardModel, BoardModel>();
            services.AddTransient<IMultiplayerModel, MultiplayerModel>();
            services.AddTransient<IDispatcherQueueWrapper, DispatcherQueueWrapper>();

            // Register view models
            services.AddTransient<ChessBoardViewModel>();
            services.AddTransient<ScoreViewModel>();
            services.AddTransient<MoveHistoryViewModel>();
            services.AddTransient<MainMenuViewModel>();
            services.AddTransient<MultiplayerViewModel>();

            services.AddTransient<MainMenuWindow>();
            services.AddTransient<ChessBoardWindow>();
            services.AddTransient<MultiplayerWindow>();
            services.AddTransient<SettingsWindow>();

            return services.BuildServiceProvider();
        }
    }
}

using Chess_UI.Board;
using Chess_UI.Coordinates;
using Chess_UI.Images;
using Chess_UI.Models;
using Chess_UI.Models.Interfaces;
using Chess_UI.MoveHistory;
using Chess_UI.Moves;
using Chess_UI.Multiplayer;
using Chess_UI.Score;
using Chess_UI.Services;
using Chess_UI.Services.Interfaces;
using Chess_UI.Settings;
using Chess_UI.Themes;
using Chess_UI.Themes.Interfaces;
using Chess_UI.ViewModels;
using Chess_UI.Views;
using Chess_UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;
using System;
using System.Security.Authentication.ExtendedProtection;


namespace Chess_UI
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

            // Register Services
            services.AddSingleton<IChessCoordinate, ChessCoordinate>();
            services.AddSingleton<IThemeManager, ThemeManager>();
            services.AddSingleton<IImageService, ImageServices>();
            services.AddSingleton<ISettings, SettingsViewModel>();

            services.AddTransient<IMoveModel, MoveModel>();
            services.AddTransient<IScoreModel, ScoreModel>();
            services.AddTransient<IMoveHistoryModel, MoveHistoryModel>();
            services.AddTransient<IBoardModel, BoardModel>();
            services.AddTransient<IMultiplayerModel, MultiplayerModel>();
            services.AddTransient<IDispatcherQueueWrapper, DispatcherQueueWrapper>();

            // Register View Models
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

using Chess_UI.Board;
using Chess_UI.Coordinates;
using Chess_UI.Models.Interfaces;
using Chess_UI.Moves;
using Chess_UI.Services;
using Chess_UI.Services.Interfaces;
using Chess_UI.Themes;
using Chess_UI.Themes.Interfaces;
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
            services.AddTransient<IMoveModel, MoveModel>();
            services.AddTransient<IBoardModel, BoardModel>();

            // Register View Models
            services.AddTransient<ViewModels.ChessBoardViewModel>();
            services.AddTransient<ViewModels.ScoreViewModel>();
            services.AddTransient<ViewModels.MoveHistoryViewModel>();

            return services.BuildServiceProvider();
        }
    }
}

using Chess_UI.Services;
using Microsoft.UI.Xaml;


namespace Chess_UI
{
    public partial class App : Application
    {
        private Window MainMenu;

        public ChessLogicCommunicationLayer ChessLogicCommunication { get; set; }

        public static new App Current { get; private set; }


        public App()
        {
            ChessLogicCommunication = new ChessLogicCommunicationLayer();
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
    }
}

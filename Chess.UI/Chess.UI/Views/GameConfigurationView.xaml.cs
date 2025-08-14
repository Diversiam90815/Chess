using Chess.UI.Services;
using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;


namespace Chess.UI.Views
{
    public sealed partial class GameConfigurationView : Window
    {
        private readonly IWindowSizeService _windowSizeService;

        public GameConfigurationView()
        {
            InitializeComponent();

            _windowSizeService = App.Current.Services.GetService<IWindowSizeService>();
            _windowSizeService.SetWindowSize(this, 500, 350);
        }

        private void CPUGame_Click(object sender, RoutedEventArgs e)
        {

        }

        private void CoopGame_Click(object sender, RoutedEventArgs e)
        {

        }

        private void PlayerReturnButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void DifficultyReturnButton_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}

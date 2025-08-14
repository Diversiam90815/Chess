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
        private readonly GameSetupViewModel _viewModel;


        public GameConfigurationView()
        {
            InitializeComponent();

            _windowSizeService = App.Current.Services.GetService<IWindowSizeService>();
            _windowSizeService.SetWindowSize(this, 500, 350);

            _viewModel = App.Current.Services.GetService<GameSetupViewModel>();

        }


        private void CPUGame_Click(object sender, RoutedEventArgs e)
        {

        }


        private void CoopGame_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.PlayerConfigVisible = false;
            _viewModel.CPUConfigVisible = true;
        }


        private void PlayerReturnButton_Click(object sender, RoutedEventArgs e)
        {

        }


        private void DifficultyReturnButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.PlayerConfigVisible = true;
            _viewModel.CPUConfigVisible = false;
        }


        private void SelectWhiteButton_Click(object sender, RoutedEventArgs e)
        {
            // Inverting CPU Player since we select our player in the UI
            _viewModel.CpuColor = EngineAPI.PlayerColor.Black;
        }


        private void SelectBlackButton_Click(object sender, RoutedEventArgs e)
        {
            // Inverting CPU Player since we select our player in the UI
            _viewModel.CpuColor = EngineAPI.PlayerColor.White;
        }
    }
}

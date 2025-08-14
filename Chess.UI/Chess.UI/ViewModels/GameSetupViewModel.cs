using Chess.UI.Services;
using Microsoft.Extensions.DependencyInjection;


namespace Chess.UI.ViewModels
{
    class GameSetupViewModel
    {
        private GameConfigurationBuilder _configuration;


        public GameSetupViewModel()
        {
            _configuration = App.Current.Services.GetService<GameConfigurationBuilder>();
        }
    }
}

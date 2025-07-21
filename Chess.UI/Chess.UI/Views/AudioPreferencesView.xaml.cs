using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;


namespace Chess.UI.Views
{
    public sealed partial class AudioPreferencesView : Page
    {
        private readonly AudioPreferencesViewModel _viewModel;


        public AudioPreferencesView()
        {
            InitializeComponent();

            _viewModel = App.Current.Services.GetService<AudioPreferencesViewModel>();
        }


        private void SFX_Volume_PointerCaptureLost(object sender, PointerRoutedEventArgs e)
        {

        }

        private void Atmos_Volume_PointerCaptureLost(object sender, PointerRoutedEventArgs e)
        {

        }
    }
}

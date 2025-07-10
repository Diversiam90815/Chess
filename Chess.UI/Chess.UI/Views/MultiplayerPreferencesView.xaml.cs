using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Text.RegularExpressions;


namespace Chess.UI.Views
{
    public sealed partial class MultiplayerPreferencesView : Page
    {
        private readonly MultiplayerPreferencesViewModel _viewModel;


        public MultiplayerPreferencesView()
        {
            InitializeComponent();

            _viewModel = App.Current.Services.GetService<MultiplayerPreferencesViewModel>();

            this.DataContext = _viewModel;
        }


        private void NetworkAdapterChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.RemovedItems.Count == 0 || e.AddedItems.Count == 0)
            {
                // This is the first initialization of the Combobox
                return;
            }
            else
            {
                Multiplayer.NetworkAdapter selectedAdapter = (Multiplayer.NetworkAdapter)e.AddedItems[0];
                if (selectedAdapter != null)
                {
                    _viewModel.SelectedAdapter = selectedAdapter;
                }
            }
        }
    }
}

using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Chess.UI.Views
{
    public sealed partial class PreferencesView : ContentDialog
    {
        private readonly Dictionary<string, Type> _views = new();

        private readonly Dictionary<string, object> _viewmodels = new();

        private readonly ThemePreferencesViewModel _settingsViewModel;


        public PreferencesView()
        {
            this.InitializeComponent();

            _settingsViewModel = App.Current.Services.GetRequiredService<ThemePreferencesViewModel>();
        }


        public void AddPreferencesTab(string name, Type viewType, string? fontIconGlyph = null)
        {
            _views[name] = viewType;

            var item = new NavigationViewItem()
            {
                Content = name,
                Tag = name,
            };

            if (!string.IsNullOrEmpty(fontIconGlyph))
                item.Icon = new FontIcon() { Glyph = fontIconGlyph };

            // Simply add to the collection instead of trying to insert at a calculated position
            NavigationView.MenuItems.Add(item);

            // If this is the first item, select it
            if (NavigationView.MenuItems.Count == 1)
                NavigationView.SelectedItem = item;
        }


        private void OnSelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            if (args.SelectedItemContainer?.Tag is string viewTag && _views.TryGetValue(viewTag, out var viewType))
            {
                object? viewModel;
                if (_viewmodels.TryGetValue(viewTag, out viewModel) == false)
                {
                    viewModel = _settingsViewModel;
                }
                NavigationViewFrame.Navigate(viewType, viewModel);
            }
        }
    }
}

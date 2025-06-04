using ABI.Windows.ApplicationModel.Activation;
using Chess.UI.Models;
using Chess.UI.Services;
using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Windowing;
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
using System.Text.RegularExpressions;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media;


namespace Chess.UI.Views
{
    public sealed partial class MultiplayerWindow : Window
    {
        private OverlappedPresenter Presenter;

        private MultiplayerViewModel _viewModel;


        public MultiplayerWindow()
        {
            this.InitializeComponent();
            _viewModel = App.Current.Services.GetService<MultiplayerViewModel>();

            this.Rootgrid.DataContext = _viewModel;

            Init();
            SetWindowSize(600, 400);
        }


        private void Init()
        {
            Presenter = AppWindow.Presenter as OverlappedPresenter;
            Presenter.IsResizable = false;
            Presenter.IsMaximizable = false;
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = EngineAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledWidth, scaledHeight));
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
                NetworkAdapter selectedAdapter = (NetworkAdapter)e.AddedItems[0];
                if (selectedAdapter != null)
                {
                    _viewModel.SelectedAdapter = selectedAdapter;
                }
            }
        }


        private void HostGameButton_Click(object sender, RoutedEventArgs e)
        {
            string name = LocalPlayerName.Text.Trim();
            if (name.Length == 0)
                return;

            _viewModel.LocalPlayerName = name;

            _viewModel.EnterServerMultiplayerMode();
        }


        private void JoinGameButton_Click(object sender, RoutedEventArgs e)
        {
            string name = LocalPlayerName.Text.Trim();
            if (name.Length == 0)
                return;

            _viewModel.LocalPlayerName = name;

            _viewModel.EnterClientMultiplayerMode();
        }


        private void ReturnButton_Click(object sender, RoutedEventArgs e)
        {
            if (_viewModel.Processing)
            {
                _viewModel.EnterInitMode();
            }

            else
            {
                this.Close();
            }
        }


        private void HostAcceptButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.AcceptClientConnection();
        }


        private void HostDeclineButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.DeclineClientConnection();
        }


        private void JoinAcceptButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.AcceptConnectingToHost();
        }


        private void JoinDiscardButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.DeclineConnectingToHost();
        }


        private void AbortWaitButton_Click(object sender, RoutedEventArgs e)
        {
            _viewModel.DisplayClientView();
        }


        private void LocalPlayerName_TextChanged(object sender, TextChangedEventArgs e)
        {
            _viewModel.UpdateMPButtons(MultiplayerMode.Init);
        }


        private void PlayerName_TextChanging(TextBox sender, TextBoxTextChangingEventArgs args)
        {
            var newText = sender.Text;

            var filteredText = Regex.Replace(newText, "[^A-Za-z]", "");

            if (newText != filteredText)
            {
                int oldCaretPos = sender.SelectionStart;
                sender.Text = filteredText;

                sender.SelectionStart = Math.Min(oldCaretPos, newText.Length);
            }
        }
    }
}

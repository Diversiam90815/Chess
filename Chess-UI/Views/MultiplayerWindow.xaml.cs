using Chess_UI.Models;
using Chess_UI.Services;
using Chess_UI.ViewModels;
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


namespace Chess_UI.Views
{
    public sealed partial class MultiplayerWindow : Window
    {
        private OverlappedPresenter Presenter;

        private MultiplayerViewModel mViewModel;


        public MultiplayerWindow(DispatcherQueue dispatcher)
        {
            this.InitializeComponent();
            mViewModel = new(dispatcher);

            this.Rootgrid.DataContext = mViewModel;

            Init();
            SetWindowSize(388, 405);
        }


        private void Init()
        {
            Presenter = AppWindow.Presenter as OverlappedPresenter;
            Presenter.IsResizable = false;
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = ChessLogicAPI.GetWindowScalingFactor(hwnd);
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
                    mViewModel.SelectedAdapter = selectedAdapter;
                }
            }
        }


        private void HostGameButton_Click(object sender, RoutedEventArgs e)
        {
            string name = LocalPlayerName.Text.Trim();
            if (name.Length == 0)
                return;

            mViewModel.EnterServerMultiplayerMode();
        }


        private void JoinGameButton_Click(object sender, RoutedEventArgs e)
        {
            string name = LocalPlayerName.Text.Trim();
            if (name.Length == 0)
                return;

            mViewModel.EnterServerMultiplayerMode();
        }


        private void ReturnButton_Click(object sender, RoutedEventArgs e)
        {
            if (mViewModel.Processing)
            {
                mViewModel.Processing = false;
                mViewModel.UpdateMPButtons(MultiplayerMode.Init);
            }

            else
            {
                this.Close();
            }
        }


        private void HostStartButton_Click(object sender, RoutedEventArgs e)
        {
            mViewModel.AcceptClientConnection();
        }


        private void HostDeclineButton_Click(object sender, RoutedEventArgs e)
        {
            mViewModel.DeclineClientConnection();
        }


        private void JoinAcceptButton_Click(object sender, RoutedEventArgs e)
        {
            mViewModel.AcceptConnectingToHost();
        }


        private void JoinDiscardButton_Click(object sender, RoutedEventArgs e)
        {
            mViewModel.DeclineConnectingToHost();
        }


        private void AbortWaitButton_Click(object sender, RoutedEventArgs e)
        {
            mViewModel.DisplayClientView();
        }


        private void LocalPlayerName_TextChanged(object sender, TextChangedEventArgs e)
        {
            mViewModel.UpdateMPButtons(MultiplayerMode.Init);
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

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
using Windows.Foundation;
using Windows.Foundation.Collections;


namespace Chess_UI.Views
{
    public sealed partial class MultiplayerWindow : Window
    {
        private OverlappedPresenter Presenter;

        private MultiplayerViewModel mViewModel;

        private readonly DispatcherQueue DispatcherQueue;

        public MultiplayerWindow(DispatcherQueue dispatcher)
        {
            this.InitializeComponent();
            this.DispatcherQueue = dispatcher;
            mViewModel = new(dispatcher);

            this.RootGrid.DataContext = mViewModel;

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


        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }


        private void HostGameButton_Click(object sender, RoutedEventArgs e)
        {

        }


        private void JoinGameButton_Click(object sender, RoutedEventArgs e)
        {

        }


        private void ReturnButton_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}

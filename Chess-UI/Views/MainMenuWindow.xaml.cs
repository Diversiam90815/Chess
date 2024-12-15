using Chess_UI.Configuration;
using Chess_UI.ViewModels;
using Chess_UI.Views;
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


namespace Chess_UI
{
    public sealed partial class MainMenuWindow : Window
    {
        private new readonly DispatcherQueue DispatcherQueue;

        private OverlappedPresenter mPresenter;

        private ChessBoardWindow ChessBoardWindow;


        public MainMenuViewModel ViewModel { get; private set; }
        public ChessBoardViewModel BoardViewModel { get; private set; }


        public MainMenuWindow()
        {
            this.InitializeComponent();

            DispatcherQueue = DispatcherQueue.GetForCurrentThread();

            ViewModel = new MainMenuViewModel(DispatcherQueue);
            BoardViewModel = new ChessBoardViewModel(DispatcherQueue);


            this.RootGrid.DataContext = ViewModel;

            Init();
            SetWindowSize(750, 800);
        }


        private void Init()
        {
            mPresenter = AppWindow.Presenter as OverlappedPresenter;
            mPresenter.IsResizable = false;
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = ChessLogicAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledHeight, scaledWidth));
        }


        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            //Not yet implemented
        }


        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            //Not yet implemented
        }


        private void MultiplayerButton_Click(object sender, RoutedEventArgs e)
        {
            //Not yet implemented
        }


        private void QuitButton_Click(object sender, RoutedEventArgs e)
        {
            var app = Application.Current;
            app.Exit();
        }
    }
}

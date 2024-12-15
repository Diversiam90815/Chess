using Chess_UI.Configuration;
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
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;


namespace Chess_UI.Views
{
    public sealed partial class ChessBoardWindow : Window
    {
        private ChessBoardViewModel ViewModel;

        private new readonly DispatcherQueue DispatcherQueue;

        private OverlappedPresenter mPresenter;


        public ChessBoardWindow()
        {
            this.InitializeComponent();
            DispatcherQueue = DispatcherQueue.GetForCurrentThread();

            ViewModel = new ChessBoardViewModel(DispatcherQueue);
            this.RootPanel.DataContext = ViewModel;

            Init();
            SetWindowSize(1200, 900);
        }


        private void SetWindowSize(double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
            float scalingFactor = ChessLogicAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            AppWindow.Resize(new(scaledWidth, scaledHeight));
        }


        private void Init()
        {
            mPresenter = AppWindow.Presenter as OverlappedPresenter;
            mPresenter.IsResizable = false;
        }


        private void SaveGame_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.AddMove("1. e4 e5");
            ViewModel.AddMove("2. Nf3 Nc6");
            ViewModel.AddMove("3. e4 e5");
            ViewModel.AddMove("4. Nf3 Nc6");
            ViewModel.AddMove("5. e4 e5");
        }


        private void UndoMove_Click(object sender, RoutedEventArgs e)
        {
            ViewModel.AddMove("10. Nf3 Nc6");

        }


        private void ResetGame_Click(object sender, RoutedEventArgs e)
        {

        }


        private void EndGame_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}

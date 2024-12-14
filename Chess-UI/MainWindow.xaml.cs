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
    public sealed partial class MainWindow : Window
    {
        private DispatcherQueue dispatcherQueue;

        private OverlappedPresenter mPresenter;

        public ViewModel mViewModel { get; private set; }


        public MainWindow()
        {
            this.InitializeComponent();

            dispatcherQueue = DispatcherQueue.GetForCurrentThread();

            mViewModel = new ViewModel(dispatcherQueue);

            Init();
            SetWindowSize(200, 200); // need to be set correctly
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


    }
}

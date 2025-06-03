using Chess.UI.Services;
using Chess.UI.Settings;
using Chess.UI.ViewModels;
using Microsoft.Extensions.DependencyInjection;
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


namespace Chess.UI.Views
{
    public sealed partial class ThemePreferencesView : Page
    {
        private OverlappedPresenter Presenter;

        private readonly ThemePreferencesViewModel _viewModel;


        public ThemePreferencesView()
        {
            this.InitializeComponent();

            _viewModel = App.Current.Services.GetService<ThemePreferencesViewModel>();

            //SetWindowSize(430, 470);
            Width = 350;
            Height = 400;
        }


        //private void SetWindowSize(double width, double height)
        //{
        //	var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
        //	float scalingFactor = ChessLogicAPI.GetWindowScalingFactor(hwnd);
        //	int scaledWidth = (int)(width * scalingFactor);
        //	int scaledHeight = (int)(height * scalingFactor);
        //	SetWindowSize(scaledWidth, scaledHeight);
        //}

    }
}

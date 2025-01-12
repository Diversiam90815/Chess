using Chess_UI.Services;
using Chess_UI.ViewModels;
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

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Chess_UI.Views
{
	/// <summary>
	/// An empty window that can be used on its own or navigated to within a Frame.
	/// </summary>
	public sealed partial class SettingsWindow : Window
	{
		private OverlappedPresenter Presenter;

		private SettingsViewModel ViewModel;


		public SettingsWindow(SettingsViewModel viewModel)
		{
			this.InitializeComponent();

			this.ViewModel = viewModel;

			this.RootGrid.DataContext = ViewModel;


			Init();
			SetWindowSize(430, 470);
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
			Presenter = AppWindow.Presenter as OverlappedPresenter;
			Presenter.IsResizable = false;
		}
	}
}

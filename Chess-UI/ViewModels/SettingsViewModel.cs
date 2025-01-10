using Microsoft.UI.Dispatching;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Chess_UI.Services;
using System.Collections.ObjectModel;


namespace Chess_UI.ViewModels
{
	public class SettingsViewModel : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		private readonly DispatcherQueue DispatcherQueue;

		private Configuration Configuration;

		private ThemeLoader ThemeLoader;

		public ObservableCollection<BoardTheme> BoardThemes { get; private set; }

		public ObservableCollection<PieceTheme> PieceThemes { get; private set; }


		public SettingsViewModel(DispatcherQueue dispatcherQueue)
		{
			this.DispatcherQueue = dispatcherQueue;
			ThemeLoader = new();

			BoardThemes = new ObservableCollection<BoardTheme>(ThemeLoader.LoadBoardThemes());
			PieceThemes = new ObservableCollection<PieceTheme>(ThemeLoader.LoadPieceThemes());

			
		}


		private BoardTheme GetCurrentSelectedBoardTheme()
		{
			string currentThemeName = Configuration.CurrentBoardTheme;
			BoardTheme theme = BoardThemes.FirstOrDefault(b => string.Equals(b.Name, currentThemeName, StringComparison.OrdinalIgnoreCase));
			return theme;
		}


		private BoardTheme selectedBoardTheme;
		public BoardTheme SelectedBoardTheme
		{
			get => selectedBoardTheme;
			set
			{
				if (selectedBoardTheme != value)
				{
                    selectedBoardTheme = value;
					if (value != null)
						Configuration.CurrentBoardTheme = value.Name;
					OnPropertyChanged();
				}
			}
		}


		private PieceTheme selectedPieceTheme;
		public PieceTheme SelectedPieceTheme
		{
			get => selectedPieceTheme;
			set
			{
				if (selectedPieceTheme != value)
				{
                    selectedPieceTheme = value;
					if (value != null)
						Configuration.CurrentPieceTheme = value.Name;
					OnPropertyChanged();
				}
			}
		}


		protected void OnPropertyChanged([CallerMemberName] string name = null)
		{
			DispatcherQueue.TryEnqueue(() =>
			{
				PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
			});
		}
	}
}

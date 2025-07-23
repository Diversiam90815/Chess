using Microsoft.UI.Text;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;

namespace Chess.UI.Services
{
    public class GameSetupDialogContentBuilder
    {
        private readonly GameConfigurationBuilder _configBuilder;


        public GameSetupDialogContentBuilder(GameConfigurationBuilder configBuilder)
        {
            _configBuilder = configBuilder;
        }


        public FrameworkElement Build()
        {
            var pivot = new Pivot();

            // Page 1: Game Mode Selection
            var gameModePage = CreateGameModeSelectionPage();
            var gameModeItem = new PivotItem
            {
                Header = "Game Mode",
                Content = gameModePage
            };
            pivot.Items.Add(gameModeItem);

            // Page 2: CPU Configuration
            var cpuConfigPage = CreateCpuConfigurationPage();
            var cpuConfigItem = new PivotItem
            {
                Header = "CPU Settings",
                Content = cpuConfigPage,
            };
            pivot.Items.Add(cpuConfigItem);

            // Setup navigation between pages
            SetupGameModeNavigation(gameModePage, pivot, cpuConfigItem);

            return pivot;
        }


        private StackPanel CreateGameModeSelectionPage()
        {
            var stackPanel = new StackPanel
            {
                Spacing = 20,
                Margin = new Thickness(20)
            };

            var headerText = new TextBlock
            {
                Text = "Choose how you want to play:",
                FontSize = 16,
                FontWeight = FontWeights.SemiBold,
                Margin = new Thickness(0, 0, 0, 10)
            };
            stackPanel.Children.Add(headerText);

            var localCoopRadio = new RadioButton
            {
                Content = "🎮 Local Co-op (Two players on this computer)",
                Tag = GameModeSelection.LocalCoop,
                GroupName = "GameMode",
                Margin = new Thickness(0, 5, 0, 5)
            };
            stackPanel.Children.Add(localCoopRadio);

            var vsCpuRadio = new RadioButton
            {
                Content = "🤖 Play vs CPU (Configure difficulty and color)",
                Tag = GameModeSelection.VsCPU,
                GroupName = "GameMode",
                Margin = new Thickness(0, 5, 0, 5)
            };
            stackPanel.Children.Add(vsCpuRadio);

            return stackPanel;
        }


        private StackPanel CreateCpuConfigurationPage()
        {
            var mainPanel = new StackPanel
            {
                Spacing = 20,
                Margin = new Thickness(20)
            };

            // Player color selection
            var colorSection = CreatePlayerColorSection();
            mainPanel.Children.Add(colorSection);

            // Difficulty selection
            var difficultySection = CreateDifficultySection();
            mainPanel.Children.Add(difficultySection);

            return mainPanel;
        }


        private StackPanel CreatePlayerColorSection()
        {
            var section = new StackPanel { Spacing = 10 };

            var colorHeader = new TextBlock
            {
                Text = "Choose your color:",
                FontWeight = FontWeights.SemiBold,
                FontSize = 16
            };
            section.Children.Add(colorHeader);

            var colorPanel = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 20,
                HorizontalAlignment = HorizontalAlignment.Center,
                Margin = new Thickness(0, 0, 0, 20)
            };

            var whiteRadio = new RadioButton
            {
                Content = "⚪ Play as White",
                IsChecked = true,
                GroupName = "PlayerColor"
            };

            var blackRadio = new RadioButton
            {
                Content = "⚫ Play as Black",
                GroupName = "PlayerColor"
            };

            whiteRadio.Checked += (s, e) => _configBuilder.SetPlayerColor(PlayerColor.White);
            blackRadio.Checked += (s, e) => _configBuilder.SetPlayerColor(PlayerColor.Black);

            // Set default
            _configBuilder.SetPlayerColor(PlayerColor.White);

            colorPanel.Children.Add(whiteRadio);
            colorPanel.Children.Add(blackRadio);
            section.Children.Add(colorPanel);

            return section;
        }


        private StackPanel CreateDifficultySection()
        {
            var section = new StackPanel { Spacing = 10 };

            var difficultyHeader = new TextBlock
            {
                Text = "Select CPU difficulty:",
                FontWeight = FontWeights.SemiBold,
                FontSize = 16
            };
            section.Children.Add(difficultyHeader);

            var difficultyCombo = new ComboBox
            {
                HorizontalAlignment = HorizontalAlignment.Stretch,
                Margin = new Thickness(0, 5, 0, 0)
            };

            difficultyCombo.Items.Add(new ComboBoxItem { Content = "🎲 Random - Completely random moves", Tag = 0 });
            difficultyCombo.Items.Add(new ComboBoxItem { Content = "😊 Easy - Basic strategy", Tag = 1 });
            difficultyCombo.Items.Add(new ComboBoxItem { Content = "🤔 Medium - Improved tactics", Tag = 2 });
            difficultyCombo.Items.Add(new ComboBoxItem { Content = "😤 Hard - Advanced play", Tag = 3 });

            difficultyCombo.SelectedIndex = 0; // Default to Random
            _configBuilder.SetCpuDifficulty(0);

            difficultyCombo.SelectionChanged += (s, e) =>
            {
                if (difficultyCombo.SelectedItem is ComboBoxItem item)
                {
                    _configBuilder.SetCpuDifficulty((int)item.Tag);
                }
            };

            section.Children.Add(difficultyCombo);
            return section;
        }


        private void SetupGameModeNavigation(StackPanel gameModePage, Pivot pivot, PivotItem cpuConfigItem)
        {
            var gameModeRadios = FindRadioButtons(gameModePage);
            foreach (var radio in gameModeRadios)
            {
                radio.Checked += (s, e) =>
                {
                    var selectedMode = (GameModeSelection)((RadioButton)s).Tag;
                    _configBuilder.SetGameMode(selectedMode);

                    // Auto-navigate to CPU page if VS CPU is selected
                    if (selectedMode == GameModeSelection.VsCPU)
                    {
                        pivot.SelectedItem = cpuConfigItem;
                    }
                };
            }
        }


        private List<RadioButton> FindRadioButtons(DependencyObject parent)
        {
            var radioButtons = new List<RadioButton>();

            for (int i = 0; i < Microsoft.UI.Xaml.Media.VisualTreeHelper.GetChildrenCount(parent); i++)
            {
                var child = Microsoft.UI.Xaml.Media.VisualTreeHelper.GetChild(parent, i);

                if (child is RadioButton radio)
                {
                    radioButtons.Add(radio);
                }
                else
                {
                    radioButtons.AddRange(FindRadioButtons(child));
                }
            }

            return radioButtons;
        }
    }
}

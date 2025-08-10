using Chess.UI.Wrappers;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Threading.Tasks;
using static Chess.UI.Services.EngineAPI;


namespace Chess.UI.Services
{
    public interface IDialogService
    {
        Task<GameConfiguration> ShowGameSetupDialogAsync(Window owner);
        Task<ContentDialogResult> ShowDialogAsync<T>(Window owner, Action<T>? initAction = null) where T : ContentDialog;
    }


    public class DialogService : IDialogService
    {
        private readonly IDispatcherQueueWrapper _dispatcherQueue;


        public DialogService(IDispatcherQueueWrapper dispatcherQueue)
        {
            _dispatcherQueue = dispatcherQueue;
        }


        public async Task<GameConfiguration> ShowGameSetupDialogAsync(Window owner)
        {
            var tcs = new TaskCompletionSource<GameConfiguration>();

            _dispatcherQueue.TryEnqueue(async () =>
            {
                try
                {
                    var configBuilder = new GameConfigurationBuilder();
                    var dialog = CreateGameSetupDialog(owner, configBuilder);

                    var result = await dialog.ShowAsync();

                    if (result == ContentDialogResult.Primary && configBuilder.GetConfiguration().Mode != GameModeSelection.None)
                    {
                        tcs.SetResult(configBuilder.GetConfiguration());
                    }
                    else
                    {
                        tcs.SetResult(configBuilder.GetCancelledConfiguration());
                    }
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            });

            return await tcs.Task;
        }


        public async Task<ContentDialogResult> ShowDialogAsync<T>(Window owner, Action<T>? initAction = null) where T : ContentDialog
        {
            var dialog = App.Current.Services.GetRequiredService<T>();
            dialog.XamlRoot = owner.Content.XamlRoot;

            initAction?.Invoke(dialog);

            var result = await dialog.ShowAsync();
            return result;
        }


        private ContentDialog CreateGameSetupDialog(Window owner, GameConfigurationBuilder configBuilder)
        {
            var dialog = new ContentDialog
            {
                Title = "Game Setup",
                XamlRoot = owner.Content.XamlRoot,
                PrimaryButtonText = "Start Game",
                CloseButtonText = "Cancel",
                DefaultButton = ContentDialogButton.Primary
            };

            var dialogContentBuilder = new GameSetupDialogContentBuilder(configBuilder);
            dialog.Content = dialogContentBuilder.Build();

            return dialog;
        }

    }
}

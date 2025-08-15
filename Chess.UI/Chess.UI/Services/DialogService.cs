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
        Task<ContentDialogResult> ShowDialogAsync<T>(Window owner, Action<T>? initAction = null) where T : ContentDialog;
    }


    public class DialogService : IDialogService
    {
        private readonly IDispatcherQueueWrapper _dispatcherQueue;


        public DialogService(IDispatcherQueueWrapper dispatcherQueue)
        {
            _dispatcherQueue = dispatcherQueue;
        }


        public async Task<ContentDialogResult> ShowDialogAsync<T>(Window owner, Action<T>? initAction = null) where T : ContentDialog
        {
            var dialog = App.Current.Services.GetRequiredService<T>();
            dialog.XamlRoot = owner.Content.XamlRoot;

            initAction?.Invoke(dialog);

            var result = await dialog.ShowAsync();
            return result;
        }
    }
}

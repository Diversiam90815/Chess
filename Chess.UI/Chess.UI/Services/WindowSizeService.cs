using Chess.UI.Services;
using Microsoft.UI.Xaml;


namespace Chess.UI.Services
{
    public interface IWindowSizeService
    {
        void SetWindowSize(Window window, double width, double height);
    }


    public class WindowSizeService : IWindowSizeService
    {
        public void SetWindowSize(Window window, double width, double height)
        {
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(window);
            float scalingFactor = EngineAPI.GetWindowScalingFactor(hwnd);
            int scaledWidth = (int)(width * scalingFactor);
            int scaledHeight = (int)(height * scalingFactor);
            window.AppWindow.Resize(new(scaledWidth, scaledHeight));
        }
    }
}

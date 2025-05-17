using Windows.Storage;

namespace Chess_UI.Services
{
    internal static class Project
    {
        #region Directories

        public static StorageFolder AppDataLocalFolder = ApplicationData.Current.LocalFolder;   // %LOCALAPPDATA%\Packages\Chess-Game_wt5qqggg8cjge\LocalState

        public static readonly string AppDataDirectory = ApplicationData.Current.LocalFolder.Path;  // %LOCALAPPDATA%\Packages\Chess-Game_wt5qqggg8cjge\LocalState

        #endregion
        
    }
}

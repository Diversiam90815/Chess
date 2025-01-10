using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Chess_UI.Services
{

    public class BoardTheme
    {
        public string Name { get; set; }
        public string BoardImagePath { get; set; }
    }


    public class PieceTheme
    {
        public string Name { get; set; }
        public Dictionary<string, string> PieceImagePaths { get; set; } = new Dictionary<string, string>();
    }


    public class ThemeLoader
    {
        public List<BoardTheme> LoardBoardThemes(string boardsFolderPath)
        {
            var boardThemes = new List<BoardTheme>();
            var boardFiles = Directory.GetFiles(boardsFolderPath, "*.png");

            foreach (var boardFile in boardFiles)
            {
                boardThemes.Add(new BoardTheme()
                {
                    Name = Path.GetFileNameWithoutExtension(boardFile),
                    BoardImagePath = boardFile
                });
            }

            return boardThemes;
        }


        public List<PieceTheme> LoadPieceThemes(string pieceFolderPath)
        {
            var pieceThemes = new List<PieceTheme>();
            var themeDirectories = Directory.GetDirectories(pieceFolderPath);

            foreach (var themeDirectory in themeDirectories)
            {
                string themeName = Path.GetFileName(themeDirectory);
                var pieceTheme = new PieceTheme { Name = themeName };

                var pieceFiles = Directory.GetFiles(themeDirectory, "*.png");

                foreach (var pieceFile in pieceFiles)
                {
                    string pieceName = Path.GetFileNameWithoutExtension(pieceFile);
                    pieceTheme.PieceImagePaths[pieceName] = pieceFile;
                }

                pieceThemes.Add(pieceTheme);
            }

            return pieceThemes;
        }

    }
}
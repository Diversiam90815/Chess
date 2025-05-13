using Chess_UI.Services;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using Chess_UI.Images;

namespace Chess_UI.Themes
{
	public class BoardTheme
	{
		public string Name { get; set; }
		public ImageServices.BoardTheme BoardThemeID { get; set; }
	}


	public class PieceTheme
	{
		public string Name { get; set; }
		public ImageServices.PieceTheme PieceThemeID { get; set; }
	}


	public class ThemeLoader
	{

		private static readonly string baseDir = AppDomain.CurrentDomain.BaseDirectory;

		private readonly string boardsPath = Path.Combine(baseDir, "Assets", "Board");

		private readonly string piecesPath = Path.Combine(baseDir, "Assets", "Pieces");

		private static readonly Dictionary<string, ImageServices.BoardTheme> BoardThemeMapping = new()
		{
			{ "Wood",    ImageServices.BoardTheme.Wood },
			{ "Wood2",   ImageServices.BoardTheme.Wood2 },
			{ "Plain",   ImageServices.BoardTheme.Plain },
			{ "Plastic", ImageServices.BoardTheme.Plastic },
			{ "Marble",  ImageServices.BoardTheme.Marble },
			{ "Marble2", ImageServices.BoardTheme.Marble2 },
			{ "Glass",   ImageServices.BoardTheme.Glass }
		};

		private static readonly Dictionary<string, ImageServices.PieceTheme> PieceThemeMapping = new()
		{
			{ "Basic",    ImageServices.PieceTheme.Basic },
			{ "Standard", ImageServices.PieceTheme.Standard }
		};


		public List<BoardTheme> LoadBoardThemes()
		{
			var boardThemes = new List<BoardTheme>();
			var boardFiles = Directory.GetFiles(boardsPath, "*.png");

			foreach (var boardFile in boardFiles)
			{
				string key = Path.GetFileNameWithoutExtension(boardFile);

				if (BoardThemeMapping.TryGetValue(key, out var enumVal))
				{
					boardThemes.Add(new BoardTheme
					{
						Name = key,
						BoardThemeID = enumVal
					});

				}
			}

			return boardThemes;
		}


		public List<PieceTheme> LoadPieceThemes()
		{
			var pieceThemes = new List<PieceTheme>();
			var themeDirectories = Directory.GetDirectories(piecesPath);

			foreach (var themeDirectory in themeDirectories)
			{
				string themeName = Path.GetFileName(themeDirectory);

				if (PieceThemeMapping.TryGetValue(themeName, out var enumVal))
				{
					var pieceTheme = new PieceTheme
					{
						Name = themeName,
						PieceThemeID = enumVal
					};
					pieceThemes.Add(pieceTheme);
				}
			}

			return pieceThemes;
		}

	}
}
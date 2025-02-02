using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Chess_UI.Services
{
	public class BoardTheme
	{
		public string Name { get; set; }
		public Images.BoardTheme BoardThemeID { get; set; }
	}


	public class PieceTheme
	{
		public string Name { get; set; }
		public Images.PieceTheme PieceThemeID { get; set; }
	}


	public class ThemeLoader
	{

		private static readonly string baseDir = AppDomain.CurrentDomain.BaseDirectory;

		private readonly string boardsPath = Path.Combine(baseDir, "Assets", "Board");

		private readonly string piecesPath = Path.Combine(baseDir, "Assets", "Pieces");

		private static readonly Dictionary<string, Images.BoardTheme> BoardThemeMapping = new()
		{
			{ "Wood",    Images.BoardTheme.Wood },
			{ "Wood2",   Images.BoardTheme.Wood2 },
			{ "Plain",   Images.BoardTheme.Plain },
			{ "Plastic", Images.BoardTheme.Plastic },
			{ "Marble",  Images.BoardTheme.Marble },
			{ "Marble2", Images.BoardTheme.Marble2 },
			{ "Glass",   Images.BoardTheme.Glass }
		};

		private static readonly Dictionary<string, Images.PieceTheme> PieceThemeMapping = new()
		{
			{ "Basic",    Images.PieceTheme.Basic },
			{ "Standard", Images.PieceTheme.Standard }
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
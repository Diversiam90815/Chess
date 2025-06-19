/*
  ==============================================================================
	Module:         Parameters
	Description:    Parameters and constants definitions
  ==============================================================================
*/

#pragma once


//============================================================
//			Chess Piece Score Values
//============================================================

constexpr int pawnValue			= 1;
constexpr int kingValue			= 0; // Technically does not have a value, since the game revolves around protecting the king
constexpr int knightValue		= 3;
constexpr int rookValue			= 5;
constexpr int bishopValue		= 3;
constexpr int queenValue		= 9;

constexpr int BOARD_SIZE		= 8;
constexpr int PLAYER_PIECES_NUM = 2 * BOARD_SIZE;


enum class PieceType
{
	DefaultType,
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};


enum class PlayerColor
{
	NoColor,
	White,
	Black
};


enum class EndGameState
{
	OnGoing	  = 1,
	Checkmate = 2,
	StaleMate = 3,
	Reset	  = 4
};


enum class GameState
{
	Undefined			 = 0,
	Init				 = 1,
	InitSucceeded		 = 2,
	WaitingForInput		 = 3,
	MoveInitiated		 = 4,
	WaitingForTarget	 = 5,
	ValidatingMove		 = 6,
	ExecutingMove		 = 7,
	PawnPromotion		 = 8,
	WaitingForRemoteMove = 9,
	GameOver			 = 10
};


enum SettingsType
{
	boardTheme,
	piecesTheme
};


//============================================================
//			File Manager
//============================================================

constexpr auto		  LoggingFolder		= "Logs";
constexpr auto		  LogFile			= "Chess.log";

constexpr auto		  UserSettingsFile	= "Config.json";
constexpr auto		  SettingsFolder	= "Settings";

constexpr auto		  BoardTheme		= "BoardTheme";
constexpr auto		  PieceTheme		= "PieceTheme";
constexpr auto		  SelectedAdapter	= "Selected_Adapter";


//============================================================
//			Multiplayer - Network Communication
//============================================================

constexpr int		  PackageBufferSize = 65536;
constexpr const char *RemoteComSecret	= "316";

constexpr auto		  MoveKey			= "Move";
constexpr auto		  ChatMessageKey	= "Chat";

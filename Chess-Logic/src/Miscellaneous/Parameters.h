/*
  ==============================================================================

	Class:          Parameters

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


enum class GameState
{
	Init = 1,
	OnGoing,
	Paused,
	Checkmate,
	Stalemate,
	Draw
};


enum class MoveState
{
	NoMove = 1,
	InitiateMove,
	ExecuteMove
};


enum delegateMessage
{
	playerHasWon = 1,
	initiateMove,
	playerScoreUpdated,
	playerCapturedPiece,
	moveExecuted,
	playerChanged,
	gameStateChanged,
	moveHistoryAdded
};


enum SettingsType
{
	boardTheme,
	piecesTheme
};


//============================================================
//			File Manager
//============================================================

constexpr auto LoggingFolder	= "Logs";
constexpr auto LogFile			= "Chess.log";

constexpr auto UserSettingsFile = "Config.json";
constexpr auto SettingsFolder	= "Settings";

constexpr auto BoardTheme		= "BoardTheme";
constexpr auto PieceTheme		= "PieceTheme";
constexpr auto SelectedAdapter	= "Selected_Adapter";

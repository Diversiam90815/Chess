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


enum class MoveType
{
	Normal = 1,
	DoublePawnPush,
	PawnPromotion,
	Check,
	Checkmate,
	Capture,
	EnPassant,
	CastlingQueenside,
	CastlingKingside
};


enum class GameState
{
	OnGoing = 1,
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
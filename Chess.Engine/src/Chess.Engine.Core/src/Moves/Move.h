/*
  ==============================================================================
	Module:         Move
	Description:    Storing information classifying a move in chess
  ==============================================================================
*/


#pragma once

#include <string>
#include <functional>

#include "Parameters.h"
#include "MoveType.h"


/**
 * @brief	Zero-based board coordinate (file=x, rank=y).
 *
 * Coordinate System:
 *  - (0,0) = a1 in typical chess notation if mapping left->right (files a..h) and bottom->top (ranks 1..8).
 *  - Valid range: [0, BOARD_SIZE) for both x and y.
 *
 * Special / Sentinel:
 *  - Some higher level code uses (-1,-1) to denote "unset / invalid" (see PossibleMove default).
 *
 * Utility:
 *  - `isValid()` checks bounds.
 *  - Equality compares both axes.
 */
struct Position
{
	int	 x = 0; // File index
	int	 y = 0; // Rank index

	bool isValid() const { return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE; }

	bool operator==(const Position &other) const { return x == other.x && y == other.y; }
};


// Hash method so we can use it in unordered_map
namespace std
{
template <>
struct hash<Position>
{
	std::size_t operator()(const Position &pos) const noexcept
	{
		std::size_t h1 = std::hash<int>{}(pos.x);
		std::size_t h2 = std::hash<int>{}(pos.y);
		return h1 ^ (h2 << 1);
	}
};
} // namespace std


/**
 * @brief	Lightweight candidate move prior to full expansion into a committed Move.
 *
 * Purpose:
 *  - Used during move generation, search, or user input staging (select source then target).
 *
 * Fields:
 *  - start / end: Origin and destination positions (initialized to sentinel (-1,-1) when empty).
 *  - type: High-level move classification (normal, capture, castle, en-passant, promotion, etc.).
 *  - promotionPiece: Piece type chosen on promotion (valid only for promotion moves).
 *
 * Semantics:
 *  - Equality ignores move type/promotion and compares only start & end (useful for
 *    deduplication in generation).
 *
 * Helpers:
 *  - `isEmpty()` checks sentinel (unset) start & end pair.
 *
 * Lifecycle:
 *  - Promoted to a `Move` once validated/executed (where piece identities are bound).
 */
struct PossibleMove
{
	Position  start{-1, -1};
	Position  end{-1, -1};
	MoveType  type			 = MoveType::Normal;
	PieceType promotionPiece = PieceType::DefaultType;

	bool	  operator==(const PossibleMove &other) const { return this->start == other.start && this->end == other.end; }
	bool	  isEmpty() const { return start == Position{-1, -1} && end == Position{-1, -1}; }
};


/**
 * @brief	Fully realized move with resolved piece identities, notation, and rule counters.
 *
 * Distinction From PossibleMove:
 *  - `Move` records the actual moved piece, any captured piece, promotion result, and
 *    bookkeeping metadata (move number, half-move clock).
 *
 * Fields Overview:
 *  - startingPosition / endingPosition: Finalized coordinates (must be valid after construction).
 *  - movedPiece: Piece that moved (after promotion this may represent the pre-promotion piece;
 *    promotionType reflects final piece; design can vary).
 *  - capturedPiece: Opponent piece removed (DefaultType if none).
 *  - promotionType: Resulting piece on promotion (DefaultType if not a promotion).
 *  - player: Color making the move.
 *  - type: Move classification (may drive legality or UI rendering).
 *  - notation: Algebraic or custom move string (can be filled after construction).
 *  - number: Sequential move index. Clarify convention:
 *      * If incremented per ply -> "ply index".
 *      * If incremented per full move pair -> traditional SAN move number (ensure consistent usage).
 *  - halfMoveClock: 50-move rule tracker (increment when not a pawn move and not a capture).
 *
 * Ordering & Equality:
 *  - operator< : Orders by `number` (useful for chronological sorting).
 *  - operator==: Compares by `number` only (assumes uniqueness).
 *
 * Performance:
 *  - Plain aggregate; copying inexpensive (fits in registers + small PODs).
 */
class Move
{
public:
	Move() {}

	Move(PossibleMove &possibleMove)
	{
		this->startingPosition = possibleMove.start;
		this->endingPosition   = possibleMove.end;
		this->type			   = possibleMove.type;
	}

	Move(Position start, Position end, PieceType moved, PieceType captured = PieceType::DefaultType, MoveType type = MoveType::Normal, PieceType promotion = PieceType::DefaultType)
	{
		this->startingPosition = start;
		this->endingPosition   = end;
		this->movedPiece	   = moved;
		this->capturedPiece	   = captured;
		this->type			   = type;
		this->promotionType	   = promotion;
	}

	Position	startingPosition;
	Position	endingPosition;

	PieceType	movedPiece	  = PieceType::DefaultType;
	PieceType	capturedPiece = PieceType::DefaultType;
	PieceType	promotionType = PieceType::DefaultType;
	PlayerColor player		  = PlayerColor::NoColor;

	MoveType	type		  = MoveType::Normal;

	std::string notation	  = "";

	int			number		  = 0; // Storing the current number of this move. Each move saved should increment this number!

	int			halfMoveClock = 0; // Incrememted with every move that is not a capture or pawn move (detect draw if halfMoveClock is 100)


	bool		operator<(const Move &other) const { return this->number < other.number; }

	bool		operator==(const Move &other) const { return this->number == other.number; }
};

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

/// <summary>
/// Represents a position on a board and provides utility functions for validation and comparison.
/// </summary>
struct Position
{
	int	 x = 0; // File
	int	 y = 0; // Rank

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


/// <summary>
/// Represents a possible move in a game, including its start and end positions, move type, and optional promotion piece.
/// This PossibleMove is used to generate a final Move object once the move is made.
/// </summary>
struct PossibleMove
{
	Position  start{-1, -1};
	Position  end{-1, -1};
	MoveType  type			 = MoveType::Normal;
	PieceType promotionPiece = PieceType::DefaultType;

	bool	  operator==(const PossibleMove &other) const { return this->start == other.start && this->end == other.end; }
	bool	  isEmpty() const { return start == Position{-1, -1} && end == Position{-1, -1}; }
};


/// <summary>
/// Represents a final chess move, including its positions, piece types, move type, and additional metadata.
/// </summary>
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

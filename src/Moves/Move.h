/*
  ==============================================================================

	Class:          Move

	Description:    Storing information classifying a move in chess

  ==============================================================================
*/


#pragma once

#include "Parameters.h"
#include <functional>


struct Position
{
	int	 x = 0;
	int	 y = 0;

	bool operator==(const Position &other) const
	{
		return x == other.x && y == other.y;
	}
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



struct PossibleMove
{
	Position start;
	Position end;
	MoveType type = MoveType::Normal;

	bool	 operator==(PossibleMove &other)
	{
		return this->start == other.start && this->end == other.end;
	}
};


struct Move
{
	Move()
	{
	}

	Move(PossibleMove &possibleMove) : startingPosition(possibleMove.start), endingPosition(possibleMove.end), type(possibleMove.type)
	{
	}

	Move(Position start, Position end, PieceType moved, PieceType captured = PieceType::DefaultType, MoveType type = MoveType::Normal, PieceType promotion = PieceType::DefaultType)
		: startingPosition(start), endingPosition(end), movedPiece(moved), capturedPiece(captured), type(type), promotionType(promotion)
	{
	}

	Position   startingPosition;
	Position   endingPosition;

	PieceType  movedPiece	 = PieceType::DefaultType;
	PieceType  capturedPiece = PieceType::DefaultType;
	PieceType  promotionType = PieceType::DefaultType;
	PieceColor player		 = PieceColor::NoColor;

	MoveType   type			 = MoveType::Normal;

	int		   number		 = 0; // Storing the current number of this move. Each move saved should increment this number!

	int		   halfMoveClock = 0; // Incrememted with every move that is not a capture or pawn move (detect draw if halfMoveClock is 100)


	bool	   operator<(const Move &other) const
	{
		return this->number < other.number;
	}

	bool operator==(const Move &other) const
	{
		return this->number == other.number;
	}
};

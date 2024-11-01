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


struct Move
{
	Move()
	{
	}

	Move(Position  start,
		 Position  end,
		 PieceType moved,
		 PieceType captured	 = PieceType::DefaultType,
		 bool	   castling	 = false,
		 bool	   enPassant = false,
		 PieceType promotion = PieceType::DefaultType)
		: startingPosition(start), endingPosition(end), movedPiece(moved), capturedPiece(captured), isCastling(castling), isEnPassant(enPassant), promotionType(promotion)
	{
	}

	Position  startingPosition;
	Position  endingPosition;

	PieceType movedPiece	= PieceType::DefaultType;
	PieceType capturedPiece = PieceType::DefaultType;
	PieceType promotionType = PieceType::DefaultType;

	bool	  isCastling	= false;
	bool	  isEnPassant	= false;
};


struct PossibleMove
{
	Position start;
	Position end;
	bool	 canCapturePiece = false;

	bool	 operator==(PossibleMove &other)
	{
		return this->start == other.start && this->end == other.end;
	}
};
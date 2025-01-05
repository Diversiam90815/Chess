/*
  ==============================================================================

	Class:          ChessPiece

	Description:    Virtual class defining a generalized chess piece

  ==============================================================================
*/

#pragma once

#include <vector>
#include <utility>
#include <memory>

#include "Move.h"
#include "Parameters.h"

class ChessBoard;


class ChessPiece
{
public:
	ChessPiece(PieceType type, PlayerColor color) : type(type), color(color), hasMoved(false)
	{
	}

	virtual ~ChessPiece()
	{
	}

	PieceType getType() const
	{
		return type;
	}

	PlayerColor getColor() const
	{
		return color;
	}

	bool getHasMoved() const
	{
		return hasMoved;
	}

	void setHasMoved(bool moved)
	{
		hasMoved = moved;
	}

	static std::shared_ptr<ChessPiece> CreatePiece(PieceType type, PlayerColor color);


	virtual std::vector<PossibleMove> getPossibleMoves(const Position &pos, ChessBoard &board) const = 0;


protected:
	PieceType	type;

	PlayerColor color;

	bool		hasMoved;
};

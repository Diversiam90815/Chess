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
	ChessPiece(PieceType type, PlayerColor color) : type(type), color(color)
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

	bool hasMoved() const
	{
		return moveCounter != 0;
	}

	void increaseMoveCounter()
	{
		moveCounter++;
	}

	void decreaseMoveCounter()
	{
		moveCounter--;
	}

	static std::shared_ptr<ChessPiece> CreatePiece(PieceType type, PlayerColor color);


	virtual std::vector<PossibleMove>  getPossibleMoves(const Position &pos, ChessBoard &board, bool attackOnly = false) const = 0;


protected:
	PieceType	type;

	PlayerColor color;

	int			moveCounter = 0;
};

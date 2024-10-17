
#pragma once

#include "PieceType.h"
#include <vector>
#include <utility>

class ChessBoard;

class ChessPiece
{
public:
	ChessPiece(PieceType type, PieceColor color) : type(type), color(color), hasMoved(false)
	{
	}

	virtual ~ChessPiece()
	{
	}

	PieceType getType() const
	{
		return type;
	}

	PieceColor getColor() const
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


	virtual bool							 isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard &board) const = 0;

	virtual std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const						= 0;


protected:
	PieceType  type;

	PieceColor color;
	
	bool	   hasMoved;
};
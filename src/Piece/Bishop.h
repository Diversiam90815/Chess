/*
  ==============================================================================

	Class:          Bishop

	Description:    Bishop class inheriting from ChessPiece

  ==============================================================================
*/


#pragma once

#include "ChessPiece.h"


class Bishop : public ChessPiece
{
public:
	Bishop(PieceColor color) : ChessPiece(PieceType::Bishop, color)
	{
	}
	

	bool							 isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard &board) const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;

};


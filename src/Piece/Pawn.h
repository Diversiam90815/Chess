/*
  ==============================================================================

	Class:          Pawn

	Description:    Pawn class inheriting from ChessPiece

  ==============================================================================
*/

#pragma once
#include "ChessPiece.h"


class Pawn : public ChessPiece
{
public:
	Pawn(PieceColor color) : ChessPiece(PieceType::Pawn, color)
	{
	}

	bool							 isValidMove(int fromX, int fromY, int toX, int toY, ChessBoard &board) const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, ChessBoard &board) const override;
};
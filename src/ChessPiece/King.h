
#pragma once
#include "ChessPiece.h"

class King : public ChessPiece
{
public:
	King(PieceColor color) : ChessPiece(PieceType::King, color)
	{
	}

	bool							 isValidMove(int fromX, int fromY, int toX, int toY, const ChessBoard &board) const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, const ChessBoard &board) const override;

};

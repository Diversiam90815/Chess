
#pragma once
#include "ChessPiece.h"


class Rook : public ChessPiece
{
public:
	Rook(PieceColor color) : ChessPiece(PieceType::Rook, color)
	{
	}

	bool							 isValidMove(int fromX, int fromY, int toX, int toY, const ChessBoard &board) const override;

	std::vector<std::pair<int, int>> getPossibleMoves(int x, int y, const ChessBoard &board) const override;
};
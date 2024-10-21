/*
  ==============================================================================

	Class:          King

	Description:    King class inheriting from ChessPiece

  ==============================================================================
*/


#include "King.h"
#include "ChessBoard.h"
#include <cmath>



int King::getPieceValue() const
{
	return kingValue;
}


std::vector<std::pair<int, int>> King::getPossibleMoves(int x, int y, ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;

	int								 offsets[8][2] = {
		 {1, 0},   // Right
		 {1, 1},   // Up-Right
		 {0, 1},   // Up
		 {-1, 1},  // Up-Left
		 {-1, 0},  // Left
		 {-1, -1}, // Down-Left
		 {0, -1},  // Down
		 {1, -1}   // Down-Right
	 };

	for (auto &offset : offsets)
	{
		int newX = x + offset[0];
		int newY = y + offset[1];
		if (newX >= 0 && newX <= 7 && newY >= 0 && newY <= 7)
		{
			auto targetPiece = board.getPiece(newX, newY);
			if (targetPiece == nullptr || targetPiece->getColor() != color)
			{
				moves.emplace_back(newX, newY);
			}
		}
	}

	// Castling moves
	if (!hasMoved)
	{
		// Kingside castling
		if (board.isEmpty(x + 1, y) && board.isEmpty(x + 2, y))
		{
			auto rook = board.getPiece(7, y);
			if (rook != nullptr && rook->getType() == PieceType::Rook && !rook->getHasMoved() && rook->getColor() == color)
			{
				moves.emplace_back(x + 2, y);
			}
		}
		// Queenside castling
		if (board.isEmpty(x - 1, y) && board.isEmpty(x - 2, y) && board.isEmpty(x - 3, y))
		{
			auto rook = board.getPiece(0, y);
			if (rook != nullptr && rook->getType() == PieceType::Rook && !rook->getHasMoved() && rook->getColor() == color)
			{
				moves.emplace_back(x - 2, y);
			}
		}
	}

	return moves;
}

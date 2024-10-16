
#include "King.h"
#include "ChessBoard.h"
#include <cmath>


bool King::isValidMove(int fromX, int fromY, int toX, int toY, const ChessBoard &board) const
{
	int dx = abs(toX - fromX);
	int dy = abs(toY - fromY);

	// Standard one-square move
	if ((dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0))
	{
		ChessPiece *targetPiece = board.getPiece(toX, toY);
		if (targetPiece == nullptr || targetPiece->getColor() != color)
		{
			// Additional checks for moving into check should be implemented
			return true;
		}
	}

	// Castling (simplified)
	if (!hasMoved && dy == 0 && dx == 2)
	{
		// Kingside or Queenside castling
		int			rookX = (toX > fromX) ? 7 : 0;
		ChessPiece *rook  = board.getPiece(rookX, fromY);
		if (rook != nullptr && rook->getType() == PieceType::Rook && !rook->getHasMoved() && rook->getColor() == color)
		{
			// Check if squares between king and rook are empty
			int step = (toX > fromX) ? 1 : -1;
			for (int x = fromX + step; x != rookX; x += step)
			{
				if (!board.isEmpty(x, fromY))
				{
					return false;
				}
			}
			// Additional checks for check should be added
			return true;
		}
	}

	return false;
}

std::vector<std::pair<int, int>> King::getPossibleMoves(int x, int y, const ChessBoard &board) const
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
			ChessPiece *targetPiece = board.getPiece(newX, newY);
			if (targetPiece == nullptr || targetPiece->getColor() != color)
			{
				// Additional checks for moving into check should be implemented
				moves.emplace_back(newX, newY);
			}
		}
	}

	// Castling moves (simplified)
	if (!hasMoved)
	{
		// Kingside castling
		if (board.isEmpty(x + 1, y) && board.isEmpty(x + 2, y))
		{
			ChessPiece *rook = board.getPiece(7, y);
			if (rook != nullptr && rook->getType() == PieceType::Rook && !rook->getHasMoved() && rook->getColor() == color)
			{
				moves.emplace_back(x + 2, y);
			}
		}
		// Queenside castling
		if (board.isEmpty(x - 1, y) && board.isEmpty(x - 2, y) && board.isEmpty(x - 3, y))
		{
			ChessPiece *rook = board.getPiece(0, y);
			if (rook != nullptr && rook->getType() == PieceType::Rook && !rook->getHasMoved() && rook->getColor() == color)
			{
				moves.emplace_back(x - 2, y);
			}
		}
	}

	return moves;
}

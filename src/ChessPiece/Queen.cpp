
#include "Queen.h"
#include "ChessBoard.h"
#include <cmath>


bool Queen::isValidMove(int fromX, int fromY, int toX, int toY, const ChessBoard &board) const
{
	int dx = abs(toX - fromX);
	int dy = abs(toY - fromY);

	if ((dx == dy && dx != 0) || (fromX == toX && dy != 0) || (fromY == toY && dx != 0))
	{
		int stepX = (toX - fromX) != 0 ? (toX - fromX) / (dx ? dx : 1) : 0;
		int stepY = (toY - fromY) != 0 ? (toY - fromY) / (dy ? dy : 1) : 0;
		int x	  = fromX + stepX;
		int y	  = fromY + stepY;
		while (x != toX || y != toY)
		{
			if (!board.isEmpty(x, y))
			{
				return false; // Path is blocked
			}
			x += stepX;
			y += stepY;
		}
		ChessPiece *targetPiece = board.getPiece(toX, toY);
		if (targetPiece == nullptr || targetPiece->getColor() != color)
		{
			return true;
		}
	}
	return false;
}

std::vector<std::pair<int, int>> Queen::getPossibleMoves(int x, int y, const ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;

	int								 directions[8][2] = {
		 {1, 0},  // Right
		 {-1, 0}, // Left
		 {0, 1},  // Up
		 {0, -1}, // Down
		 {1, 1},  // Up-Right
		 {1, -1}, // Down-Right
		 {-1, 1}, // Up-Left
		 {-1, -1} // Down-Left
	 };

	for (auto &dir : directions)
	{
		int newX = x + dir[0];
		int newY = y + dir[1];
		while (newX >= 0 && newX <= 7 && newY >= 0 && newY <= 7)
		{
			if (board.isEmpty(newX, newY))
			{
				moves.emplace_back(newX, newY);
			}
			else
			{
				if (board.getPiece(newX, newY)->getColor() != color)
				{
					moves.emplace_back(newX, newY); // Can capture
				}
				break;								// Can't move past occupied square
			}
			newX += dir[0];
			newY += dir[1];
		}
	}

	return moves;
}


#include "Rook.h"
#include "ChessBoard.h"
#include <cmath>


bool Rook::isValidMove(int fromX, int fromY, int toX, int toY, const ChessBoard &board) const
{
	if ((fromX == toX || fromY == toY) && !(fromX == toX && fromY == toY))
	{
		int dx = (toX - fromX) != 0 ? (toX - fromX) / abs(toX - fromX) : 0;
		int dy = (toY - fromY) != 0 ? (toY - fromY) / abs(toY - fromY) : 0;
		int x  = fromX + dx;
		int y  = fromY + dy;
		while (x != toX || y != toY)
		{
			if (!board.isEmpty(x, y))
			{
				return false; // Path is blocked
			}
			x += dx;
			y += dy;
		}
		ChessPiece *targetPiece = board.getPiece(toX, toY);
		if (targetPiece == nullptr || targetPiece->getColor() != color)
		{
			return true;
		}
	}
	return false;
}

std::vector<std::pair<int, int>> Rook::getPossibleMoves(int x, int y, const ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;

	int								 directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

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
				break;
			}
			newX += dir[0];
			newY += dir[1];
		}
	}

	return moves;
}

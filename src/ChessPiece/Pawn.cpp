

#include "Pawn.h"
#include "ChessBoard.h"


bool Pawn::isValidMove(int fromX, int fromY, int toX, int toY, const ChessBoard &board) const
{
	int direction = (color == PieceColor::White) ? 1 : -1;
	int startRow  = (color == PieceColor::White) ? 1 : 6;

	// Standard move
	if (fromX == toX && toY - fromY == direction && board.getPiece(toX, toY) == nullptr)
	{
		return true;
	}

	// Double move from starting position
	if (fromX == toX && fromY == startRow && toY - fromY == 2 * direction && board.getPiece(toX, toY) == nullptr && board.getPiece(toX, fromY + direction) == nullptr)
	{
		return true;
	}

	// Capture move
	if (abs(toX - fromX) == 1 && toY - fromY == direction)
	{
		ChessPiece *targetPiece = board.getPiece(toX, toY);
		if (targetPiece != nullptr && targetPiece->getColor() != color)
		{
			return true;
		}
		// Handle en passant here if necessary
	}

	return false;
}

std::vector<std::pair<int, int>> Pawn::getPossibleMoves(int x, int y, const ChessBoard &board) const
{
	std::vector<std::pair<int, int>> moves;
	int								 direction = (color == PieceColor::White) ? 1 : -1;
	int								 startRow  = (color == PieceColor::White) ? 1 : 6;

	// Standard move
	if (board.isEmpty(x, y + direction))
	{
		moves.emplace_back(x, y + direction);

		// Double move
		if (!hasMoved && board.isEmpty(x, y + 2 * direction))
		{
			moves.emplace_back(x, y + 2 * direction);
		}
	}

	// Captures
	if (x > 0 && board.getPiece(x - 1, y + direction) != nullptr && board.getPiece(x - 1, y + direction)->getColor() != color)
	{
		moves.emplace_back(x - 1, y + direction);
	}
	if (x < 7 && board.getPiece(x + 1, y + direction) != nullptr && board.getPiece(x + 1, y + direction)->getColor() != color)
	{
		moves.emplace_back(x + 1, y + direction);
	}

	// Add en passant moves if implemented

	return moves;
}
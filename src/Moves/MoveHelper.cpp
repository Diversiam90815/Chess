/*
  ==============================================================================

	Class:          MoveHelper

	Description:    Helper class for generating general moves in chess

  ==============================================================================
*/


#include "MoveHelper.h"


MoveHelper::MoveHelper()
{
}


MoveHelper::~MoveHelper()
{
}


bool MoveHelper::checkAvailableMoves(const Position &position, ChessBoard &board, const PieceColor color, PieceType piece, bool hasMoved)
{
	bool movesAdded = false;

	switch (piece)
	{
	case (PieceType::Pawn):
	{
		checkPawnMovement(position, board, color, hasMoved);
		checkPawnCaptureMovement(position, board, color);
		break;
	}

	case (PieceType::Knight):
	{
		checkLShapedMoves(position, board, color);
		break;
	}

	case (PieceType::Bishop):
	{
		checkDiagonalMoves(position, board, color);
		break;
	}

	case (PieceType::Rook):
	{
		checkFileMoves(position, board, color);
		break;
	}

	case (PieceType::Queen):
	{
		checkDiagonalMoves(position, board, color);
		checkAdjacentMoves(position, board, color);
		checkFileMoves(position, board, color);
		break;
	}

	case (PieceType::King):
	{
		checkAdjacentMoves(position, board, color);
		break;
	}

	default: break;
	}
}


std::vector<Move> MoveHelper::getAvailableMoves()
{
	return mPossibleMovesAndCaptures;
}


bool MoveHelper::checkPawnMovement(const Position &position, ChessBoard &board, const PieceColor color, bool hasMoved)
{
	int colorFactor = (color == PieceColor::White) ? 1 : -1;

	int newX		= position.x + mPawnMoveDirections[0].first;
	int newY		= position.y + mPawnMoveDirections[0].second * colorFactor;

	if (checkForBorders(newX, newY) && board.isEmpty(newX, newY))
	{
		Move move;
		move.X = newX;
		move.Y = newY;
		addToAvailableMoves(move);

		// If it's the pawn's first move, check for the two-step option
		if (!hasMoved)
		{
			newX = position.x + mPawnMoveDirections[1].first;
			newY = position.y + mPawnMoveDirections[1].second * colorFactor;

			if (checkForBorders(newX, newY) && board.isEmpty(newX, newY))
			{
				move.X = newX;
				move.Y = newY;
				addToAvailableMoves(move);
			}
		}
	}

	return mPossibleMovesAndCaptures.empty();
}


bool MoveHelper::checkPawnCaptureMovement(const Position &position, ChessBoard &board, const PieceColor color)
{
	int colorFactor = (color == PieceColor::White) ? 1 : -1;

	for (const auto &dir : mPawnCaptureDirections)
	{
		int newX = position.x + dir.first;
		int newY = position.y + dir.second * colorFactor;

		if (checkForBorders(newX, newY) && !board.isEmpty(newX, newY))
		{
			const auto piece = board.getPiece(newX, newY);
			if (piece->getColor() != color)
			{
				Move move;
				move.X				 = newX;
				move.Y				 = newY;
				move.canCapturePiece = true;
				addToAvailableMoves(move);
			}
		}
	}

	return mPossibleMovesAndCaptures.empty();
}


bool MoveHelper::checkDiagonalMoves(const Position &position, ChessBoard &board, const PieceColor color)
{
	return checkMovesInDirection(position, board, color, mDiagonalDirections, false);
}


bool MoveHelper::checkAdjacentMoves(const Position &position, ChessBoard &board, const PieceColor color)
{
	return checkMovesInDirection(position, board, color, mAdjacentPositions, true);
}


bool MoveHelper::checkLShapedMoves(const Position &position, ChessBoard &board, const PieceColor color)
{
	return checkMovesInDirection(position, board, color, mLShapedDirections, true);
}


bool MoveHelper::checkFileMoves(const Position &position, ChessBoard &board, const PieceColor color)
{
	return checkMovesInDirection(position, board, color, mFileDirections, false);
}


template <std::size_t N>
bool MoveHelper::checkMovesInDirection(const Position &position, ChessBoard &board, const PieceColor color, const std::array<std::pair<int, int>, N> &directions, bool oneStep)
{
	for (const auto &dir : directions)
	{
		int newX = position.x + dir.first;
		int newY = position.y + dir.second;

		while (checkForBorders(newX, newY))
		{
			Move move;

			if (board.isEmpty(newX, newY))
			{
				move.X = newX;
				move.Y = newY;
				addToAvailableMoves(move);
			}
			else
			{
				if (board.getPiece(newX, newY)->getColor() != color)
				{
					move.X				 = newX;
					move.Y				 = newY;
					move.canCapturePiece = true;
					addToAvailableMoves(move);
					break;
				}
			}

			if (oneStep)
			{
				break; // Stop after one step in this direction
			}

			newX += dir.first;
			newY += dir.second;
		}
	}

	return mPossibleMovesAndCaptures.empty();
}


bool MoveHelper::checkForBorders(const int x, const int y)
{
	return (x >= 0 && x <= 7 && y >= 0 && y <= 7);
}


bool MoveHelper::checkIfPositionAlreadyExists(const int x, const int y)
{
	for (const auto &pos : mPossibleMovesAndCaptures)
	{
		if (pos.X == x && pos.Y == y)
		{
			return true;
		}
	}
	return false;
}


void MoveHelper::addToAvailableMoves(Move &move)
{
	if (!checkIfPositionAlreadyExists(move.X, move.Y))
	{
		mPossibleMovesAndCaptures.push_back(move);
	}
}

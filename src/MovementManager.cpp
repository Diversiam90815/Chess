
#include "MovementManager.h"
#include <algorithm>


MovementManager::MovementManager(ChessBoard &board) : board(board)
{
}


bool MovementManager::isValidMove(const Move &move, PieceColor playerColor)
{
	// Basic validations
	if (move.fromX < 0 || move.fromX > 7 || move.fromY < 0 || move.fromY > 7 || move.toX < 0 || move.toX > 7 || move.toY < 0 || move.toY > 7)
	{
		return false;
	}

	auto piece = board.getPiece(move.fromX, move.fromY);
	if (!piece || piece->getColor() != playerColor)
	{
		return false;
	}

	auto targetPiece = board.getPiece(move.toX, move.toY);
	if (targetPiece && targetPiece->getColor() == playerColor)
	{
		return false; // Cannot capture own piece
	}

	// Piece-specific move validation
	if (!piece->isValidMove(move.fromX, move.fromY, move.toX, move.toY, board))
	{
		return false;
	}

	// Handle special moves
	if (isCastlingMove(move, playerColor))
	{
		if (!validateCastling(move, playerColor))
		{
			return false;
		}
	}
	else if (isEnPassantMove(move, playerColor))
	{
		if (!validateEnPassant(move, playerColor))
		{
			return false;
		}
	}

	// Simulate the move and check for king safety
	if (wouldKingBeInCheckAfterMove(move, playerColor))
	{
		return false;
	}

	return true;
}


bool MovementManager::executeMove(const Move &move)
{
	auto piece = board.getPiece(move.fromX, move.fromY);
	if (!piece)
		return false;

	// Handle special moves
	if (move.isCastling)
	{
		performCastling(move);
	}
	else if (move.isEnPassant)
	{
		performEnPassant(move);
	}
	else
	{
		// Regular move
		board.setPiece(move.toX, move.toY, piece);
		board.setPiece(move.fromX, move.fromY, nullptr);
		if (move.capturedPiece != PieceType::None)
		{
			// Capture the piece (already removed from the board)
		}
	}

	// Update piece state
	piece->setHasMoved(true);

	// Handle pawn promotion
	if (piece->getType() == PieceType::Pawn && (move.toY == 0 || move.toY == 7))
	{
		promotePawn(move.toX, move.toY, piece->getColor(), move.promotionType);
	}

	return true;
}


bool MovementManager::isKingInCheck(PieceColor color)
{
	// Find the king's position
	int kingX = -1, kingY = -1;
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			auto piece = board.getPiece(x, y);
			if (piece && piece->getType() == PieceType::King && piece->getColor() == color)
			{
				kingX = x;
				kingY = y;
				break;
			}
		}
		if (kingX != -1)
			break;
	}
	if (kingX == -1)
		return true; // King not found

	return isSquareUnderAttack(kingX, kingY, color);
}


bool MovementManager::isSquareUnderAttack(int x, int y, PieceColor color)
{
	PieceColor opponentColor = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
	for (int y1 = 0; y1 < 8; ++y1)
	{
		for (int x1 = 0; x1 < 8; ++x1)
		{
			auto piece = board.getPiece(x1, y1);
			if (piece && piece->getColor() == opponentColor)
			{
				if (piece->isValidMove(x1, y1, x, y, board))
				{
					return true;
				}
			}
		}
	}
	return false;
}


bool MovementManager::wouldKingBeInCheckAfterMove(const Move &move, PieceColor color)
{
	// Save current state
	auto piece		 = board.getPiece(move.fromX, move.fromY);
	auto targetPiece = board.getPiece(move.toX, move.toY);

	// Make the move
	board.setPiece(move.toX, move.toY, piece);
	board.setPiece(move.fromX, move.fromY, nullptr);

	bool inCheck = isKingInCheck(color);

	// Undo the move
	board.setPiece(move.fromX, move.fromY, piece);
	board.setPiece(move.toX, move.toY, targetPiece);

	return inCheck;
}


bool MovementManager::isCastlingMove(const Move &move, PieceColor color)
{
	auto piece = board.getPiece(move.fromX, move.fromY);
	if (piece && piece->getType() == PieceType::King)
	{
		int dx = move.toX - move.fromX;
		if (abs(dx) == 2 && move.fromY == move.toY)
		{
			return true;
		}
	}
	return false;
}


bool MovementManager::validateCastling(const Move &move, PieceColor color)
{
	auto king = board.getPiece(move.fromX, move.fromY);
	if (!king || king->getHasMoved())
		return false;

	int	 rookX = (move.toX > move.fromX) ? 7 : 0;
	auto rook  = board.getPiece(rookX, move.fromY);
	if (!rook || rook->getType() != PieceType::Rook || rook->getColor() != color || rook->getHasMoved())
	{
		return false;
	}

	// Check path between king and rook
	int direction = (move.toX > move.fromX) ? 1 : -1;
	for (int x = move.fromX + direction; x != rookX; x += direction)
	{
		if (board.getPiece(x, move.fromY))
		{
			return false; // Path is not clear
		}
	}

	// Check that king is not in check and doesn't pass through or end in check
	if (isKingInCheck(color))
		return false;

	for (int x = move.fromX; x != move.toX + direction; x += direction)
	{
		if (isSquareUnderAttack(x, move.fromY, color))
		{
			return false; // Square is under attack
		}
	}

	return true;
}


void MovementManager::performCastling(const Move &move)
{
	int	 direction = (move.toX > move.fromX) ? 1 : -1;
	int	 rookFromX = (direction == 1) ? 7 : 0;
	int	 rookToX   = move.fromX + direction;

	auto king	   = board.getPiece(move.fromX, move.fromY);
	auto rook	   = board.getPiece(rookFromX, move.fromY);

	// Move king
	board.setPiece(move.toX, move.toY, king);
	board.setPiece(move.fromX, move.fromY, nullptr);

	// Move rook
	board.setPiece(rookToX, move.fromY, rook);
	board.setPiece(rookFromX, move.fromY, nullptr);

	// Update moved status
	king->setHasMoved(true);
	rook->setHasMoved(true);
}


bool MovementManager::isEnPassantMove(const Move &move, PieceColor color)
{
	auto piece = board.getPiece(move.fromX, move.fromY);
	if (piece && piece->getType() == PieceType::Pawn)
	{
		int dy = move.toY - move.fromY;
		int dx = move.toX - move.fromX;
		if (abs(dx) == 1 && dy == ((color == PieceColor::White) ? 1 : -1))
		{
			if (!board.getPiece(move.toX, move.toY))
			{
				return true;
			}
		}
	}
	return false;
}


bool MovementManager::validateEnPassant(const Move &move, PieceColor color)
{
	// The last move must be a pawn moving two squares forward to the adjacent file
	auto lastMove = board.getLastMove();
	if (!lastMove)
		return false;

	auto lastMovedPiece = board.getPiece(lastMove->toX, lastMove->toY);
	if (!lastMovedPiece || lastMovedPiece->getType() != PieceType::Pawn)
		return false;

	if (abs(lastMove->fromY - lastMove->toY) != 2)
		return false;

	if (lastMove->toX != move.toX || lastMove->toY != move.fromY)
		return false;

	return true;
}


void MovementManager::performEnPassant(const Move &move)
{
	auto pawn = board.getPiece(move.fromX, move.fromY);
	board.setPiece(move.toX, move.toY, pawn);
	board.setPiece(move.fromX, move.fromY, nullptr);

	// Remove the captured pawn
	int capturedPawnY = move.fromY;
	board.setPiece(move.toX, capturedPawnY, nullptr);

	pawn->setHasMoved(true);
}


void MovementManager::promotePawn(int x, int y, PieceColor color, PieceType promotionType)
{
	switch (promotionType)
	{
	case PieceType::Queen:
	{
		board.setPiece(x, y, std::make_shared<Queen>(color));
		break;
	}

	case PieceType::Rook:
	{
		board.setPiece(x, y, std::make_shared<Rook>(color));
		break;
	}

	case PieceType::Bishop:
	{
		board.setPiece(x, y, std::make_shared<Bishop>(color));
		break;
	}

	case PieceType::Knight:
	{
		board.setPiece(x, y, std::make_shared<Knight>(color));
		break;
	}

	default:
		board.setPiece(x, y, std::make_shared<Queen>(color));
		break;
	}
}


bool MovementManager::isCheckmate(PieceColor color)
{
	if (!isKingInCheck(color))
		return false;

	// Generate all legal moves
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			auto piece = board.getPiece(x, y);
			if (piece && piece->getColor() == color)
			{
				auto moves = piece->getPossibleMoves(x, y, board);
				for (auto &movePos : moves)
				{
					Move testMove(x, y, movePos.first, movePos.second, piece->getType());
					if (isValidMove(testMove, color))
					{
						return false; // At least one valid move exists
					}
				}
			}
		}
	}
	return true; // No valid moves, player is checkmated
}


bool MovementManager::isStalemate(PieceColor color)
{
	if (isKingInCheck(color))
		return false;

	// Generate all legal moves
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			auto piece = board.getPiece(x, y);
			if (piece && piece->getColor() == color)
			{
				auto moves = piece->getPossibleMoves(x, y, board);
				for (auto &movePos : moves)
				{
					Move testMove(x, y, movePos.first, movePos.second, piece->getType());
					if (isValidMove(testMove, color))
					{
						return false; // At least one valid move exists
					}
				}
			}
		}
	}
	return true; // No valid moves, player is stalemated
}

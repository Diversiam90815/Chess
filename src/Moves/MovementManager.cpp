/*
  ==============================================================================

	Class:          MovementManager

	Description:    Manages and executes valid moves in chess

  ==============================================================================
*/


#include "MovementManager.h"
#include <algorithm>



MovementManager::MovementManager(ChessBoard &board) : board(board)
{
}


std::unordered_map<Position, std::vector<PossibleMove>> MovementManager::getAllPossibleMoves(PieceColor playerColor)
{
	std::unordered_map<Position, std::vector<PossibleMove>> allPossibleMoves;

	auto													playerPieces = board.getPiecesFromPlayer(playerColor);

	for (const auto &[startPosition, piece] : playerPieces)
	{
		auto					  possibleMoves = piece->getPossibleMoves(startPosition, board);

		std::vector<PossibleMove> validMoves;
		validMoves.reserve(possibleMoves.size()); // Reserve space to avoid reallocations

		for (const auto &possibleMove : possibleMoves)
		{
			Move testMove(possibleMove.start, possibleMove.end, piece->getType());

			// Validate the move
			if (!validateMove(testMove, playerColor))
			{
				validMoves.push_back(possibleMove);
			}
		}

		if (!validMoves.empty())
		{
			allPossibleMoves.emplace(startPosition, std::move(validMoves));
		}
	}
	return allPossibleMoves;
}


bool MovementManager::validateMove(Move &move, PieceColor playerColor)
{
	auto kingPosition = board.getKingsPosition(playerColor);

	if (isKingInCheck(kingPosition,playerColor) && move.startingPosition != kingPosition)
		return false;

	if(wouldKingBeInCheckAfterMove(move, playerColor))
		return false;
}


bool MovementManager::isKingInCheck(Position &ourKing, PieceColor playerColor)
{
	PieceColor opponentColor = playerColor == PieceColor::White ? PieceColor::Black : PieceColor::White;
	return isSquareAttacked(ourKing, opponentColor);
}


bool MovementManager::wouldKingBeInCheckAfterMove(Move &move, PieceColor playerColor)
{
	/*
	 *  ToDo's:
	 *	1. Save the current state
	 *	2. Simulate the move
	 *	3. Update King's position if if this is the king
	 *	4. Check if King is under attack (isSquareUnderAttack)
	 *	5. Revert the move
	 *	6. Update Kings position back if necessary
	 */

	bool	 kingInCheck	= false;

	// 1.
	auto	 movingPiece	= board.getPiece(move.startingPosition);
	auto	 capturingPiece = board.getPiece(move.endingPosition); // If there is no piece being captured in this move, this will be nullptr

	Position kingPosition	= board.getKingsPosition(playerColor);

	// 2
	board.setPiece(move.startingPosition, movingPiece);
	board.removePiece(move.endingPosition);

	// 3
	if (movingPiece->getType() == PieceType::King)
	{
		kingPosition = move.endingPosition;
	}

	// 4.
	kingInCheck = isSquareAttacked(kingPosition, playerColor == PieceColor::White ? PieceColor::Black : PieceColor::White);


	// 5.
	board.setPiece(move.startingPosition, movingPiece);
	board.setPiece(move.endingPosition, capturingPiece); // This could be nullptr if there was no captured piece


	return kingInCheck;
}


bool MovementManager::isSquareAttacked(const Position &square, PieceColor attackerColor)
{
	// Iterate over all opponent pieces
	auto opponentPieces = board.getPiecesFromPlayer(attackerColor);

	for (const auto &[pos, piece] : opponentPieces)
	{
		// Get possible moves for the opponent's piece
		auto moves = piece->getPossibleMoves(pos, board);

		for (const auto &move : moves)
		{
			if (move.end == square)
			{
				return true;
			}
		}
	}

	return false;
}


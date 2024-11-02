/*
  ==============================================================================

	Class:          MovementManager

	Description:    Manages and executes valid moves in chess

  ==============================================================================
*/


#include "MovementManager.h"
#include <algorithm>



MovementManager::MovementManager(std::shared_ptr<ChessBoard> board) : board(board)
{
}


std::vector<PossibleMove> MovementManager::getMovesForPosition(Position &position)
{
	auto piece	= board->getPiece(position);
	auto player = piece->getColor();

	if (mAllLegalMovesForCurrentRound.size() == 0)
		calculateAllLegalBasicMoves(player);

	auto &possibleMoves = mAllLegalMovesForCurrentRound[position];

	// Add special moves available for this position to the possibleMoves
	if (piece->getType() == PieceType::King)
	{
		auto castlingMoves = generateCastlingMoves(position, player);
		for (auto &move : castlingMoves)
		{
			possibleMoves.push_back(move);
		}
	}

	if (piece->getType() == PieceType::Pawn)
	{
		auto enPasssantMove = generateEnPassantMove(position, player);
		possibleMoves.push_back(enPasssantMove);
	}

	return possibleMoves;
}


bool MovementManager::calculateAllLegalBasicMoves(PieceColor playerColor)
{
	auto playerPieces = board->getPiecesFromPlayer(playerColor);

	for (const auto &[startPosition, piece] : playerPieces)
	{
		auto					  possibleMoves = piece->getPossibleMoves(startPosition, *board);

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
			mAllLegalMovesForCurrentRound.emplace(startPosition, std::move(validMoves));
		}
	}
	return mAllLegalMovesForCurrentRound.size() != 0;
}


Move MovementManager::executeMove(PossibleMove &possibleMove)
{
	// Store positions in Move from executed PossibleMove
	Move executedMove		= Move(possibleMove);

	// Store the moved piece type
	auto movedPiece			= board->getPiece(possibleMove.start)->getType();
	executedMove.movedPiece = movedPiece;

	// Store if this move captured another piece
	bool capturedPiece		= possibleMove.type == MoveType::Capture;
	if (capturedPiece)
	{
		auto capturedPiece		   = board->getPiece(possibleMove.end)->getType();
		executedMove.capturedPiece = capturedPiece;
	}

	// Move piece on the board
	bool result				   = board->movePiece(possibleMove.start, possibleMove.end);


	// Increment or reset the halfMoveClock

	int	 previousHalfMoveClock = 0;

	if (!mMoveHistory.empty())
	{
		previousHalfMoveClock = getLastMove()->halfMoveClock;
	}

	if (movedPiece != PieceType::Pawn && !capturedPiece)
	{
		executedMove.halfMoveClock = previousHalfMoveClock + 1;
	}
	else
	{
		executedMove.halfMoveClock = 0;
	}

	addMoveToHistory(executedMove);
	return executedMove;
}


bool MovementManager::validateMove(Move &move, PieceColor playerColor)
{
	auto kingPosition = board->getKingsPosition(playerColor);

	if (isKingInCheck(kingPosition, playerColor) && move.startingPosition != kingPosition)
		return false;

	if (wouldKingBeInCheckAfterMove(move, playerColor))
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
	auto	 movingPiece	= board->getPiece(move.startingPosition);
	auto	 capturingPiece = board->getPiece(move.endingPosition); // If there is no piece being captured in this move, this will be nullptr

	Position kingPosition	= board->getKingsPosition(playerColor);

	// 2
	board->setPiece(move.startingPosition, movingPiece);
	board->removePiece(move.endingPosition);

	// 3
	if (movingPiece->getType() == PieceType::King)
	{
		kingPosition = move.endingPosition;
	}

	// 4.
	kingInCheck = isSquareAttacked(kingPosition, playerColor == PieceColor::White ? PieceColor::Black : PieceColor::White);


	// 5.
	board->setPiece(move.startingPosition, movingPiece);
	board->setPiece(move.endingPosition, capturingPiece); // This could be nullptr if there was no captured piece


	return kingInCheck;
}


bool MovementManager::isSquareAttacked(const Position &square, PieceColor attackerColor)
{
	// Iterate over all opponent pieces
	auto opponentPieces = board->getPiecesFromPlayer(attackerColor);

	for (const auto &[pos, piece] : opponentPieces)
	{
		// Get possible moves for the opponent's piece
		auto moves = piece->getPossibleMoves(pos, *board);

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


bool MovementManager::executeCastlingMove(PossibleMove &move)
{
	return false;
}

std::vector<PossibleMove> MovementManager::generateCastlingMoves(const Position &kingPosition, PieceColor player)
{
	std::vector<PossibleMove> castlingMoves;
	castlingMoves.reserve(2 * sizeof(castlingMoves)); // There are max. of 2 moves of castling

	if (canCastle(kingPosition, player, true))
	{
		PossibleMove kingsideCastling;
		kingsideCastling.start = kingPosition;
		kingsideCastling.end   = Position{kingPosition.x + 2, kingPosition.y};
		kingsideCastling.type  = MoveType::CastlingKingside;
		castlingMoves.push_back(kingsideCastling);
	}

	if (canCastle(kingPosition, player, false))
	{
		PossibleMove queensideCastling;
		queensideCastling.start = kingPosition;
		queensideCastling.end	= Position{kingPosition.x - 2, kingPosition.y};
		queensideCastling.type	= MoveType::CastlingQueenside;
		castlingMoves.push_back(queensideCastling);
	}

	return castlingMoves;
}


bool MovementManager::canCastle(const Position &kingposition, PieceColor player, bool kingside)
{
	auto king	   = board->getPiece(kingposition);
	int	 direction = kingside ? +1 : -1; // Determine the direction of castling

	if (king->getHasMoved())
		return false;

	// Define the y-coordinate and king's x-coordinate
	int		 y	   = kingposition.y;
	int		 kingX = kingposition.x;

	// Determine the rook's position based on the direction
	int		 rookX = (direction == 1) ? 7 : 0; // 7 for kingside (h-file), 0 for queenside (a-file)
	Position rookPosition{rookX, y};
	auto	 rook = board->getPiece(rookPosition);

	// Check if the rook exists, is of the correct type, color, and has not moved
	if (!rook || rook->getType() != PieceType::Rook || rook->getColor() != player || rook->getHasMoved())
		return false;

	// Check if way is free
	for (int x = kingX + direction; x != rookX; x += direction)
	{
		Position pos{x, y};
		if (!board->isEmpty(pos))
			return false;
	}

	// Check if way is under attack
	std::vector<Position> positionsToCheck = {{kingX + direction, y}, {kingX + 2 * direction, y}};
	for (const auto &pos : positionsToCheck)
	{
		Move testMove(kingposition, pos, PieceType::King);
		if (wouldKingBeInCheckAfterMove(testMove, player))
			return false;
	}

	return true;
}


bool MovementManager::executeEnPassantMove(PossibleMove &move)
{
	return false;
}


PossibleMove MovementManager::generateEnPassantMove(const Position &position, PieceColor player)
{
	if (!canEnPassant(position, player))
		return PossibleMove();

	auto	 lastMove = getLastMove();

	// Calculate target position
	Position targetPosition;
	if (player == PieceColor::White)
	{
		targetPosition = Position(lastMove->endingPosition.x, lastMove->endingPosition.y + 1);
	}
	else
	{
		targetPosition = Position(lastMove->endingPosition.x, lastMove->endingPosition.y - 1);
	}

	PossibleMove enPassantMove;
	enPassantMove.start = position;
	enPassantMove.end	= targetPosition;
	enPassantMove.type	= MoveType::EnPassant;

	return enPassantMove;
}


bool MovementManager::canEnPassant(const Position &position, PieceColor player)
{
	auto lastMove = getLastMove();

	if (!lastMove)
		return false;

	// Ensure if the last move was a pawn double push
	bool lastMoveWasPawnDoublePush = lastMove->type == MoveType::DoublePawnPush;
	if (!lastMoveWasPawnDoublePush)
		return false;

	// Ensure the last move was made by the opponent
	if (lastMove->player == player)
		return false;

	// Ensure that the position match
	Position lastMoveEndPosition	 = lastMove->endingPosition;

	// Check if both pawns are on the same file
	bool	 bothPiecesOnTheSameFile = (lastMoveEndPosition.x == position.x);
	if (!bothPiecesOnTheSameFile)
		return false;

	// Ensure both pawns are adjacent ranks
	bool bothPiecesNextToEachOther = (std::abs(lastMoveEndPosition.y - position.y) == 1);
	if (!bothPiecesNextToEachOther)
		return false;

	// Ensure the capturing pawn is on the correct rank for en passant
	if ((player == PieceColor::White && position.y != 5) || (player == PieceColor::Black && position.y != 4))
		return false;

	return true;
}


const Move *MovementManager::getLastMove()
{
	if (mMoveHistory.empty())
		return nullptr;

	return &(*mMoveHistory.rbegin());
}


void MovementManager::addMoveToHistory(Move &move)
{
	move.number = mMoveHistory.size() + 1; // Set the move number based on history size
	mMoveHistory.insert(move);
}

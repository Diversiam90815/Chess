/*
  ==============================================================================

	Class:          MovementManager

	Description:    Manages and executes valid moves in chess

  ==============================================================================
*/

#include "MovementManager.h"
#include <algorithm>


MovementManager::MovementManager()
{
}


void MovementManager::init()
{
	mChessBoard = std::make_unique<ChessBoard>();
	mChessBoard->initializeBoard();

	mMoveNotation = std::make_unique<MoveNotationHelper>();
}


std::vector<PossibleMove> MovementManager::getMovesForPosition(Position &position)
{
	auto &piece = mChessBoard->getPiece(position);

	if (!piece)
		return {};

	auto player = piece->getColor();

	//if (mAllLegalMovesForCurrentRound.size() == 0)
	{
		LOG_INFO("All Legel Moves are empty, so we start calculating for Player {}!", LoggingHelper::playerColourToString(player).c_str());
		calculateAllLegalBasicMoves(player);
		/*
			Changes in the board will not be updated in the legal moves correctly!!
		*/
	}

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

	LOG_INFO("Position {} has {} possible moves!", LoggingHelper::positionToString(position).c_str(), possibleMoves.size());
	return possibleMoves;
}


bool MovementManager::calculateAllLegalBasicMoves(PlayerColor playerColor)
{
	auto playerPieces = mChessBoard->getPiecesFromPlayer(playerColor);

	for (const auto &[startPosition, piece] : playerPieces)
	{
		auto					  possibleMoves = piece->getPossibleMoves(startPosition, *mChessBoard);

		std::vector<PossibleMove> validMoves;
		validMoves.reserve(possibleMoves.size()); // Reserve space to avoid reallocations

		for (const auto &possibleMove : possibleMoves)
		{
			Move testMove(possibleMove.start, possibleMove.end, piece->getType());

			// Validate the move
			if (validateMove(testMove, playerColor))
			{
				validMoves.push_back(possibleMove);
			}
		}

		if (!validMoves.empty())
		{
			mAllLegalMovesForCurrentRound.emplace(startPosition, std::move(validMoves));
		}
	}

	size_t numMoves = mAllLegalMovesForCurrentRound.size();

	LOG_INFO("Calculating all moves finished, with {} moves!", numMoves);

	return numMoves != 0;
}


Move MovementManager::executeMove(PossibleMove &possibleMove, PieceType pawnPromotion)
{
	// Store positions in Move from executed PossibleMove
	Move  executedMove		= Move(possibleMove);

	// Store the moved piece type
	auto &movedPiece		= mChessBoard->getPiece(possibleMove.start);
	auto  movedPieceType	= movedPiece->getType();
	auto  player			= mChessBoard->getPiece(possibleMove.start)->getColor();

	executedMove.movedPiece = movedPieceType;
	executedMove.player		= player;

	// Set hasMoved of piece
	movedPiece->setHasMoved(true);

	// Store if this move captured another piece
	bool capturedPiece = (possibleMove.type & MoveType::Capture) == MoveType::Capture;
	if (capturedPiece)
	{
		auto pieceCaptured		   = mChessBoard->getPiece(possibleMove.end)->getType();
		executedMove.capturedPiece = pieceCaptured;
		mChessBoard->movePiece(possibleMove.start, possibleMove.end);
	}

	if ((possibleMove.type & MoveType::EnPassant) == MoveType::EnPassant)
	{
		executeEnPassantMove(possibleMove, player);
	}
	else if ((possibleMove.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (possibleMove.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
	{
		executeCastlingMove(possibleMove);
	}
	else if ((possibleMove.type & MoveType::Normal) == MoveType::Normal || (possibleMove.type & MoveType::DoublePawnPush) == MoveType::DoublePawnPush)
	{
		mChessBoard->movePiece(possibleMove.start, possibleMove.end);
	}
	else if ((possibleMove.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	{
		executePawnPromotion(possibleMove, pawnPromotion);
		executedMove.promotionType = pawnPromotion;
	}

	PlayerColor opponent	 = player == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;
	auto		opponentKing = mChessBoard->getKingsPosition(opponent);

	if (isCheckmate(opponent))
	{
		executedMove.type |= MoveType::Checkmate;
	}
	else if (isKingInCheck(opponentKing, player))
	{
		executedMove.type |= MoveType::Check;
	}


	// Increment or reset the halfMoveClock

	int previousHalfMoveClock = 0;

	if (!mMoveHistory.empty())
	{
		previousHalfMoveClock = getLastMove()->halfMoveClock;
	}

	if (movedPieceType != PieceType::Pawn && !capturedPiece)
	{
		executedMove.halfMoveClock = previousHalfMoveClock + 1;
	}
	else
	{
		executedMove.halfMoveClock = 0;
	}

	executedMove.notation = mMoveNotation->generateStandartAlgebraicNotation(executedMove);

	addMoveToHistory(executedMove);
	return executedMove;
}


bool MovementManager::validateMove(Move &move, PlayerColor playerColor)
{
	auto kingPosition = mChessBoard->getKingsPosition(playerColor);

	if (isKingInCheck(kingPosition, playerColor) && move.startingPosition != kingPosition)
	{
		LOG_INFO("Move could not be validated, since the king is in check!");
		return false;
	}

	if (wouldKingBeInCheckAfterMove(move, playerColor))
	{
		LOG_INFO("Move could not be validated, since the king would be in check after this move!");
		return false;
	}

	return true;
}


bool MovementManager::isKingInCheck(Position &ourKing, PlayerColor playerColor)
{
	PlayerColor opponentColor = playerColor == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;
	return isSquareAttacked(ourKing, opponentColor);
}


bool MovementManager::isCheckmate(PlayerColor player)
{
	Position kingPosition = mChessBoard->getKingsPosition(player);

	if (!isKingInCheck(kingPosition, player))
		return false; // If the king is not in check, it's not checkmate

	// If the king is in check, check if there are any moves that can escape check
	auto playerPieces = mChessBoard->getPiecesFromPlayer(player);
	for (const auto &[startPosition, piece] : playerPieces)
	{
		auto possibleMoves = piece->getPossibleMoves(startPosition, *mChessBoard);

		for (const auto &move : possibleMoves)
		{
			Move testMove(startPosition, move.end, piece->getType());

			if (!wouldKingBeInCheckAfterMove(testMove, player))
			{
				return false; // If there's a legal move that stops check, it's not checkmate
			}
		}
	}

	// No legal move can stop the check; it's checkmate
	return true;
}


bool MovementManager::isStalemate(PlayerColor player)
{
	Position kingPosition = mChessBoard->getKingsPosition(player);
	if (isKingInCheck(kingPosition, player))
		return false;

	if (!calculateAllLegalBasicMoves(player))
	{
		// If no legal moves are available and king is not in check, it's stalemate
		return true;
	}

	// Legal moves are available, so it's not stalemate
	return false;
}


bool MovementManager::wouldKingBeInCheckAfterMove(Move &move, PlayerColor playerColor)
{
	bool	   kingInCheck	  = false;

	// Make a local copy of the board
	ChessBoard boardCopy	  = *mChessBoard; // uses your ChessBoard(const ChessBoard&) constructor


	// Save the current state
	auto	  &movingPiece	  = boardCopy.getPiece(move.startingPosition);
	auto	  &capturingPiece = boardCopy.getPiece(move.endingPosition); // If there is no piece being captured in this move, this will be nullptr

	Position   kingPosition	  = boardCopy.getKingsPosition(playerColor);

	// Simulate the move
	boardCopy.setPiece(move.startingPosition, movingPiece);
	boardCopy.removePiece(move.endingPosition);

	// Update King's position if if this is the king
	bool isKing = movingPiece->getType() == PieceType::King;
	if (isKing)
	{
		kingPosition = move.endingPosition;
	}

	// Check if King is under attack (isSquareUnderAttack)
	kingInCheck = isSquareAttacked(kingPosition, playerColor == PlayerColor::White ? PlayerColor::Black : PlayerColor::White, boardCopy);

	// Update Kings position back if necessary
	if (isKing)
	{
		kingPosition = move.startingPosition;
	}

	return kingInCheck;
}


bool MovementManager::isSquareAttacked(const Position &square, PlayerColor attackerColor)
{
	// Iterate over all opponent pieces
	auto opponentPieces = mChessBoard->getPiecesFromPlayer(attackerColor);

	for (const auto &[pos, piece] : opponentPieces)
	{
		// Get possible moves for the opponent's piece
		auto moves = piece->getPossibleMoves(pos, *mChessBoard);

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


bool MovementManager::isSquareAttacked(const Position &square, PlayerColor attackerColor, ChessBoard &chessboard)
{
	// Iterate over all opponent pieces
	auto opponentPieces = chessboard.getPiecesFromPlayer(attackerColor);

	for (const auto &[pos, piece] : opponentPieces)
	{
		// Get possible moves for the opponent's piece
		auto moves = piece->getPossibleMoves(pos, chessboard);

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
	Position kingStart = move.start;
	Position kingEnd;
	Position rookStart;
	Position rookEnd;

	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside)
	{
		kingEnd	  = Position(kingStart.x + 2, kingStart.y); // King moves two squares right
		rookStart = Position(kingStart.x + 3, kingStart.y); // Rook's original position
		rookEnd	  = Position(kingStart.x + 1, kingStart.y); // Rook moves to the square left of the king
	}
	else if ((move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
	{
		kingEnd	  = Position(kingStart.x - 2, kingStart.y); // King moves two squares left
		rookStart = Position(kingStart.x - 4, kingStart.y); // Rook's original position
		rookEnd	  = Position(kingStart.x - 1, kingStart.y); // Rook moves to the square right of the king
	}

	mChessBoard->movePiece(kingStart, kingEnd);
	mChessBoard->movePiece(rookStart, rookEnd);

	return true;
}


std::vector<PossibleMove> MovementManager::generateCastlingMoves(const Position &kingPosition, PlayerColor player)
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


bool MovementManager::canCastle(const Position &kingposition, PlayerColor player, bool kingside)
{
	auto &king		= mChessBoard->getPiece(kingposition);
	int	  direction = kingside ? +1 : -1; // Determine the direction of castling

	if (king->getHasMoved())
		return false;

	// Define the y-coordinate and king's x-coordinate
	int		 y	   = kingposition.y;
	int		 kingX = kingposition.x;

	// Determine the rook's position based on the direction
	int		 rookX = (direction == 1) ? 7 : 0; // 7 for kingside (h-file), 0 for queenside (a-file)
	Position rookPosition{rookX, y};
	auto	&rook = mChessBoard->getPiece(rookPosition);

	if (!rook || rook->getType() != PieceType::Rook || rook->getColor() != player || rook->getHasMoved())
		return false;

	// Check if way is free
	for (int x = kingX + direction; x != rookX; x += direction)
	{
		Position pos{x, y};
		if (!mChessBoard->isEmpty(pos))
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


bool MovementManager::executeEnPassantMove(PossibleMove &move, PlayerColor player)
{
	Position capturedPawnPosition;
	if (player == PlayerColor::White)
	{
		capturedPawnPosition = Position(move.end.x, move.end.y - 1);
	}
	else
	{
		capturedPawnPosition = Position(move.end.x, move.end.y + 1);
	}

	mChessBoard->movePiece(move.start, move.end);
	mChessBoard->removePiece(capturedPawnPosition);

	return true;
}


PossibleMove MovementManager::generateEnPassantMove(const Position &position, PlayerColor player)
{
	if (!canEnPassant(position, player))
		return PossibleMove();

	auto	 lastMove = getLastMove();

	// Calculate target position
	Position targetPosition;
	if (player == PlayerColor::White)
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
	enPassantMove.type	= MoveType::EnPassant | MoveType::Capture;

	return enPassantMove;
}


bool MovementManager::canEnPassant(const Position &position, PlayerColor player)
{
	auto lastMove = getLastMove();

	if (!lastMove)
		return false;

	// Ensure if the last move was a pawn double push
	bool lastMoveWasPawnDoublePush = (lastMove->type & MoveType::DoublePawnPush) == MoveType::DoublePawnPush;
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
	if ((player == PlayerColor::White && position.y != 5) || (player == PlayerColor::Black && position.y != 4))
		return false;

	return true;
}


bool MovementManager::executePawnPromotion(const PossibleMove &move, PieceType promotedType)
{
	if ((move.type & MoveType::PawnPromotion) != MoveType::PawnPromotion)
		return false;

	// Validate promoted piece
	if ((promotedType != PieceType::Queen) && (promotedType != PieceType::Rook) && (promotedType != PieceType::Knight) && (promotedType != PieceType::Bishop))
	{
		return false;
	}

	// Get Pawn position and remove it
	auto &pawn = mChessBoard->getPiece(move.start);
	mChessBoard->removePiece(move.start);

	// Place promoted piece
	std::shared_ptr<ChessPiece> promotedPiece = nullptr;
	PlayerColor					player		  = pawn->getColor();

	switch (promotedType)
	{
	case PieceType::Queen:
	{
		promotedPiece = std::make_shared<Queen>(player);
		break;
	}

	case PieceType::Rook:
	{
		promotedPiece = std::make_shared<Rook>(player);
		break;
	}

	case PieceType::Knight:
	{
		promotedPiece = std::make_shared<Knight>(player);
		break;
	}

	case PieceType::Bishop:
	{
		promotedPiece = std::make_shared<Bishop>(player);
		break;
	}

	default: break;
	}

	if (promotedPiece)
	{
		mChessBoard->setPiece(move.start, promotedPiece);
		return true;
	}
	return false;
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

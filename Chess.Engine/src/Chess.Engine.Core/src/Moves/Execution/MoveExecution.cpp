/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#include "MoveExecution.h"
#include <strsafe.h>


MoveExecution::MoveExecution(Chessboard &board) : mChessBoard(board) {}


Move MoveExecution::executeMove(PossibleMove &possibleMove, bool fromRemote)
{
	// Store positions in Move from executed PossibleMove
	Move executedMove = Move(possibleMove);

	//// Store the moved piece type
	// auto &movedPiece   = mChessBoard->getPiece(possibleMove.start);

	// if (!movedPiece)
	//{
	//	LOG_ERROR("The piece that has been moved is empty. Position was x:{}, y:{}", possibleMove.start.x, possibleMove.start.y);
	//	return {};
	// }

	// auto movedPieceType		= movedPiece->getType();
	// auto player				= mChessBoard->getPiece(possibleMove.start)->getColor();

	// executedMove.movedPiece = movedPieceType;
	// executedMove.player		= player;
	// bool capturedPiece		= false;

	//{
	//	std::lock_guard<std::mutex> lock(mExecutionMutex);

	//	movedPiece->increaseMoveCounter();

	//	if (movedPieceType == PieceType::King)
	//	{
	//		mChessBoard->updateKingsPosition(executedMove.endingPosition, movedPiece->getColor());
	//	}

	//	// Store if this move captured another piece
	//	capturedPiece = (possibleMove.type & MoveType::Capture) == MoveType::Capture;
	//	if (capturedPiece)
	//	{
	//		auto &pieceCaptured = mChessBoard->getPiece(possibleMove.end);
	//		if (pieceCaptured)
	//		{
	//			auto capturedPieceType	   = pieceCaptured->getType();
	//			executedMove.capturedPiece = capturedPieceType;
	//			mChessBoard->movePiece(possibleMove.start, possibleMove.end);
	//		}
	//	}


	//	if ((possibleMove.type & MoveType::EnPassant) == MoveType::EnPassant)
	//	{
	//		bool result = executeEnPassantMove(possibleMove, player);
	//		if (result)
	//		{
	//			executedMove.capturedPiece = PieceType::Pawn;
	//		}
	//	}

	//	if ((possibleMove.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (possibleMove.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
	//	{
	//		executeCastlingMove(possibleMove);
	//	}

	//	if ((possibleMove.type & MoveType::Normal) == MoveType::Normal || (possibleMove.type & MoveType::DoublePawnPush) == MoveType::DoublePawnPush)
	//	{
	//		mChessBoard->movePiece(possibleMove.start, possibleMove.end);
	//	}

	//	if ((possibleMove.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	//	{
	//		executePawnPromotion(possibleMove, executedMove.player);
	//		executedMove.promotionType = possibleMove.promotionPiece;
	//	}

	//} // End critical section

	// PlayerColor opponent	 = player == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;
	// auto		opponentKing = mChessBoard->getKingsPosition(opponent);

	// if (mValidation->isCheckmate(opponent))
	//{
	//	executedMove.type |= MoveType::Checkmate;
	// }
	// else if (mValidation->isKingInCheck(opponentKing, opponent))
	//{
	//	executedMove.type |= MoveType::Check;
	// }

	//// Increment or reset the halfMoveClock

	// int previousHalfMoveClock = 0;

	// if (!mMoveHistory.empty())
	//{
	//	previousHalfMoveClock = getLastMove()->halfMoveClock;
	// }

	// if (movedPieceType != PieceType::Pawn && !capturedPiece)
	//{
	//	executedMove.halfMoveClock = previousHalfMoveClock + 1;
	// }
	// else
	//{
	//	executedMove.halfMoveClock = 0;
	// }

	// executedMove.notation = mMoveNotation->generateStandardAlgebraicNotation(executedMove);

	// for (auto &observer : mObservers)
	//{
	//	if (auto obs = observer.lock())
	//		obs->onExecuteMove(possibleMove, fromRemote);
	// }

	// addMoveToHistory(executedMove);

	return executedMove;
}


bool MoveExecution::executeCastlingMove(PossibleMove &move)
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

	// mChessBoard->movePiece(kingStart, kingEnd);
	// mChessBoard->movePiece(rookStart, rookEnd);

	return true;
}


bool MoveExecution::executeEnPassantMove(PossibleMove &move, PlayerColor player)
{
	Position capturedPawnPosition;
	if (player == PlayerColor::White)
	{
		capturedPawnPosition = Position(move.end.x, move.end.y + 1);
	}
	else
	{
		capturedPawnPosition = Position(move.end.x, move.end.y - 1);
	}

	// mChessBoard->movePiece(move.start, move.end);
	// mChessBoard->removePiece(capturedPawnPosition);

	return true;
}


bool MoveExecution::executePawnPromotion(const PossibleMove &move, PlayerColor player)
{
	// if ((move.type & MoveType::PawnPromotion) != MoveType::PawnPromotion)
	//	return false;

	// PieceType promotedPieceType = move.promotionPiece;

	//// Validate promoted piece
	// if ((promotedPieceType != PieceType::Queen) && (promotedPieceType != PieceType::Rook) && (promotedPieceType != PieceType::Knight) && (promotedPieceType !=
	// PieceType::Bishop)) 	return false;

	// mChessBoard->removePiece(move.start);

	//// Place promoted piece
	// std::shared_ptr<ChessPiece> promotedPiece = nullptr;

	// promotedPiece							  = ChessPiece::CreatePiece(promotedPieceType, player);

	// if (promotedPiece)
	//{
	//	mChessBoard->setPiece(move.end, promotedPiece);
	//	return true;
	// }
	return false;
}


const Move *MoveExecution::getLastMove() const
{
	if (mMoveHistory.empty())
		return nullptr;

	return &(*mMoveHistory.rbegin());
}


void MoveExecution::addMoveToHistory(Move &move)
{
	move.number = mMoveHistory.size() + 1; // Set the move number based on history size
	mMoveHistory.insert(move);

	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onAddToMoveHistory(move);
	}
}


void MoveExecution::clearMoveHistory()
{
	mMoveHistory.clear();

	for (auto &observer : mObservers)
	{
		if (auto obs = observer.lock())
			obs->onClearMoveHistory();
	}
}


void MoveExecution::removeLastMove()
{
	if (!mMoveHistory.empty())
	{
		mMoveHistory.erase(std::prev(mMoveHistory.end()));
	}
}

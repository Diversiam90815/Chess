/*
  ==============================================================================
	Module:         MoveGeneration
	Description:    Manages the generation of moves
  ==============================================================================
*/

#include "MoveGeneration.h"

MoveGeneration::MoveGeneration(Chessboard &board) : mChessBoard(board) {}


//
// std::vector<PossibleMove> MoveGeneration::getMovesForPosition(const Position &position)
//{
//	// auto &piece = mChessBoard->getPiece(position);
//
//	// if (!piece)
//	//	return {};
//
//	// auto					  player = piece->getColor();
//
//	// std::vector<PossibleMove> possibleMoves;
//
//	//{
//	//	std::lock_guard<std::mutex> lock(mMoveMutex);
//	//	possibleMoves = mAllLegalMovesForCurrentRound[position];
//	//}
//
//	//// Add special moves available for this position to the possibleMoves
//	// if (piece->getType() == PieceType::King)
//	//{
//	//	auto castlingMoves = generateCastlingMoves(position, player);
//	//	for (auto &move : castlingMoves)
//	//	{
//	//		possibleMoves.push_back(move);
//	//	}
//	// }
//
//	// if (piece->getType() == PieceType::Pawn)
//	//{
//	//	if (canEnPassant(position, player))
//	//	{
//	//		auto enPasssantMove = generateEnPassantMove(position, player);
//	//		possibleMoves.push_back(enPasssantMove);
//	//	}
//	// }
//
//	// if (GENERATION_DEBUG)
//	//	LOG_DEBUG("Position {} has {} possible moves!", LoggingHelper::positionToString(position).c_str(), possibleMoves.size());
//
//	// return possibleMoves;
//
//	return {};
//}
//
//
// bool MoveGeneration::calculateAllLegalBasicMoves(PlayerColor playerColor)
//{
//	// auto playerPieces = mChessBoard->getPiecesFromPlayer(playerColor);
//
//	//// Clear the previous round legal-moves map
//	//{
//	//	std::lock_guard<std::mutex> lock(mMoveMutex);
//	//	mAllLegalMovesForCurrentRound.clear();
//	//}
//
//	//// Container to hold futures for each piece move generation
//	// std::vector<std::future<std::pair<Position, std::vector<PossibleMove>>>> futures;
//	// futures.reserve(playerPieces.size());
//
//	//// Launch async task
//	// for (const auto &[startPosition, piece] : playerPieces)
//	//{
//	//	futures.push_back(std::async(std::launch::async,
//	//								 [this, playerColor, startPosition, piece]() -> std::pair<Position, std::vector<PossibleMove>>
//	//								 {
//	//									 // Generate all pseudo-legal moves for this piece
//	//									 auto					   possibleMoves = piece->getPossibleMoves(startPosition, *mChessBoard);
//
//	//									 // Validate them (i.e., filter out moves that leave the king in check)
//	//									 std::vector<PossibleMove> validMoves;
//	//									 validMoves.reserve(possibleMoves.size());
//
//	//									 for (const auto &pm : possibleMoves)
//	//									 {
//	//										 Move testMove(pm.start, pm.end, piece->getType());
//	//										 if (mValidation->validateMove(testMove, playerColor))
//	//										 {
//	//											 validMoves.push_back(pm);
//	//										 }
//	//									 }
//
//	//									 return {startPosition, std::move(validMoves)};
//	//								 }));
//	//}
//
//	// size_t totalValidMoves = 0;
//
//	// for (auto &fut : futures)
//	//{
//	//	auto  result = fut.get();
//	//	auto &pos	 = result.first;
//	//	auto &moves	 = result.second;
//	//	totalValidMoves += moves.size();
//
//	//	if (!moves.empty())
//	//	{
//	//		loadMoveToMap(pos, moves);
//	//	}
//	//}
//
//	// LOG_INFO("Calculating all moves finished, with {} moves!", totalValidMoves);
//
//	// return totalValidMoves != 0;
//
//	return false;
//}
//
//
// void MoveGeneration::loadMoveToMap(Position pos, std::vector<PossibleMove> moves)
//{
//	std::lock_guard<std::mutex> lock(mMoveMutex);
//	mAllLegalMovesForCurrentRound.emplace(pos, std::move(moves));
//}
//
//
// std::vector<PossibleMove> MoveGeneration::generateCastlingMoves(const Position &kingPosition, PlayerColor player)
//{
//	std::vector<PossibleMove> castlingMoves;
//	castlingMoves.reserve(2 * sizeof(castlingMoves)); // There are max. of 2 moves of castling
//
//	if (canCastle(kingPosition, player, true))
//	{
//		PossibleMove kingsideCastling;
//		kingsideCastling.start = kingPosition;
//		kingsideCastling.end   = Position{kingPosition.x + 2, kingPosition.y};
//		kingsideCastling.type  = MoveType::CastlingKingside;
//		castlingMoves.push_back(kingsideCastling);
//	}
//
//	if (canCastle(kingPosition, player, false))
//	{
//		PossibleMove queensideCastling;
//		queensideCastling.start = kingPosition;
//		queensideCastling.end	= Position{kingPosition.x - 2, kingPosition.y};
//		queensideCastling.type	= MoveType::CastlingQueenside;
//		castlingMoves.push_back(queensideCastling);
//	}
//
//	return castlingMoves;
//}
//
//
// bool MoveGeneration::canCastle(const Position &kingposition, PlayerColor player, bool kingside)
//{
//	// auto &king		= mChessBoard->getPiece(kingposition);
//	// int	  direction = kingside ? +1 : -1; // Determine the direction of castling
//
//	// if (king->hasMoved())
//	//	return false;
//
//	// if (mValidation->isKingInCheck(kingposition, player))
//	//	return false;
//
//	//// Define the y-coordinate and king's x-coordinate
//	// int		 y	   = kingposition.y;
//	// int		 kingX = kingposition.x;
//
//	//// Determine the rook's position based on the direction
//	// int		 rookX = (direction == 1) ? 7 : 0; // 7 for kingside (h-file), 0 for queenside (a-file)
//	// Position rookPosition{rookX, y};
//	// auto	&rook = mChessBoard->getPiece(rookPosition);
//
//	// if (!rook || rook->getType() != PieceType::Rook || rook->getColor() != player || rook->hasMoved())
//	//	return false;
//
//	//// Check if way is free
//	// for (int x = kingX + direction; x != rookX; x += direction)
//	//{
//	//	Position pos{x, y};
//	//	if (!mChessBoard->isEmpty(pos))
//	//		return false;
//	// }
//
//	//// Check if way is under attack
//	// std::vector<Position> positionsToCheck = {{kingX + direction, y}, {kingX + 2 * direction, y}};
//	// for (const auto &pos : positionsToCheck)
//	//{
//	//	Move testMove(kingposition, pos, PieceType::King);
//	//	if (mValidation->wouldKingBeInCheckAfterMove(testMove, player))
//	//		return false;
//	// }
//
//	return true;
//}
//
//
// PossibleMove MoveGeneration::generateEnPassantMove(const Position &position, PlayerColor player)
//{
//	// if (!canEnPassant(position, player))
//	//	return PossibleMove();
//
//	// auto	 lastMove = mExecution->getLastMove();
//
//	//// Calculate target position
//	// Position targetPosition;
//	// if (player == PlayerColor::White)
//	//{
//	//	targetPosition = Position(lastMove->endingPosition.x, lastMove->endingPosition.y - 1);
//	// }
//	// else
//	//{
//	//	targetPosition = Position(lastMove->endingPosition.x, lastMove->endingPosition.y + 1);
//	// }
//
//	PossibleMove enPassantMove;
//	enPassantMove.start = position;
//	// enPassantMove.end	= targetPosition;
//	enPassantMove.type	= MoveType::EnPassant | MoveType::Capture;
//
//	return enPassantMove;
//}
//
//
// bool MoveGeneration::canEnPassant(const Position &position, PlayerColor player)
//{
//	// auto lastMove = mExecution->getLastMove();
//
//	// if (!lastMove)
//	//	return false;
//
//	//// Ensure if the last move was a pawn double push
//	// bool lastMoveWasPawnDoublePush = (lastMove->type & MoveType::DoublePawnPush) == MoveType::DoublePawnPush;
//	// if (!lastMoveWasPawnDoublePush)
//	//	return false;
//
//	//// Ensure the last move was made by the opponent
//	// if (lastMove->player == player)
//	//	return false;
//
//	//// Ensure that the position match
//	// Position lastMoveEndPosition = lastMove->endingPosition;
//
//	//// Check if the last moved pawn is on the same rank as the capturing pawn
//	// bool	 sameRank			 = (lastMoveEndPosition.y == position.y);
//	// if (!sameRank)
//	//	return false;
//
//	//// Ensure both pawns are adjacent ranks
//	// bool bothPiecesNextToEachOther = (std::abs(lastMoveEndPosition.x - position.x) == 1);
//	// if (!bothPiecesNextToEachOther)
//	//	return false;
//
//	//// Ensure the capturing pawn is on the correct rank for en passant
//	// if ((player == PlayerColor::White && position.y != 3) || (player == PlayerColor::Black && position.y != 4))
//	//	return false;
//
//	return true;
//}
//

bool MoveGeneration::isSquareAttacked(Square square, Side side) const
{
	const auto &at		= AttackTables::instance();
	const U64	occBoth = mChessBoard.occ()[to_index(Side::Both)];

	// attacked by white pawns
	if ((side == Side::White) && (at.pawnAttacks(Side::Black, square) & mChessBoard.pieces()[WPawn]))
		return true;

	// attacked by black pawns
	if ((side == Side::Black) && (at.pawnAttacks(Side::White, square) & mChessBoard.pieces()[BPawn]))
		return true;

	// attacked by knights
	if (at.knightAttacks(square) & ((side == Side::White) ? mChessBoard.pieces()[WKnight] : mChessBoard.pieces()[BKnight]))
		return true;

	// attacked by kings
	if (at.kingAttacks(square) & ((side == Side::White) ? mChessBoard.pieces()[WKing] : mChessBoard.pieces()[BKing]))
		return true;

	// attacked by rooks
	if (at.rookAttacks(square, occBoth) & ((side == Side::White) ? mChessBoard.pieces()[WRook] : mChessBoard.pieces()[BRook]))
		return true;

	// attacked by bishops
	if (at.bishopAttacks(square, occBoth) & ((side == Side::White) ? mChessBoard.pieces()[WBishop] : mChessBoard.pieces()[BBishop]))
		return true;

	// attacked by queens
	if (at.queenAttacks(square, occBoth) & ((side == Side::White) ? mChessBoard.pieces()[WQueen] : mChessBoard.pieces()[BQueen]))
		return true;

	return false;
}


void MoveGeneration::generateAllMoves()
{
	Square sourceSquare = Square::None;
	Square targetSqaure = Square::None;

	U64	   bitboard		= 0ULL;
	U64	   captures		= 0ULL;

	Side   currentSide	= mChessBoard.getCurrentSide(); // TODO:: need to refactor

	// loop over all PieceTypes (TODO: Refactor later)
	for (int piece = WKing; piece <= BRook; piece++)
	{
		bitboard = mChessBoard.pieces()[piece]; // Get a copy of the piece's bitboard

		// generate pawn moves
		if (piece == WPawn)
			generatePawnMoves(bitboard, Side::White);
		else if (piece == BPawn)
			generatePawnMoves(bitboard, Side::Black);


		// generate knight moves

		// generate bishop moves

		// generate rook moves

		// generate queen moves

		// generate king moves
	}
}


void MoveGeneration::addMoveToList(PossibleMove &move) {}


void MoveGeneration::generateCastlingMoves(U64 bitboard)
{
	Castling castlingRights = mChessBoard.getCurrentCastlingRights();
	U64		 allOccupancies = mChessBoard.occ()[to_index(Side::Both)];

	if ((castlingRights & Castling::WK) != Castling::None)
	{
		// make sure squares between king and king's rook are empty
		if (!BitUtils::getBit(allOccupancies, to_index(Square::f1)) && !BitUtils::getBit(allOccupancies, to_index(Square::g1)))
		{
			// Make sure the king and the f1 square are not under attack
			if (!isSquareAttacked(Square::e1, Side::Black) && !isSquareAttacked(Square::f1, Side::Black))
			{
				// TODO: Add King side castling for white
			}
		}
	}
	if ((castlingRights & Castling::WQ) != Castling::None)
	{
		// make sure squares between king and queen's rook are empty
		if (!BitUtils::getBit(allOccupancies, to_index(Square::d1)) && !BitUtils::getBit(allOccupancies, to_index(Square::c1)))
		{
			// Make sure the king and the d1 square are not under attack
			if (!isSquareAttacked(Square::e1, Side::Black) && !isSquareAttacked(Square::d1, Side::Black))
			{
				// TODO: Add Queen side castling for white
			}
		}
	}

	if ((castlingRights & Castling::BK) != Castling::None)
	{
		// make sure squares between king and king's rook are empty
		if (!BitUtils::getBit(allOccupancies, to_index(Square::f8)) && !BitUtils::getBit(allOccupancies, to_index(Square::g8)))
		{
			// Make sure the king and the f8 square are not under attack
			if (!isSquareAttacked(Square::e8, Side::White) && !isSquareAttacked(Square::f8, Side::White))
			{
				// TODO: Add King side castling for black
			}
		}
	}
	if ((castlingRights & Castling::BQ) != Castling::None)
	{
		// make sure squares between king and queen's rook are empty
		if (!BitUtils::getBit(allOccupancies, to_index(Square::d8)) && !BitUtils::getBit(allOccupancies, to_index(Square::c8)))
		{
			// Make sure the king and the d8 square are not under attack
			if (!isSquareAttacked(Square::e8, Side::White) && !isSquareAttacked(Square::d8, Side::White))
			{
				// TODO: Add Queen side castling for black
			}
		}
	}
}


void MoveGeneration::generatePawnMoves(U64 bitboard, Side side)
{
	Square sourceSquare = Square::None;
	Square targetSqaure = Square::None;

	if (side == Side::White)
	{
		while (bitboard)
		{
			int source	 = BitUtils::lsb(bitboard);
			int target	 = source - 8;

			sourceSquare = (Square)source;
			targetSqaure = (Square)target;

			if (!(target < to_index(Square::a8)) && !BitUtils::getBit(mChessBoard.occ()[to_index(Side::Both)], target))
			{
				// pawn promotion
				if (sourceSquare >= Square::a7 && sourceSquare <= Square::h7)
				{
					// TODO: Add pawn promotion to move list
				}
				else
				{
					// TODO: Add pawn move to move list

					// Double pawn push
					if (sourceSquare >= Square::a2 && sourceSquare <= Square::h2 && !BitUtils::getBit(mChessBoard.occ()[to_index(Side::Both)], target - 8))
					{
						// TODO: Add double pawn push move to list
					}
				}
			}

			// captures
			U64 captures = AttackTables::instance().pawnAttacks(side, sourceSquare) & mChessBoard.occ()[to_index(Side::Black)];

			while (captures)
			{
				target = BitUtils::lsb(captures);

				// TODO: Check for promotion capture
				if (sourceSquare >= Square::a7 && sourceSquare <= Square::h7)
				{
					// Promotion capture
				}

				// TODO: Add capture move

				BitUtils::popBit(captures, target);
			}

			BitUtils::popBit(bitboard, source);
		}
	}
	else if (side == Side::Black)
	{

		while (bitboard)
		{
			int source = BitUtils::lsb(bitboard);
			int target = source + 8;

			if (!(target > to_index(Square::h1)) && !BitUtils::getBit(mChessBoard.occ()[to_index(Side::Both)], target))
			{
				sourceSquare = (Square)source;
				targetSqaure = (Square)target;

				// pawn promotion
				if (sourceSquare >= Square::a2 && sourceSquare <= Square::h2)
				{
					// TODO: Add pawn promotion to move list
				}
				else
				{
					// TODO: Add pawn move to move list

					// Double pawn push
					if (sourceSquare >= Square::a7 && sourceSquare <= Square::h7 && !BitUtils::getBit(mChessBoard.occ()[to_index(Side::Both)], target + 8))
					{
						// TODO: Add double pawn push move to list
					}
				}
			}

			// captures
			U64 captures = AttackTables::instance().pawnAttacks(side, sourceSquare) & mChessBoard.occ()[to_index(Side::White)];

			while (captures)
			{
				target = BitUtils::lsb(captures);

				// TODO: Check for promotion capture
				if (sourceSquare >= Square::a2 && sourceSquare <= Square::h2)
				{
					// Promotion capture
				}

				// TODO: Add capture move

				BitUtils::popBit(captures, target);
			}
		}
	}

	generateEnPassantMoves(side, sourceSquare);
}


void MoveGeneration::generateEnPassantMoves(Side side, Square sourceSquare)
{
	Square currentEnPassantSquare = mChessBoard.getCurrentEnPassantSqaure();

	if (currentEnPassantSquare != Square::None)
	{
		U64 enpassantCaptures = AttackTables::instance().pawnAttacks(side, sourceSquare) & (1ULL << to_index(currentEnPassantSquare));

		if (enpassantCaptures)
		{
			int targetEnPassant = BitUtils::lsb(enpassantCaptures);
			// TODO: Add En Passant
		}
	}
}

/*
  ==============================================================================
	Module:         MoveGeneration
	Description:    Manages the generation of moves
  ==============================================================================
*/

#include "MoveGeneration.h"


MoveGeneration::MoveGeneration(Chessboard &board) : mChessBoard(board) {}


bool MoveGeneration::isSquareAttacked(Square square, Side attacker) const
{
	const auto &at		 = AttackTables::instance();
	const U64	occBoth	 = mChessBoard.occ()[to_index(Side::Both)];
	const auto &pieces	 = mChessBoard.pieces();

	const int	offset	 = (attacker == Side::White) ? 0 : 6;

	// Pawns attack in reverse direction
	Side		pawnSide = (attacker == Side::White) ? Side::Black : Side::White;
	if (at.pawnAttacks(pawnSide, square) & pieces[WPawn + offset])
		return true;
	if (at.knightAttacks(square) & pieces[WKnight + offset])
		return true;

	if (at.kingAttacks(square) & pieces[WKing + offset])
		return true;

	if (at.bishopAttacks(square, occBoth) & pieces[WBishop + offset])
		return true;

	if (at.rookAttacks(square, occBoth) & pieces[WRook + offset])
		return true;

	if (at.queenAttacks(square, occBoth) & pieces[WQueen + offset])
		return true;

	return false;
}


void MoveGeneration::generateAllMoves(MoveList &moves)
{
	moves.clear();
	Side currentSide = mChessBoard.getCurrentSide();

	generatePawnMoves(moves, currentSide);
	generateKnightMoves(moves, currentSide);
	generateBishopMoves(moves, currentSide);
	generateRookMoves(moves, currentSide);
	generateQueenMoves(moves, currentSide);
	generateKingMoves(moves, currentSide);
	generateCastlingMoves(moves, currentSide);
}


void MoveGeneration::generateCastlingMoves(MoveList &moves, Side side)
{
	Castling rights = mChessBoard.getCurrentCastlingRights();
	U64		 occ	= mChessBoard.occ()[to_index(Side::Both)];
	Side	 enemy	= (side == Side::White) ? Side::Black : Side::White;

	if (side == Side::White)
	{
		// Kingside
		if ((rights & Castling::WK) != Castling::None)
		{
			if (!BitUtils::getBit(occ, to_index(Square::f1)) && !BitUtils::getBit(occ, to_index(Square::g1)) && !isSquareAttacked(Square::e1, enemy) &&
				!isSquareAttacked(Square::f1, enemy))
			{
				moves.push(Move(Square::e1, Square::g1, MoveFlag::KingCastle));
			}
		}
		// Queenside
		if ((rights & Castling::WQ) != Castling::None)
		{
			if (!BitUtils::getBit(occ, to_index(Square::d1)) && !BitUtils::getBit(occ, to_index(Square::c1)) && !BitUtils::getBit(occ, to_index(Square::b1)) &&
				!isSquareAttacked(Square::e1, enemy) && !isSquareAttacked(Square::d1, enemy))
			{
				moves.push(Move(Square::e1, Square::c1, MoveFlag::QueenCastle));
			}
		}
	}
	else
	{
		// Kingside
		if ((rights & Castling::BK) != Castling::None)
		{
			if (!BitUtils::getBit(occ, to_index(Square::f8)) && !BitUtils::getBit(occ, to_index(Square::g8)) && !isSquareAttacked(Square::e8, enemy) &&
				!isSquareAttacked(Square::f8, enemy))
			{
				moves.push(Move(Square::e8, Square::g8, MoveFlag::KingCastle));
			}
		}
		// Queenside
		if ((rights & Castling::BQ) != Castling::None)
		{
			if (!BitUtils::getBit(occ, to_index(Square::d8)) && !BitUtils::getBit(occ, to_index(Square::c8)) && !BitUtils::getBit(occ, to_index(Square::b8)) &&
				!isSquareAttacked(Square::e8, enemy) && !isSquareAttacked(Square::d8, enemy))
			{
				moves.push(Move(Square::e8, Square::c8, MoveFlag::QueenCastle));
			}
		}
	}
}


void MoveGeneration::generatePawnMoves(MoveList &moves, Side side)
{
	const auto &at			 = AttackTables::instance();
	const U64	occBoth		 = mChessBoard.occ()[to_index(Side::Both)];
	const U64	occEnemy	 = mChessBoard.occ()[to_index(side == Side::White ? Side::Black : Side::White)];

	const int	pawnType	 = (side == Side::White) ? WPawn : BPawn;
	U64			pawns		 = mChessBoard.pieces()[pawnType];

	const int	pushDir		 = (side == Side::White) ? -8 : 8;
	const int	startRankMin = (side == Side::White) ? to_index(Square::a2) : to_index(Square::a7);
	const int	startRankMax = (side == Side::White) ? to_index(Square::h2) : to_index(Square::h7);
	const int	promoRankMin = (side == Side::White) ? to_index(Square::a7) : to_index(Square::a2);
	const int	promoRankMax = (side == Side::White) ? to_index(Square::h7) : to_index(Square::h2);

	while (pawns)
	{
		int	   source	   = BitUtils::lsb(pawns);
		int	   target	   = source + pushDir;
		Square from		   = static_cast<Square>(source);
		Square to		   = static_cast<Square>(target);

		bool   isPromoRank = (source >= promoRankMin && source <= promoRankMax);

		// Single push
		if (!BitUtils::getBit(occBoth, target))
		{
			if (isPromoRank)
			{
				moves.push(Move(from, to, MoveFlag::QueenPromotion));
				moves.push(Move(from, to, MoveFlag::BishopPromotion));
				moves.push(Move(from, to, MoveFlag::RookPromotion));
				moves.push(Move(from, to, MoveFlag::KnightPromotion));
			}
			else
			{
				moves.push(Move(from, to, MoveFlag::Quiet));

				// double push
				if (source >= startRankMin && source <= startRankMax)
				{
					int doublePush = target + pushDir;

					if (!BitUtils::getBit(occBoth, doublePush))
						moves.push(Move(from, static_cast<Square>(doublePush), MoveFlag::DoublePawnPush));
				}
			}
		}

		// Captures
		U64 captures = at.pawnAttacks(side, from) & occEnemy;
		while (captures)
		{
			int	   capTarget = BitUtils::lsb(captures);
			Square capTo	 = static_cast<Square>(capTarget);

			if (isPromoRank)
			{
				moves.push(Move(from, capTo, MoveFlag::QueenPromoCapture));
				moves.push(Move(from, capTo, MoveFlag::BishopPromoCapture));
				moves.push(Move(from, capTo, MoveFlag::RookPromoCapture));
				moves.push(Move(from, capTo, MoveFlag::KnightPromoCapture));
			}
			else
				moves.push(Move(from, to, MoveFlag::Capture));

			BitUtils::popBit(captures, capTarget);
		}

		// En Passant
		Square epSquare = mChessBoard.getCurrentEnPassantSqaure();
		if (epSquare != Square::None)
		{
			U64 epCapture = at.pawnAttacks(side, from) & (1ULL << to_index(epSquare));

			if (epCapture)
				moves.push(Move(from, epSquare, MoveFlag::EnPassant));
		}

		BitUtils::popBit(pawns, source);
	}
}


void MoveGeneration::generateKnightMoves(MoveList &moves, Side side)
{
	const auto &at		   = AttackTables::instance();
	const int	knightType = (side == Side::White) ? WKnight : BKnight;
	U64			knights	   = mChessBoard.pieces()[knightType];
	const U64	ownOcc	   = mChessBoard.occ()[to_index(side)];
	const U64	enemyOcc   = mChessBoard.occ()[to_index(side == Side::White ? Side::Black : Side::White)];

	while (knights)
	{
		int	   source  = BitUtils::lsb(knights);
		Square from	   = static_cast<Square>(source);
		U64	   attacks = at.knightAttacks(from) & ~ownOcc;

		addSlidingMoves(moves, from, attacks, enemyOcc);

		BitUtils::popBit(knights, source);
	}
}


void MoveGeneration::generateRookMoves(MoveList &moves, Side side)
{
	const auto &at		 = AttackTables::instance();
	const int	rookType = (side == Side::White) ? WRook : BRook;
	U64			rooks	 = mChessBoard.pieces()[rookType];
	const U64	occBoth	 = mChessBoard.occ()[to_index(Side::Both)];
	const U64	ownOcc	 = mChessBoard.occ()[to_index(side)];
	const U64	enemyOcc = mChessBoard.occ()[to_index(side == Side::White ? Side::Black : Side::White)];

	while (rooks)
	{
		int	   source  = BitUtils::lsb(rooks);
		Square from	   = static_cast<Square>(source);
		U64	   attacks = at.rookAttacks(from, occBoth) & ~ownOcc;

		addSlidingMoves(moves, from, attacks, enemyOcc);

		BitUtils::popBit(rooks, source);
	}
}


void MoveGeneration::generateBishopMoves(MoveList &moves, Side side)
{
	const auto &at		   = AttackTables::instance();
	const int	bishopType = (side == Side::White) ? WBishop : BBishop;
	U64			bishops	   = mChessBoard.pieces()[bishopType];
	const U64	occBoth	   = mChessBoard.occ()[to_index(Side::Both)];
	const U64	ownOcc	   = mChessBoard.occ()[to_index(side)];
	const U64	enemyOcc   = mChessBoard.occ()[to_index(side == Side::White ? Side::Black : Side::White)];

	while (bishops)
	{
		int	   source  = BitUtils::lsb(bishops);
		Square from	   = static_cast<Square>(source);
		U64	   attacks = at.bishopAttacks(from, occBoth) & ~ownOcc;

		addSlidingMoves(moves, from, attacks, enemyOcc);

		BitUtils::popBit(bishops, source);
	}
}


void MoveGeneration::generateQueenMoves(MoveList &moves, Side side)
{
	const auto &at		  = AttackTables::instance();
	const int	queenType = (side == Side::White) ? WQueen : BQueen;
	U64			queens	  = mChessBoard.pieces()[queenType];
	const U64	occBoth	  = mChessBoard.occ()[to_index(Side::Both)];
	const U64	ownOcc	  = mChessBoard.occ()[to_index(side)];
	const U64	enemyOcc  = mChessBoard.occ()[to_index(side == Side::White ? Side::Black : Side::White)];

	while (queens)
	{
		int	   source  = BitUtils::lsb(queens);
		Square from	   = static_cast<Square>(source);
		U64	   attacks = at.queenAttacks(from, occBoth) & ~ownOcc;

		addSlidingMoves(moves, from, attacks, enemyOcc);

		BitUtils::popBit(queens, source);
	}
}


void MoveGeneration::generateKingMoves(MoveList &moves, Side side)
{
	const auto &at		 = AttackTables::instance();
	const int	kingType = (side == Side::White) ? WKing : BKing;
	U64			king	 = mChessBoard.pieces()[kingType];
	const U64	ownOcc	 = mChessBoard.occ()[to_index(side)];
	const U64	enemyOcc = mChessBoard.occ()[to_index(side == Side::White ? Side::Black : Side::White)];

	if (king)
	{
		int	   source  = BitUtils::lsb(king);
		Square from	   = static_cast<Square>(source);
		U64	   attacks = at.kingAttacks(from) & ~ownOcc;

		addSlidingMoves(moves, from, attacks, enemyOcc);
	}
}


void MoveGeneration::addSlidingMoves(MoveList &moves, Square from, U64 attacks, U64 enemyOcc)
{
	while (attacks)
	{
		int	   target = BitUtils::lsb(attacks);
		Square to	  = static_cast<Square>(target);

		if (BitUtils::getBit(enemyOcc, target))
			moves.push(Move(from, to, MoveFlag::Capture));
		else
			moves.push(Move(from, to, MoveFlag::Quiet));

		BitUtils::popBit(attacks, target);
	}
}

/*
  ==============================================================================
	Module:         Bitboard
	Description:    A bitboard representing a chess board
  ==============================================================================
*/

#include "Chessboard.h"
#include <stdio.h>
#include <string.h>


void Chessboard::init()
{
	ZobristHash::initialize();
	parseFEN(mStartPosition);
}


void Chessboard::clear()
{
	mBitBoards.fill(0);
	mOccupancyBitboards.fill(0);

	mSide			 = Side::None;
	mEnPassantSquare = Square::None;
	mCastlingRights	 = Castling::None;

	mHalfMoveClock	 = 0;
	mMoveCounter	 = 0;
	mHash			 = 0;
}


void Chessboard::parseFEN(std::string_view fen)
{
	clear();

	size_t i		  = 0;

	auto   skipSpaces = [&]()
	{
		while (i < fen.size() && fen[i] == ' ')
			++i;
	};

	// 1 Piece placement
	int rank = 0;
	int file = 0;

	while (i < fen.size() && fen[i] != ' ')
	{
		char c = fen[i++];

		if (c == '/')
		{
			++rank;
			file = 0;
			continue;
		}

		if (c >= '1' && c <= '8')
		{
			file += (c - '0');
			continue;
		}

		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			int piece  = GetPieceTypeFromChar(c);
			int square = rank * 8 + file;
			BitUtils::setBit(mBitBoards[piece], square);
			++file;
		}
	}

	skipSpaces();

	// 2 Side to move
	if (i <= fen.size())
	{
		mSide = (fen[i] == 'w') ? Side::White : Side::Black;
		++i;
	}

	skipSpaces();

	// 3 Castling
	while (i < fen.size() && fen[i] != ' ')
	{
		switch (fen[i])
		{
		case 'K': mCastlingRights |= Castling::WK; break;
		case 'Q': mCastlingRights |= Castling::WQ; break;
		case 'k': mCastlingRights |= Castling::BK; break;
		case 'q': mCastlingRights |= Castling::BQ; break;
		case '-': break;
		}
		++i;
	}

	skipSpaces();

	// 4 En Passant
	if (i < fen.size() && fen[i] != '-')
	{
		int epFile		 = fen[i + 0] - 'a';
		int epRank		 = 8 - (fen[i + 1] - '0');
		mEnPassantSquare = (Square)(epRank * 8 + epFile);
	}
	else
	{
		mEnPassantSquare = Square::None;
	}

	// 5 occupancies
	updateOccupancies();

	// 6 Compute hash
	computeHash();
}


void Chessboard::removePiece(PieceType piece, Square sq)
{
	if (piece == PieceType::None)
		return;

	BitUtils::popBit(mBitBoards[piece], to_index(sq));

	// Update hash
	hashPiece(piece, sq);
}


void Chessboard::addPiece(PieceType piece, Square sq)
{
	if (piece == PieceType::None)
		return;

	BitUtils::setBit(mBitBoards[piece], to_index(sq));

	// update hash
	hashPiece(piece, sq);
}


void Chessboard::movePiece(PieceType piece, Square from, Square to)
{
	removePiece(piece, from);
	addPiece(piece, to);
}


void Chessboard::updateOccupancies()
{
	mOccupancyBitboards[to_index(Side::White)] = mBitBoards[WKing] | mBitBoards[WQueen] | mBitBoards[WPawn] | mBitBoards[WKnight] | mBitBoards[WBishop] | mBitBoards[WRook];
	mOccupancyBitboards[to_index(Side::Black)] = mBitBoards[BKing] | mBitBoards[BQueen] | mBitBoards[BPawn] | mBitBoards[BKnight] | mBitBoards[BBishop] | mBitBoards[BRook];

	mOccupancyBitboards[to_index(Side::Both)]  = mOccupancyBitboards[to_index(Side::White)] | mOccupancyBitboards[to_index(Side::Black)];
}


PieceType Chessboard::pieceAt(Square sq) const
{
	int sqIndex = to_index(sq);
	for (int p = 0; p < 12; ++p)
	{
		if (BitUtils::getBit(mBitBoards[p], sqIndex))
			return (PieceType)p;
	}
	return PieceType::None;
}


void Chessboard::setSide(Side s) noexcept
{
	if (mSide == s)
		return;

	// Remove old hash
	if (mSide == Side::Black)
		hashSide();

	mSide = s;

	// update hash
	if (mSide == Side::Black)
		hashSide();
}


void Chessboard::flipSide() noexcept
{
	Side newSide = getCurrentSide() == Side::White ? Side::Black : Side::White;
	setSide(newSide);
}


void Chessboard::setCastlingRights(Castling c) noexcept
{
	// Remove old hash
	hashCastling(mCastlingRights);

	// adapt new castling
	mCastlingRights = c;

	// update hash
	hashCastling(c);
}


void Chessboard::setEnPassantSquare(Square sq) noexcept
{
	// remove old hash
	hashEnPassant(mEnPassantSquare);

	// update en passant square
	mEnPassantSquare = sq;

	// update hash
	hashEnPassant(sq);
}


BoardState Chessboard::saveState() const
{
	return {mCastlingRights, mEnPassantSquare, mHalfMoveClock, PieceType::None, mHash};
}


void Chessboard::restoreState(const BoardState &state)
{
	mCastlingRights	 = state.castle;
	mEnPassantSquare = state.enPassant;
	mHalfMoveClock	 = state.halfMoveClock;
	mHash			 = state.hash;
}


void Chessboard::computeHash()
{
	mHash = 0;

	// hash all pieces
	for (int piece = 0; piece < 12; piece++)
	{
		U64 bb = mBitBoards[piece];

		while (bb)
		{
			int sq = BitUtils::lsb(bb);
			hashPiece((PieceType)piece, (Square)sq);
			BitUtils::popBit(bb, sq);
		}
	}

	// hash side to move
	if (mSide == Side::Black)
		hashSide();

	// hash castling rights
	hashCastling(mCastlingRights);

	// hash enpassant
	hashEnPassant(mEnPassantSquare);
}

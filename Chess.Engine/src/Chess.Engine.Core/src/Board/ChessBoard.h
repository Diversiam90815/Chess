/*
  ==============================================================================
	Module:         Bitboard
	Description:    A bitboard representing a chess board
  ==============================================================================
*/

#pragma once

#include <string>
#include <algorithm>
#include <cctype>
#include <array>

#include "BitboardTypes.h"
#include "AttackTables.h"

/*
							ALL TOGETHER

						8  ♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜
						7  ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎ ♟︎
						6  . . . . . . . .
						5  . . . . . . . .
						4  . . . . . . . .
						3  . . . . . . . .
						2  ♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙
						1  ♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖

						   a b c d e f g h


							WHITE PIECES

		Pawns                  Knights              Bishops

  8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  0 0 0 0 0 0 0 0    1  0 1 0 0 0 0 1 0    1  0 0 1 0 0 1 0 0

	 a b c d e f g h       a b c d e f g h       a b c d e f g h

		 Rooks                 Queens                 King

  8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 0 0 0
  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  1 0 0 0 0 0 0 1    1  0 0 0 1 0 0 0 0    1  0 0 0 0 1 0 0 0

	 a b c d e f g h       a b c d e f g h       a b c d e f g h


							BLACK PIECES

		Pawns                  Knights              Bishops

  8  0 0 0 0 0 0 0 0    8  0 1 0 0 0 0 1 0    8  0 0 1 0 0 1 0 0
  7  1 1 1 1 1 1 1 1    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0

	 a b c d e f g h       a b c d e f g h       a b c d e f g h

		 Rooks                 Queens                 King

  8  1 0 0 0 0 0 0 1    8  0 0 0 1 0 0 0 0    8  0 0 0 0 1 0 0 0
  7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 0 0 0
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 0 0 0
  1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 0 0 0

	 a b c d e f g h       a b c d e f g h       a b c d e f g h


							 OCCUPANCIES

	 White occupancy       Black occupancy       All occupancies

  8  0 0 0 0 0 0 0 0    8  1 1 1 1 1 1 1 1    8  1 1 1 1 1 1 1 1
  7  0 0 0 0 0 0 0 0    7  1 1 1 1 1 1 1 1    7  1 1 1 1 1 1 1 1
  6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0    6  0 0 0 0 0 0 0 0
  5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 0 0 0
  4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 0 0 0
  3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 0 0 0
  2  1 1 1 1 1 1 1 1    2  0 0 0 0 0 0 0 0    2  1 1 1 1 1 1 1 1
  1  1 1 1 1 1 1 1 1    1  0 0 0 0 0 0 0 0    1  1 1 1 1 1 1 1 1
*/

// State snapshot for unmake move
struct BoardState
{
	Castling  castle		= Castling::None;
	Square	  enPassant		= Square::None;
	int		  halfMoveClock = 0;
	PieceType capturedPiece = PieceType::None;
};


class Chessboard
{
public:
	Chessboard()	  = default;
	~Chessboard()	  = default;

	using Bitboards	  = std::array<U64, 12>;
	using Occupancies = std::array<U64, 3>;

	void							 init();
	void							 clear();
	void							 parseFEN(std::string_view fen);

	void							 removePiece(PieceType piece, Square sq);
	void							 addPiece(PieceType piece, Square sq);
	void							 movePiece(PieceType piece, Square from, Square to);
	void							 updateOccupancies();

	// Piece lookup
	[[nodiscard]] PieceType			 pieceAt(Square sq) const;

	[[nodiscard]] const Bitboards	&pieces() const noexcept { return mBitBoards; }
	[[nodiscard]] Bitboards			&pieces() noexcept { return mBitBoards; }
	[[nodiscard]] const Occupancies &occ() const noexcept { return mOccupancyBitboards; }

	[[nodiscard]] Side				 getCurrentSide() const noexcept { return mSide; }
	[[nodiscard]] Castling			 getCurrentCastlingRights() const noexcept { return mCastlingRights; }
	[[nodiscard]] Square			 getCurrentEnPassantSqaure() const noexcept { return mEnPassantSquare; }
	[[nodiscard]] int				 getHalfMoveClock() const noexcept { return mHalfMoveClock; }

	void							 setSide(Side s) noexcept { mSide = s; }
	void							 flipSide() noexcept { mSide = mSide == Side::White ? Side::Black : Side::White; }
	void							 setCastlingRights(Castling c) noexcept { mCastlingRights = c; }
	void							 setEnPassantSquare(Square sq) noexcept { mEnPassantSquare = sq; }
	void							 setHalfMoveClock(int clock) noexcept { mHalfMoveClock = clock; }
	void							 incrementMoveCounter() noexcept { ++mMoveCounter; }
	void							 decrementMoveCounter() noexcept
	{
		if (mMoveCounter > 1)
			--mMoveCounter;
	}

	[[nodiscard]] BoardState saveState() const;
	void					 restoreState(const BoardState &state);

private:
	Bitboards		  mBitBoards{};						 // Array of all bitboards
	Occupancies		  mOccupancyBitboards{};			 // Occupancies

	Side			  mSide			   = Side::None;	 // side to move
	Square			  mEnPassantSquare = Square::None;	 // enpassant square
	Castling		  mCastlingRights  = Castling::None; // castling rights

	int				  mHalfMoveClock   = 0;
	int				  mMoveCounter	   = 1;

	// FEN positions
	const std::string mEmptyBoard	   = "8/8/8/8/8/8/8/8 w - - ";
	const std::string mStartPosition   = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
};

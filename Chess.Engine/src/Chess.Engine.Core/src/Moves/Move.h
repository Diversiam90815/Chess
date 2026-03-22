/*
  ==============================================================================
	Module:         Move
	Description:    Storing information classifying a move in chess
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"


/*
	Move encoding (16-bit):

	0000 0000 0011 1111    source square       (6 bits, 0-63)
	0000 1111 1100 0000    target square       (6 bits, 0-63)
	1111 0000 0000 0000    flags               (4 bits)

	Flags:
	0000 = quiet move
	0001 = double pawn push
	0010 = king castle
	0011 = queen castle
	0100 = capture
	0101 = en passant capture
	1000 = knight promotion
	1001 = bishop promotion
	1010 = rook promotion
	1011 = queen promotion
	1100 = knight promo capture
	1101 = bishop promo capture
	1110 = rook promo capture
	1111 = queen promo capture
*/
enum class MoveFlag : uint8_t
{
	Quiet			   = 0b0000,
	DoublePawnPush	   = 0b0001,
	KingCastle		   = 0b0010,
	QueenCastle		   = 0b0011,
	Capture			   = 0b0100,
	EnPassant		   = 0b0101,
	KnightPromotion	   = 0b1000,
	BishopPromotion	   = 0b1001,
	RookPromotion	   = 0b1010,
	QueenPromotion	   = 0b1011,
	KnightPromoCapture = 0b1100,
	BishopPromoCapture = 0b1101,
	RookPromoCapture   = 0b1110,
	QueenPromoCapture  = 0b1111
};


class Move
{
public:
	constexpr Move() : mData(0) {}
	constexpr Move(Square from, Square to, MoveFlag flags = MoveFlag::Quiet)
		: mData(static_cast<uint16_t>(to_index(from)) | (static_cast<uint16_t>(to_index(to)) << 6) | (static_cast<uint16_t>(flags) << 12))
	{
	}
	constexpr Move(const uint16_t data) : mData(data) {}

	// Accessors
	[[nodiscard]] constexpr Square	 from() const { return static_cast<Square>(mData & 0x3F); }
	[[nodiscard]] constexpr Square	 to() const { return static_cast<Square>((mData >> 6) & 0x3F); }
	[[nodiscard]] constexpr MoveFlag flags() const { return static_cast<MoveFlag>((mData >> 12) & 0x0F); }
	[[nodiscard]] constexpr uint16_t raw() const { return mData; }

	// Move type checks
	[[nodiscard]] constexpr bool	 isQuiet() const { return flags() == MoveFlag::Quiet; }
	[[nodiscard]] constexpr bool	 isCapture() const { return (mData >> 12) & 0b0100; }
	[[nodiscard]] constexpr bool	 isPromotion() const { return (mData >> 12) & 0b1000; }
	[[nodiscard]] constexpr bool	 isCastle() const { return flags() == MoveFlag::KingCastle || flags() == MoveFlag::QueenCastle; }
	[[nodiscard]] constexpr bool	 isEnPassant() const { return flags() == MoveFlag::EnPassant; }
	[[nodiscard]] constexpr bool	 isDoublePush() const { return flags() == MoveFlag::DoublePawnPush; }
	[[nodiscard]] constexpr bool	 isValid() const { return mData != 0; }

	// Get promotion piece type (only valid if isPromotion() is true)
	[[nodiscard]] constexpr int		 promotionPieceOffset() const
	{
		// Returns offset: 0=Knight, 1=Bishop, 2=Rook, 3=Queen
		return (static_cast<int>(flags()) & 0b0011);
	}

	constexpr bool		  operator==(const Move &other) const { return mData == other.mData; }
	constexpr bool		  operator!=(const Move &other) const { return mData != other.mData; }

	static constexpr Move none() { return Move(); }

private:
	uint16_t mData;
};


// Pre-sized move list for performance
class MoveList
{
public:
	static constexpr size_t MAX_MOVES = 256; // Max legal moves in any position is 218

	MoveList() : mCount(0) {}

	void				 push(Move move) { mMoves[mCount++] = move; }
	void				 clear() { mCount = 0; }

	[[nodiscard]] size_t size() const { return mCount; }
	[[nodiscard]] bool	 empty() const { return mCount == 0; }

	Move				&operator[](size_t i) { return mMoves[i]; }
	const Move			&operator[](size_t i) const { return mMoves[i]; }

	// Iterator support
	Move				*begin() { return mMoves; }
	Move				*end() { return mMoves + mCount; }
	const Move			*begin() const { return mMoves; }
	const Move			*end() const { return mMoves + mCount; }

private:
	Move   mMoves[MAX_MOVES];
	size_t mCount;
};


/**
 * @brief	Represents an in-progress move from user input.
 *			Replaces multiple boolean flags with a single coherent structure.
 */
struct MoveIntent
{
	Square	  fromSquare = Square::None;
	Square	  toSquare	 = Square::None;
	PieceType promotion	 = PieceType::None;
	MoveList  legalMoves; // Cached legal moves from selected square
	bool	  fromRemote = false;
	bool	  fromCPU	 = false;

	void	  clear()
	{
		fromSquare = Square::None;
		toSquare   = Square::None;
		promotion  = PieceType::None;
		fromRemote = false;
		fromCPU	   = false;
		legalMoves.clear();
	}

	[[nodiscard]] bool hasSource() const { return fromSquare != Square::None; }
	[[nodiscard]] bool hasTarget() const { return toSquare != Square::None; }
	[[nodiscard]] bool isComplete() const { return hasSource() && hasTarget(); }
};

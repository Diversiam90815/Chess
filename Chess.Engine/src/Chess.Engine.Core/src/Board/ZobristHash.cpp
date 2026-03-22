/*
  ==============================================================================
	Module:         ZobristHash
	Description:    Zobrist hashing for board position identification
  ==============================================================================
*/

#include "ZobristHash.h"

// Static member definitions
bool									 ZobristHash::mInitialized = false;
std::array<std::array<uint64_t, 64>, 12> ZobristHash::mPieceKeys{};
uint64_t								 ZobristHash::mSideKey = 0;
std::array<uint64_t, 16>				 ZobristHash::mCastlingKeys{};
std::array<uint64_t, 8>					 ZobristHash::mEnPassantKeys{};


void									 ZobristHash::initialize()
{
	if (mInitialized)
		return;

	// Use fixed seed for reproducibility
	std::mt19937_64 rng(0x1234567890ABCDEFULL);

	// Generate piece-square keys
	for (int piece = 0; piece < 12; ++piece)
	{
		for (int sq = 0; sq < 64; ++sq)
		{
			mPieceKeys[piece][sq] = rng();
		}
	}

	// Side to move key
	mSideKey = rng();

	// castling keys
	for (int i = 0; i < 16; ++i)
	{
		mCastlingKeys[i] = rng();
	}

	// enpassant keys
	for (int i = 0; i < 8; ++i)
	{
		mEnPassantKeys[i] = rng();
	}

	mInitialized = true;
}

/*
  ==============================================================================
	Module:         MoveGeneration
	Description:    Manages the generation of moves
  ==============================================================================
*/

#pragma once

#include <vector>
#include <unordered_map>
#include <future>
#include <memory>

#include "Chessboard.h"
#include "Logging.h"
#include "Move.h"
#include "Execution/MoveExecution.h"
#include "BitboardTypes.h"


#define GENERATION_DEBUG false


class MoveGeneration
{
public:
	MoveGeneration(Chessboard &board);
	~MoveGeneration() = default;

	// std::vector<PossibleMove> getMovesForPosition(const Position &position);

	// bool					  calculateAllLegalBasicMoves(PlayerColor playerColor);


	bool isSquareAttacked(Square square, Side side) const;
	void generateAllMoves();

private:
	// std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PlayerColor player);

	// bool													canCastle(const Position &kingposition, PlayerColor player, bool kingside);

	// PossibleMove											generateEnPassantMove(const Position &position, PlayerColor player);

	// bool													canEnPassant(const Position &position, PlayerColor player);

	// void													loadMoveToMap(Position pos, std::vector<PossibleMove> moves);

	void												  addMoveToList(PossibleMove &move);

	void												  generateCastlingMoves(U64 bitboard);
	void												  generatePawnMoves(U64 bitboard, Side side);
	void												  generateEnPassantMoves(Side side, Square sourceSquare);
	void												  generateKnightMoves(U64 bitboard, Side side);
	void												  generateRookMoves(U64 bitboard, Side side);
	void												  generateBishopMoves(U64 bitboard, Side side);
	void												  generateQueenMoves(U64 bitboard, Side side);
	void												  generateKingMoves(U64 bitboard, Side side);

	Chessboard											 &mChessBoard;

	std::unordered_map<Square, std::vector<PossibleMove>> mAllMoves;
	std::mutex											  mMoveMutex;
};

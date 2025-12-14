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

#include "ChessBoard.h"
#include "Logging.h"
#include "Move.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"


#define GENERATION_DEBUG false


/**
 * @brief	Generates and caches legal moves for a given board position.
 *
 * Strategy:
 *  - Generate candidate (pseudo-legal) moves.
 *  - Filter using MoveValidation (ensuring king safety, etc.).
 *  - Cache per starting square for quick UI / engine queries.
 */
class MoveGeneration
{
public:
	MoveGeneration(std::shared_ptr<ChessBoard> board, std::shared_ptr<MoveValidation> validation, std::shared_ptr<MoveExecution> execution);
	~MoveGeneration();

	/**
	 * @brief	Get legal moves for a specific starting square (Position).
	 */
	std::vector<PossibleMove> getMovesForPosition(const Position &position);

	/**
	 * @brief	Calculate all legal basic (non-search) moves for current player.
	 * @return	true if successfully cached.
	 */
	bool					  calculateAllLegalBasicMoves(PlayerColor playerColor);

private:
	/**
	 * @brief	Generate (and validate) castling moves for a king.
	 */
	std::vector<PossibleMove>								generateCastlingMoves(const Position &kingPosition, PlayerColor player);

	/**
	 * @brief	Determine if castling is permitted for side (kingside/queenside).
	 */
	bool													canCastle(const Position &kingposition, PlayerColor player, bool kingside);

	/**
	 * @brief	Produce an en passant capture move candidate (if available).
	 */
	PossibleMove											generateEnPassantMove(const Position &position, PlayerColor player);

	/**
	 * @brief	Check if en passant is possible from position.
	 */
	bool													canEnPassant(const Position &position, PlayerColor player);

	/**
	 * @brief	Insert list of moves into the cache map for quick retrieval.
	 * @param	pos -> start position from the move
	 * @param	moves -> available moves from that position
	 */
	void													loadMoveToMap(Position pos, std::vector<PossibleMove> moves);


	std::shared_ptr<ChessBoard>								mChessBoard;
	std::shared_ptr<MoveValidation>							mValidation;
	std::shared_ptr<MoveExecution>							mExecution;
	std::unordered_map<Position, std::vector<PossibleMove>> mAllLegalMovesForCurrentRound;
	std::mutex												mMoveMutex;
};

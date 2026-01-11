/*
  ==============================================================================
	Module:			CPUPlayer
	Description:    Managing the CPU opponent player
  ==============================================================================
*/

#pragma once

#include <memory>
#include <random>
#include <thread>
#include <algorithm>

#include "Parameters.h"
#include "GameEngine.h"


/**
 * @brief	Transposition table entry for search optimization.
 */
struct TranspositionEntry
{
	enum class NodeType
	{
		Exact,
		LowerBound,
		UpperBound,
	};

	uint64_t hash{};
	int		 depth{};
	int		 score{};
	NodeType type{NodeType::Exact};
	Move	 bestMove{};
};


/**
 * @brief	Move with associated score for sorting/selection.
 */
struct ScoredMove
{
	Move move;
	int	 score;

	bool operator<(const ScoredMove &other) { return score < other.score; }
};


/**
 * @brief	Configuration for CPU player behavior.
 */
struct CPUConfiguration
{
	bool		  enabled			  = false;
	Side		  cpuColor			  = Side::Black; // Default to black
	CPUDifficulty difficulty		  = CPUDifficulty::Medium;
	bool		  enableRandomization = true;		 // Add some randomness to move selection
	int			  maxDepth			  = 6;
};


/**
 * @brief	AI chess player using minimax with alpha-beta pruning.
 *			Works directly with bitboard Move type.
 */
class CPUPlayer
{
public:
	explicit CPUPlayer(GameEngine &engine);
	~CPUPlayer();

	// Non-copyable
	CPUPlayer(const CPUPlayer &)				  = delete;
	CPUPlayer		&operator=(const CPUPlayer &) = delete;

	//=========================================================================
	// Configuration
	//=========================================================================

	void			 configure(const CPUConfiguration &config);
	CPUConfiguration getConfiguration() const { return mConfig; }

	void			 setEnabled(bool enabled) { mConfig.enabled = enabled; }
	bool			 isEnabled() const { return mConfig.enabled; }
	bool			 isCPUPlayer(Side side) const { return mConfig.enabled && mConfig.cpuColor == side; }

	//=========================================================================
	// Move Calculation
	//=========================================================================

	/**
	 * @brief	Calculate best move asynchronously.
	 * @param	callback Called on completion with the chosen move.
	 */
	void			 calculateMoveAsync(std::function<void(Move)> callback);

	/**
	 * @brief	Calculate best move synchronously (blocking).
	 * @return	Best move found, or null move if none available.
	 */
	Move			 calculateMove();

	/**
	 * @brief	Cancel any ongoing calculation.
	 */
	void			 cancelCalculation();

	/**
	 * @brief	Check if calculation is in progress.
	 */
	bool			 isCalculating() const { return mIsCalculating.load(); }


private:
	//=========================================================================
	// Search Algorithms
	//=========================================================================

	/**
	 * @brief	Top-level search dispatcher based on difficulty.
	 */
	Move											 computeBestMove(std::stop_token stopToken);

	/**
	 * @brief	Minimax search with alpha-beta pruning.
	 */
	Move											 searchAlphaBeta(const MoveList &moves, int depth, std::stop_token stopToken);

	/**
	 * @brief	Recursive alpha-beta implementation.
	 */
	int												 alphaBeta(int depth, int alpha, int beta, bool maximizing, std::stop_token stopToken);

	/**
	 * @brief	Quiescence search to avoid horizon effect.
	 */
	int												 quiescence(int alpha, int beta, std::stop_token stopToken);

	//=========================================================================
	// Move Selection
	//=========================================================================

	Move											 selectBestMove(std::vector<ScoredMove> &scoredMoves);
	Move											 selectWithRandomization(std::vector<ScoredMove> &scoredMoves);
	std::vector<ScoredMove>							 filterTopCandidates(std::vector<ScoredMove> &moves, size_t maxCount) const;


	//=========================================================================
	// Transposition Table
	//=========================================================================

	void											 storeTransposition(uint64_t hash, int depth, int score, TranspositionEntry::NodeType type, Move bestMove);
	bool											 lookupTransposition(uint64_t hash, int depth, int alpha, int beta, int &score, Move &bestMove);
	void											 clearTranspositionTable();

	//=========================================================================
	// Helpers
	//=========================================================================

	int												 evaluate() const;
	bool											 isCancelled(std::stop_token token) const { return token.stop_requested(); }
	int												 getSearchDepth() const;

	//=========================================================================
	// Members
	//=========================================================================

	CPUConfiguration								 mConfig;
	GameEngine										&mEngine;

	// Search thread
	std::jthread									 mSearchThread;
	std::atomic<bool>								 mIsCalculating{false};

	// Transposition Table
	std::unordered_map<uint64_t, TranspositionEntry> mTranspositionTable;
	static constexpr size_t							 MAX_TRANSPOSITION_ENTRIES = 1'000'000;

	// Statistics
	int												 mNodesSearched			   = 0;
	int												 mTranspositionHits		   = 0;

	// Randomization
	std::random_device								 mRandomDevice;
	std::mt19937									 mRandomGenerator;
};

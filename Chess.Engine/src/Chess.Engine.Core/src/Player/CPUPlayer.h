/*
  ==============================================================================
	Module:			CPUPlayer
	Description:    Managing the CPU opponent player
  ==============================================================================
*/

#pragma once

#include <chrono>
#include <future>
#include <memory>
#include <random>
#include <thread>
#include <algorithm>

#include "Parameters.h"
#include "Generation/MoveGeneration.h"
#include "Evaluation/MoveEvaluation.h"
#include "ChessBoard.h"
#include "IObservable.h"
#include "LightChessBoard.h"
#include "PositionalEvaluation.h"


#define DEBUG_MOVES 0

/// <summary>
/// Enumerates the possible difficulty levels for a CPU opponent.
/// </summary>
enum class CPUDifficulty
{
	Random = 0,
	Easy   = 1,
	Medium = 2,
	Hard   = 3
};

/// <summary>
/// Represents an entry in a transposition table, storing information about a previously evaluated game state.
/// </summary>
struct TranspositionEntry
{
	uint64_t hash{};
	int		 depth{};
	int		 score{};
	enum class NodeType
	{
		Exact,
		Alpha,
		Beta
	} type;
	PossibleMove move{};
};

/// <summary>
/// Represents a possible move along with its associated score.
/// </summary>
struct MoveCandidate
{
	PossibleMove move;
	int			 score;
};

/// <summary>
/// Represents the configuration settings for a CPU-controlled player.
/// </summary>
struct CPUConfiguration
{
	CPUDifficulty			  difficulty = CPUDifficulty::Random;
	std::chrono::milliseconds thinkingTime{1000};
	bool					  enabled			  = false;
	PlayerColor				  cpuColor			  = PlayerColor::Black; // Default to black

	bool					  enableRandomization = true;				// Add some randomness to move selection
	float					  randomizationFactor = 0.1f;				// How much randomness? Between 0.0 and 1.0
	int						  candidateMoveCount  = 5;					// Number of top moves to consider
};


/// <summary>
/// Manages a computer-controlled chess player capable of generating and evaluating moves using various algorithms such as minimax and alpha-beta pruning.
/// Provides asynchronous search with cancellation via jthread stop tokens, positional + heuristic evaluation,
/// transposition table caching and optional randomization for variance.
/// </summary>
class CPUPlayer : public ICPUMoveObservable
{
public:
	CPUPlayer(std::shared_ptr<MoveGeneration> moveGeneration, std::shared_ptr<MoveEvaluation> moveEvaluation, std::shared_ptr<ChessBoard> board);
	~CPUPlayer();

	/**
	 * @brief Apply new CPU configuration (difficulty, timing, features).
	 */
	void			 setCPUConfiguration(const CPUConfiguration &config);

	/**
	 * @brief Current configuration snapshot.
	 */
	CPUConfiguration getCPUConfiguration() const { return mConfig; }

	/**
	 * @brief Begin asynchronous computation of a move for the configured CPU color.
	 * Launches a std::jthread; any existing search thread is cancelled.
	 */
	void			 requestMoveAsync();

	/**
	 * @brief True if specified player color belongs to this CPU instance.
	 */
	bool			 isCPUPlayer(PlayerColor player) const;

	/**
	 * @brief Query whether CPU participation is enabled.
	 */
	bool			 isCPUEnabled() const { return mConfig.enabled; }

	/**
	 * @brief Enable / disable CPU participation (does not cancel in-progress search).
	 */
	void			 setEnabled(bool enabled) { mConfig.enabled = enabled; }

	/**
	 * @brief Observer callback when a move has been computed (dispatch to listeners).
	 */
	void			 moveCalculated(PossibleMove calculatedMove) override;

	/**
	 * @brief Select a random move from provided list.
	 * @return Chosen move or default constructed if list empty.
	 */
	PossibleMove	 getRandomMove(const std::vector<PossibleMove> &moves);

	/**
	 * @brief Evaluate each move and return the one with highest score (static evaluation).
	 */
	PossibleMove	 getBestEvaluatedMove(const std::vector<PossibleMove> &moves);

	/**
	 * @brief Execute a minimax search to a fixed depth.
	 * @param moves Root move candidates.
	 * @param depth Search depth (plies).
	 * @param stopToken Cooperative cancellation token.
	 */
	PossibleMove	 getMiniMaxMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken = {});

	/**
	 * @brief Execute an alpha-beta pruned minimax search to depth.
	 */
	PossibleMove	 getAlphaBetaMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken = {});

	/**
	 * @brief Evaluate static positional score for a player on a lightweight board snapshot.
	 */
	int				 evaluatePlayerPosition(const LightChessBoard &board, PlayerColor player);


private:
	/**
	 * @brief High-level dispatcher choosing strategy based on difficulty / config.
	 * @return Best computed move (or fallback).
	 */
	PossibleMove computeBestMove(PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief Minimax recursive evaluation of a move branch.
	 * @param move Move leading into this node.
	 * @param board Mutable light board for fast make/unmake operations.
	 * @param depth Remaining depth (0 => leaf evaluation).
	 * @param maximizing True if maximizing player's turn.
	 * @param player Root CPU player color.
	 * @return Score relative to root player.
	 */
	int			 minimax(const PossibleMove &move, LightChessBoard &board, int depth, bool maximizing, PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief Alpha-beta pruned variant of minimax.
	 * @param alpha Best already found lower bound.
	 * @param beta  Best already found upper bound.
	 */
	int			 alphaBeta(const PossibleMove &move, LightChessBoard &board, int depth, int alpha, int beta, bool maximizing, PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief Quiescence search extension exploring only capture / tactical volatility
	 * to mitigate horizon effect.
	 */
	int			 quiescence(LightChessBoard &board, int alpha, int beta, PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief Choose highest scoring candidate (deterministic).
	 */
	PossibleMove selectBestMove(std::vector<MoveCandidate> &moves);

	/**
	 * @brief Choose a candidate with optional randomness (weighted / uniform)
	 * to avoid repetitive play patterns.
	 */
	PossibleMove selectMoveWithRandomization(std::vector<MoveCandidate> &moves);

	/**
	 * @brief Keep only top-N scoring candidates (pruning breadth).
	 */
	std::vector<MoveCandidate> filterTopCandidates(std::vector<MoveCandidate> &allMoves) const;

	/**
	 * @brief Insert (or replace) an entry in transposition table (size-limited).
	 */
	void					   storeTransposition(uint64_t hash, int depth, int score, TranspositionEntry::NodeType type, const PossibleMove &move);

	/**
	 * @brief Lookup transposition; returns true if usable entry found.
	 */
	bool					   lookupTransposition(uint64_t hash, int depth, int &score, PossibleMove &move);

	/**
	 * @brief Launch search on background jthread (handles cancellation of previous).
	 */
	void					   launchSearchAsync(PlayerColor player);

	/**
	 * @brief Convenience cancellation probe.
	 */
	inline bool				   cancelled(std::stop_token token) const { return token.stop_requested(); }

	/**
	 * @brief Generate a cache key combining board hash + move signature + player.
	 * Provides stable identity for evaluation memoization.
	 */
	inline uint64_t			   makeEvalKey(const PossibleMove &move, PlayerColor player, const LightChessBoard &board) const
	{
		uint64_t h = board.getHashKey();
		uint64_t m = (uint64_t(move.start.x & 7) << 48) | (uint64_t(move.start.y & 7) << 45) | (uint64_t(move.end.x & 7) << 42) | (uint64_t(move.end.y & 7) << 39) |
					 (uint64_t(move.type) << 16) | (uint64_t(move.promotionPiece) << 8) | uint64_t(player);
		// mix
		return h ^ (m + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
	}



	CPUConfiguration								 mConfig;

	std::shared_ptr<MoveGeneration>					 mMoveGeneration;
	std::shared_ptr<MoveEvaluation>					 mMoveEvaluation;
	std::shared_ptr<ChessBoard>						 mBoard;
	std::shared_ptr<PositionalEvaluation>			 mPositionalEvaluation;

	std::jthread									 mSearchThread;

	std::unordered_map<uint64_t, TranspositionEntry> mTranspositionTable;
	static constexpr size_t							 MAX_TRANSPOSITION_ENTRIES = 1000000;
	int												 mNodesSearched			   = 0;
	int												 mTranspositionHits		   = 0;

	std::random_device								 mRandomDevice;
	std::mt19937									 mRandomGenerator;

	mutable std::unordered_map<uint64_t, int>		 mEvaluationCache;
	static constexpr size_t							 MAX_EVAL_CACHE_SIZE = 1000000;
};

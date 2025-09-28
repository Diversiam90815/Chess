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



/**
 * @brief	Manages a computer-controlled chess player capable of generating and evaluating moves using various algorithms such as minimax and alpha-beta pruning.
 *			Provides asynchronous search with cancellation via jthread stop tokens, positional + heuristic evaluation,
 *			transposition table caching and optional randomization for variance.
 */
class CPUPlayer : public ICPUMoveObservable
{
public:
	CPUPlayer(std::shared_ptr<MoveGeneration> moveGeneration, std::shared_ptr<MoveEvaluation> moveEvaluation, std::shared_ptr<ChessBoard> board);
	~CPUPlayer();

	/**
	 * @brief	Apply new CPU configuration (difficulty, timing, features).
	 */
	void			 setCPUConfiguration(const CPUConfiguration &config);

	/**
	 * @brief	Current configuration snapshot.
	 */
	CPUConfiguration getCPUConfiguration() const { return mConfig; }

	/**
	 * @brief	Begin asynchronous computation of a move for the configured CPU color.
	 *			Launches a std::jthread; any existing search thread is cancelled.
	 */
	void			 requestMoveAsync();

	/**
	 * @brief	True if specified player color belongs to this CPU instance.
	 */
	bool			 isCPUPlayer(PlayerColor player) const;

	/**
	 * @brief	Query whether CPU participation is enabled.
	 */
	bool			 isCPUEnabled() const { return mConfig.enabled; }

	/**
	 * @brief	Enable / disable CPU participation (does not cancel in-progress search).
	 */
	void			 setEnabled(bool enabled) { mConfig.enabled = enabled; }

	/**
	 * @brief	Observer callback when a move has been computed (dispatch to listeners).
	 */
	void			 moveCalculated(PossibleMove calculatedMove) override;

	/**
	 * @brief	Select a random move from provided list.
	 * @param	moves -> all possible moves
	 * @return	Chosen move or default constructed / empty move if list empty.
	 */
	PossibleMove	 getRandomMove(const std::vector<PossibleMove> &moves);

	/**
	 * @brief	Evaluate each move and return the one with highest score (static evaluation).
	 *			Applies some randomization if enabled via the CPUConfig.
	 * @param	moves -> all possible moves
	 * @return	Chosen move or default constructed / empty move if list empty.
	 */
	PossibleMove	 getBestEvaluatedMove(const std::vector<PossibleMove> &moves);

	/**
	 * @brief	Execute a minimax search to a fixed depth.
	 * @param	moves -> Root move candidates.
	 * @param	depth -> Search depth (plies).
	 * @param	stopToken -> Cooperative cancellation token.
	 * @return	Chosen move or default constructed / empty move if list empty.
	 */
	PossibleMove	 getMiniMaxMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken = {});

	/**
	 * @brief	Execute a alpha-beta search to a fixed depth.
	 * @param	moves -> Root move candidates.
	 * @param	depth -> Search depth (plies).
	 * @param	stopToken -> Cooperative cancellation token.
	 * @return	Chosen move or default constructed / empty move if list empty.
	 */
	PossibleMove	 getAlphaBetaMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken = {});

	/**
	 * @brief	Evaluate static positional score for a player on a lightweight board snapshot.
	 */
	int				 evaluatePlayerPosition(const LightChessBoard &board, PlayerColor player);


private:
	/**
	 * @brief	High-level dispatcher choosing strategy based on difficulty / config.
	 * @param	player -> current player color
	 * @param	stopToken -> Cooperative cancellation token.
	 * @return	Best computed move (or fallback).
	 */
	PossibleMove computeBestMove(PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief	Minimax recursive evaluation of a move branch.
	 * @param	move -> Move leading into this node.
	 * @param	board -> Mutable light board for fast make/unmake operations.
	 * @param	depth -> Remaining depth (0 => leaf evaluation).
	 * @param	maximizing -> True if maximizing player's turn.
	 * @param	player	-> Root CPU player color.
	 * @param	stopToken -> Cooperative cancellation token.
	 * @return	Score relative to root player.
	 */
	int			 minimax(const PossibleMove &move, LightChessBoard &board, int depth, bool maximizing, PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief	Alpha-beta pruned variant of minimax.
	 * @param	move -> Move leading into this node.
	 * @param	board -> Mutable light board for fast make/unmake operations.
	 * @param	depth -> Remaining depth (0 => leaf evaluation).
	 * @param	alpha -> Best already found lower bound.
	 * @param	beta  -> Best already found upper bound.
	 * @param	maximizing -> True if maximizing player's turn.
	 * @param	player	-> Root CPU player color.
	 * @param	stopToken -> Cooperative cancellation token.
	 * @return	Score relative to root player.
	 */
	int			 alphaBeta(const PossibleMove &move, LightChessBoard &board, int depth, int alpha, int beta, bool maximizing, PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief	Quiescence search extension exploring only capture / tactical volatility
	 *			to mitigate horizon effect.
	 * @param	board -> Mutable light board for fast make/unmake operations.
	 * @param	alpha -> Best already found lower bound.
	 * @param	beta  -> Best already found upper bound.
	 * @param	player	-> Root CPU player color.
	 * @param	stopToken -> Cooperative cancellation token.
	 * @return	Score relative to root player.
	 */
	int			 quiescence(LightChessBoard &board, int alpha, int beta, PlayerColor player, std::stop_token stopToken = {});

	/**
	 * @brief	Choose highest scoring candidate (deterministic).
	 */
	PossibleMove selectBestMove(std::vector<MoveCandidate> &moves);

	/**
	 * @brief	Choose a candidate with optional randomness (weighted / uniform)
	 *			to avoid repetitive play patterns.
	 * @param	moves -> MoveCandidates to choose from. The moves will be sorted and filtered before selection.
	 * @return	Chosen move, or empty move on failure
	 */
	PossibleMove selectMoveWithRandomization(std::vector<MoveCandidate> &moves);

	/**
	 * @brief	Sorts the MoveCandidates depending on their score values and keeps only top-N scoring candidates (pruning breadth).
	 * @param	allMoves -> All moves available to be filtered
	 * @return	Top N moves according to their score values in a sorted order.
	 */
	std::vector<MoveCandidate> filterTopCandidates(std::vector<MoveCandidate> &allMoves) const;

	/**
	 * @brief	Insert (or replace) an entry in transposition table (size-limited). Entries are defined via the unique hash.
	 * @param	hash -> hash value of the current board state
	 * @param	depth -> current depth
	 * @param	score -> score value calculated
	 * @param	type -> TranspositonEntry NodeType
	 * @param	move -> best move to store
	 */
	void					   storeTransposition(uint64_t hash, int depth, int score, TranspositionEntry::NodeType type, const PossibleMove &move);

	/**
	 * @brief	Lookup a hash from the transposition table.
	 * @param	hash -> hash to look up from the transposition table
	 * @param	depth -> current depth
	 * @param	[OUT] score -> if the hash was found, the score value gets set to the cached value
	 * @param	[OUT] move -> if the hash was found, the move gets set to the cached move
	 * @return	True if the hash was found; false otherwise
	 */
	bool					   lookupTransposition(uint64_t hash, int depth, int &score, PossibleMove &move);

	/**
	 * @brief	Launch search on background jthread (handles cancellation of previous).
	 * @param	player -> launches to search a move for that player
	 */
	void					   launchSearchAsync(PlayerColor player);

	/**
	 * @brief	Checks if the alogithms were asked to terminate early. Since the move calculation runs in a detached thread, we have to terminate
	 *			the thread on eg. closure of the app.
	 */
	inline bool				   cancelled(std::stop_token token) const { return token.stop_requested(); }

	/**
	 * @brief	Generate a cache key combining board hash + move signature + player.
	 *			Provides stable identity for evaluation memoization.
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

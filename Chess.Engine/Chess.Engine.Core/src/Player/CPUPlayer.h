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


enum class CPUDifficulty
{
	Random = 0,
	Easy   = 1,
	Medium = 2,
	Hard   = 3
};

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

struct MoveCandidate
{
	PossibleMove move;
	int			 score;
};


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


class CPUPlayer : public ICPUMoveObservable
{
public:
	CPUPlayer(std::shared_ptr<MoveGeneration> moveGeneration, std::shared_ptr<MoveEvaluation> moveEvaluation, std::shared_ptr<ChessBoard> board);
	~CPUPlayer();

	void			 setCPUConfiguration(const CPUConfiguration &config);
	CPUConfiguration getCPUConfiguration() const { return mConfig; }

	void			 requestMoveAsync();

	bool			 isCPUPlayer(PlayerColor player) const;
	bool			 isCPUEnabled() const { return mConfig.enabled; }
	void			 setEnabled(bool enabled) { mConfig.enabled = enabled; }

	void			 moveCalculated(PossibleMove calculatedMove) override;

	PossibleMove	 getRandomMove(const std::vector<PossibleMove> &moves);

	PossibleMove	 getBestEvaluatedMove(const std::vector<PossibleMove> &moves);

	PossibleMove	 getMiniMaxMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken = {});
	PossibleMove	 getAlphaBetaMove(const std::vector<PossibleMove> &moves, int depth, std::stop_token stopToken = {});

	int				 evaluatePlayerPosition(const LightChessBoard &board, PlayerColor player);


private:
	PossibleMove computeBestMove(PlayerColor player, std::stop_token stopToken = {});

	int			 minimax(const PossibleMove &move, LightChessBoard &board, int depth, bool maximizing, PlayerColor player, std::stop_token stopToken = {});
	int			 alphaBeta(const PossibleMove &move, LightChessBoard &board, int depth, int alpha, int beta, bool maximizing, PlayerColor player, std::stop_token stopToken = {});
	int			 quiescence(LightChessBoard &board, int alpha, int beta, PlayerColor player, std::stop_token stopToken = {});

	PossibleMove selectBestMove(std::vector<MoveCandidate> &moves);

	PossibleMove selectMoveWithRandomization(std::vector<MoveCandidate> &moves);

	std::vector<MoveCandidate> filterTopCandidates(std::vector<MoveCandidate> &allMoves) const;

	void					   storeTransposition(uint64_t hash, int depth, int score, TranspositionEntry::NodeType type, const PossibleMove &move);
	bool					   lookupTransposition(uint64_t hash, int depth, int &score, PossibleMove &move);

	void					   launchSearchAsync(PlayerColor player);
	inline bool				   cancelled(std::stop_token token) const { return token.stop_requested(); }

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

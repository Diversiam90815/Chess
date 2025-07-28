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


enum class CPUDifficulty
{
	Random = 0,
	Easy   = 1,
	Medium = 2,
	Hard   = 3
};

struct TranspositionEntry
{
	uint64_t hash;
	int		 depth;
	int		 score;
	enum class NodeType
	{
		Exact,
		Alpha,
		Beta
	} type;
	PossibleMove move;
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
	~CPUPlayer() = default;

	void			 setCPUConfiguration(const CPUConfiguration &config);
	CPUConfiguration getCPUConfiguration() const { return mConfig; }

	void			 requestMoveAsync(PlayerColor player);

	bool			 isCPUPlayer(PlayerColor player) const;
	bool			 isCPUEnabled() const { return mConfig.enabled; }
	void			 setEnabled(bool enabled) { mConfig.enabled = enabled; }

	void			 moveCalculated(PossibleMove calculatedMove) override;

	PossibleMove	 getRandomMove(const std::vector<PossibleMove> &moves);
	PossibleMove	 getEasyMove(const std::vector<PossibleMove> &moves);
	PossibleMove	 getMediumMove(const std::vector<PossibleMove> &moves);
	PossibleMove	 getHardMove(const std::vector<PossibleMove> &moves);

	PossibleMove	 getMiniMaxMove(const std::vector<PossibleMove> &moves, int depth);
	PossibleMove	 getAlphaBetaMove(const std::vector<PossibleMove> &moves, int depth);

private:
	void											 calculateMove(PlayerColor player);

	void											 simulateThinking();

	int												 minimax(LightChessBoard &board, int depth, bool maximizing, PlayerColor player);
	int												 alphaBeta(LightChessBoard &board, int depth, int alpha, int beta, bool maximizing, PlayerColor player);

	PossibleMove									 selectBestMove(std::vector<MoveCandidate> &moves);

	PossibleMove									 selectMoveWithRandomization(std::vector<MoveCandidate> &moves);

	std::vector<MoveCandidate>						 filterTopCandidates(std::vector<MoveCandidate> &allMoves);

	void											 storeTransposition(uint64_t hash, int depth, int score, TranspositionEntry::NodeType type, const PossibleMove &move);
	bool											 lookupTransposition(uint64_t hash, int depth, int &score, PossibleMove &move);


	CPUConfiguration								 mConfig;

	std::shared_ptr<MoveGeneration>					 mMoveGeneration;
	std::shared_ptr<MoveEvaluation>					 mMoveEvaluation;
	std::shared_ptr<ChessBoard>						 mBoard;

	std::unordered_map<uint64_t, TranspositionEntry> mTranspositionTable;
	int												 mNodesSearched		= 0;
	int												 mTranspositionHits = 0;

	std::random_device								 mRandomDevice;
	std::mt19937									 mRandomGenerator;

    static constexpr size_t							 MAX_TRANSPOSITION_ENTRIES = 1000000;
};

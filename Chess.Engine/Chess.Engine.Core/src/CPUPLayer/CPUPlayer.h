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


enum class CPUDifficulty
{
	Random = 0,
	Easy   = 1,
	Medium = 2,
	Hard   = 3
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

private:
	void							calculateMove(PlayerColor player);

	void							simulateThinking();

	PossibleMove					selectBestMove(std::vector<MoveCandidate> &moves);

	PossibleMove					selectMoveWithRandomization(std::vector<MoveCandidate> &moves);

	std::vector<MoveCandidate>		filterTopCandidates(std::vector<MoveCandidate> &allMoves);


	CPUConfiguration				mConfig;

	std::shared_ptr<MoveGeneration> mMoveGeneration;
	std::shared_ptr<MoveEvaluation> mMoveEvaluation;
	std::shared_ptr<ChessBoard>		mBoard;

	std::random_device				mRandomDevice;
	std::mt19937					mRandomGenerator;
};

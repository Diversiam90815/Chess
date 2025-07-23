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


struct CPUConfiguration
{
	CPUDifficulty			  difficulty = CPUDifficulty::Random;
	std::chrono::milliseconds thinkingTime{1000};
	bool					  enabled  = false;
	PlayerColor				  cpuColor = PlayerColor::Black; // Default to black
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

private:
	PossibleMove					getRandomMove(const std::vector<PossibleMove> &moves);
	PossibleMove					getEasyMove(const std::vector<PossibleMove> &moves);
	PossibleMove					getMediumMove(const std::vector<PossibleMove> &moves);
	PossibleMove					getHardMove(const std::vector<PossibleMove> &moves);

	void							calculateMove(PlayerColor player);

	void							simulateThinking();


	CPUConfiguration				mConfig;

	std::shared_ptr<MoveGeneration> mMoveGeneration;
	std::shared_ptr<MoveEvaluation> mMoveEvaluation;
	std::shared_ptr<ChessBoard>		mBoard;

	std::random_device				mRandomDevice;
	std::mt19937					mRandomGenerator;
};

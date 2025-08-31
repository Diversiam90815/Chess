/*
  ==============================================================================
	Module:         Perf Result
	Description:    Performance result structs
  ==============================================================================
*/

#pragma once

#include <string>
#include <chrono>

#include "Project.h"
#include "Move.h"


struct ResultBase
{
	std::string							  testName{};
	std::chrono::system_clock::time_point timestamp;
	std::string							  version{ProjectInfo::Version};
	std::chrono::microseconds			  duration{};
};


struct MoveGenerationPerformanceResult : public ResultBase
{
	size_t		movesGenerated{};
	double		movesPerSecond{};
	size_t		positionsEvaluated{};
	std::string boardConfiguration{};
};


struct PositionalEvaluationPerformanceResult : public ResultBase
{
	std::string evaluationType{};
	size_t		evaluationsPerformed{};
	double		evaluationsPerSecond{};
	double		averageEvaluationTime{};
	std::string gamePhase{};
	int			minScore{};
	int			maxScore{};
	double		averageScore{};
	int			pieceCount{};
};


struct MoveEvaluationPerformanceResult : public ResultBase
{
	std::string evaluationType{};
	size_t		movesEvaluated{};
	double		evaluationsPerSecond{};
	double		averageEvaluationTime{};
	int			minScore{};
	int			maxScore{};
	double		averageScore{};
};


struct LightChessBoardPerformanceResult : public ResultBase
{
	std::string operation{};
	int			operationsPerformed{};
	double		operationsPerSecond{};
	double		averageOperationTime{};
	std::string boardConfiguration{};
	int			moveCount{};
	int			positionCount{};
};


struct CPUAlgorithmPerformanceResult : public ResultBase
{
	int			 depth;
	PossibleMove selectedMove;
	std::string	 position;
};

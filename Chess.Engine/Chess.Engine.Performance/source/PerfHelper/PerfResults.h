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


struct MoveGenerationPerformanceResult
{
	std::string							  testName{};
	std::chrono::microseconds			  duration{};
	size_t								  movesGenerated{};
	double								  movesPerSecond{};
	size_t								  positionsEvaluated{};
	std::string							  boardConfiguration{};

	std::chrono::system_clock::time_point timestamp;
	std::string							  version{ProjectInfo::Version};
};


struct PositionalEvaluationPerformanceResult
{
	std::string							  testName{};
	std::string							  evaluationType{};
	std::chrono::microseconds			  duration{};
	size_t								  evaluationsPerformed{};
	double								  evaluationsPerSecond{};
	double								  averageEvaluationTime{};
	std::string							  gamePhase{};
	int									  minScore{};
	int									  maxScore{};
	double								  averageScore{};
	int									  pieceCount{};

	std::chrono::system_clock::time_point timestamp;
	std::string							  version{ProjectInfo::Version};
};


struct MoveEvaluationPerformanceResult
{
	std::string							  testName{};
	std::string							  evaluationType{};
	std::chrono::microseconds			  duration{};
	size_t								  movesEvaluated{};
	double								  evaluationsPerSecond{};
	double								  averageEvaluationTime{};
	int									  minScore{};
	int									  maxScore{};
	double								  averageScore{};

	std::chrono::system_clock::time_point timestamp;
	std::string							  version{ProjectInfo::Version};
};


struct LightChessBoardPerformanceResult
{
	std::string							  testName{};
	std::string							  operation{};
	std::chrono::microseconds			  duration{};
	int									  operationsPerformed{};
	double								  operationsPerSecond{};
	double								  averageOperationTime{};
	std::string							  boardConfiguration{};
	int									  moveCount{};
	int									  positionCount{};

	std::chrono::system_clock::time_point timestamp;
	std::string							  version{ProjectInfo::Version};
};


struct CPUAlgorithmPerformanceResult
{
	std::string							  algorithmName{};
	int									  depth;
	std::chrono::milliseconds			  duration;
	PossibleMove						  selectedMove;
	std::string							  position;

	std::chrono::system_clock::time_point timeStamp;
	std::string							  version{ProjectInfo::Version};
};

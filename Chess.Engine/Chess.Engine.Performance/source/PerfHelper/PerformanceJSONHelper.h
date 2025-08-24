/*
  ==============================================================================
	Module:         Performance JSON Helper
	Description:    Helper functions for JSON output in performance tests
  ==============================================================================
*/

#pragma once

#include <nlohmann/json.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>

namespace fs = std::filesystem;
using json	 = nlohmann::json;


namespace PerformanceTests
{

class PerformanceJsonHelper
{
public:
	// Convert timestamp to ISO 8601 string
	static std::string timestampToIsoString(const std::chrono::system_clock::time_point &timestamp)
	{
		auto			   time_t = std::chrono::system_clock::to_time_t(timestamp);
		auto			   tm	  = *std::gmtime(&time_t); // Use UTC

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
		return oss.str();
	}

	// Create base JSON structure for performance results
	static json createBaseJson(const std::string &testGroup, const std::string &testFile)
	{
		json baseJson;
		baseJson["metadata"] = {
			{"timestamp", timestampToIsoString(std::chrono::system_clock::now())}, {"testGroup", testGroup}, {"testFile", testFile}, {"format", "chess-engine-performance-v1.0"}};
		baseJson["results"] = json::array();
		return baseJson;
	}

	// Save JSON to file with proper formatting
	template <typename ResultType>
	static void saveJsonResults(const std::string &fileName, const std::string &testGroup, const std::vector<ResultType> &results)
	{
		// Create directory if it doesn't exist
		fs::path resultDir = resultDirectory;

		if (!fs::exists(resultDir))
			fs::create_directories(resultDir);

		// Create unique filename with timestamp
		auto			   now	  = std::chrono::system_clock::now();
		auto			   time_t = std::chrono::system_clock::to_time_t(now);
		auto			   tm	  = *std::localtime(&time_t);

		std::ostringstream filename;
		filename << fileName << "_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".json";

		fs::path fullPath	= resultDir / filename.str();

		// Create JSON structure
		json	 jsonOutput = createBaseJson(testGroup, fileName);

		// Add results
		for (const auto &result : results)
		{
			jsonOutput["results"].push_back(resultToJson(result));
		}

		// Write to file with proper formatting
		std::ofstream file(fullPath);
		if (file.is_open())
		{
			file << jsonOutput.dump(2); // print with 2 space indentation
			file.close();
		}
	}

	// Convert Result to JSON
	template <typename T>
	static json			  resultToJson(const T &result);


	static constexpr auto resultDirectory = "Performance_Results";
};


// Template specializations for different result types
template <>
json PerformanceJsonHelper::resultToJson(const struct PositionalEvaluationPerformanceResult &result)
{
	return json{{"testName", result.testName},
				{"evaluationType", result.evaluationType},
				{"gamePhase", result.gamePhase},
				{"performance",
				 {{"durationMicroseconds", result.duration.count()},
				  {"evaluationsPerformed", result.evaluationsPerformed},
				  {"evaluationsPerSecond", result.evaluationsPerSecond},
				  {"averageEvaluationTimeMicroseconds", result.averageEvaluationTime}}},
				{"scores", {{"minimum", result.minScore}, {"maximum", result.maxScore}, {"average", result.averageScore}}},
				{"boardState", {{"pieceCount", result.pieceCount}}},
				{"testMetadata", {{"timestamp", timestampToIsoString(result.timestamp)}, {"version", result.version}}}};
}

template <>
json PerformanceJsonHelper::resultToJson(const struct LightChessBoardPerformanceResult &result)
{
	return json{{"testName", result.testName},
				{"operation", result.operation},
				{"boardConfiguration", result.boardConfiguration},
				{"performance",
				 {{"durationMicroseconds", result.duration.count()},
				  {"operationsPerformed", result.operationsPerformed},
				  {"operationsPerSecond", result.operationsPerSecond},
				  {"averageOperationTimeMicroseconds", result.averageOperationTime}}},
				{"boardState", {{"moveCount", result.moveCount}, {"positionCount", result.positionCount}}},
				{"testMetadata", {{"timestamp", timestampToIsoString(result.timestamp)}, {"version", result.version}}}};
}

template <>
json PerformanceJsonHelper::resultToJson(const struct MoveEvaluationPerformanceResult &result)
{
	return json{{"testName", result.testName},
				{"evaluationType", result.evaluationType},
				{"performance",
				 {{"durationMicroseconds", result.duration.count()},
				  {"movesEvaluated", result.movesEvaluated},
				  {"evaluationsPerSecond", result.evaluationsPerSecond},
				  {"averageEvaluationTimeMicroseconds", result.averageEvaluationTime}}},
				{"scores", {{"minimum", result.minScore}, {"maximum", result.maxScore}, {"average", result.averageScore}}},
				{"testMetadata", {{"timestamp", timestampToIsoString(result.timestamp)}, {"version", result.version}}}};
}

} // namespace PerformanceTests
/*
  ==============================================================================
	Module:         CPU Performance Tests
	Description:    Testing performance through CPU vs CPU games with different configurations
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>

#include "GameEngine/GameEngine.h"

namespace fs = std::filesystem;

namespace PerformanceTests
{

struct GameResult
{
	PlayerColor				  winner	= PlayerColor::NoColor;
	EndGameState			  endState	= EndGameState::OnGoing;
	int						  moveCount = 0;
	std::chrono::milliseconds duration{0};
	std::string				  gameDescription;
};

struct CPUPerformanceStats
{
	int						  wins		 = 0;
	int						  losses	 = 0;
	int						  draws		 = 0;
	int						  totalGames = 0;
	double					  winRate	 = 0.0;
	std::chrono::milliseconds averageGameDuration{0};
	int						  averageMoveCount = 0;

	void					  calculateStats()
	{
		if (totalGames > 0)
		{
			winRate = static_cast<double>(wins) / totalGames * 100;
		}
	}
};


class CPUPerformanceTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>			  mBoard;
	std::shared_ptr<MoveValidation>		  mValidation;
	std::shared_ptr<MoveExecution>		  mExecution;
	std::shared_ptr<MoveGeneration>		  mGeneration;
	std::shared_ptr<MoveEvaluation>		  mEvaluation;

	std::shared_ptr<CPUPlayer>			  mWhiteCPU;
	std::shared_ptr<CPUPlayer>			  mBlackCPU;

	std::atomic<bool>					  mGameCompleted{false};
	std::atomic<bool>					  mGameRunning{false};
	GameResult							  mCurrentGameResult;
	std::chrono::steady_clock::time_point mGameStartTime;


	void								  SetUp() override
	{
		// Initialize core components
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();

		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
		mEvaluation = std::make_shared<MoveEvaluation>(mBoard, mGeneration);

		// Create CPU players
		mWhiteCPU	= std::make_shared<CPUPlayer>(mGeneration, mEvaluation, mBoard);
		mBlackCPU	= std::make_shared<CPUPlayer>(mGeneration, mEvaluation, mBoard);
	}


	GameResult runSingleGame(const CPUConfiguration &whiteCPU,
							 const CPUConfiguration &blackCPU,
							 const std::string		&description = "",
							 int					 maxMoves	 = 200,
							 std::chrono::seconds	 timeout	 = std::chrono::seconds(300))
	{
		GameResult result;
		result.gameDescription = description;

		auto gameStartTime	   = std::chrono::steady_clock::now();

		// Reset the board
		mBoard->removeAllPiecesFromBoard();
		mBoard->initializeBoard();

		// Configure CPU Players
		mWhiteCPU->setCPUConfiguration(whiteCPU);
		mBlackCPU->setCPUConfiguration(blackCPU);

		// Game Loop
		PlayerColor currentPlayer = PlayerColor::White;
		auto		startTime	  = std::chrono::steady_clock::now();

		while (std::chrono::steady_clock::now() - startTime < timeout && result.moveCount < maxMoves)
		{
			// Generate moves for current player
			bool hasValidMove = mGeneration->calculateAllLegalBasicMoves(currentPlayer);
			if (!hasValidMove)
			{
				result.endState = EndGameState::StaleMate;
				break;
			}

			// Get CPU move
			auto &currentCPU = (currentPlayer == PlayerColor::White) ? mWhiteCPU : mBlackCPU;
			auto  move		 = selectCPUMove(currentCPU, currentPlayer);

			if (move.isEmpty())
			{
				result.endState = EndGameState::StaleMate;
				break;
			}

			// Execute move
			auto executedMove = mExecution->executeMove(move);
			result.moveCount++;

			// Check for checkmate
			if ((executedMove.type & MoveType::Checkmate) == MoveType::Checkmate)
			{
				result.endState = EndGameState::Checkmate;
				result.winner	= currentPlayer;
				break;
			}

			// switch players
			currentPlayer = (currentPlayer == PlayerColor::White) ? PlayerColor::Black : PlayerColor::White;
		}

		if (result.endState == EndGameState::OnGoing)
		{
			// Get timed out or move limit reached
			result.endState = EndGameState::StaleMate;
		}

		// Calculate game duration
		result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - gameStartTime);

		return result;
	}


	CPUPerformanceStats runMultipleGames(const CPUConfiguration &testCPU, const CPUConfiguration &referenceCPU, int gameCount, bool testCPUAsWhite = true)
	{
		CPUPerformanceStats		stats;
		std::vector<GameResult> games;

		for (int i = 0; i < gameCount; ++i)
		{
			// Setup CPUConfigurations for white and black

			CPUConfiguration whiteCPU = testCPUAsWhite ? testCPU : referenceCPU;
			CPUConfiguration blackCPU = testCPUAsWhite ? referenceCPU : testCPU;

			whiteCPU.cpuColor		  = PlayerColor::White;
			blackCPU.cpuColor		  = PlayerColor::Black;

			std::string description	  = "Game " + std::to_string(i + 1) + "/" + std::to_string(gameCount);

			// Run the game
			GameResult	result		  = runSingleGame(whiteCPU, blackCPU, description);

			games.push_back(result);

			// Update stats
			stats.totalGames++;

			if (result.winner == (testCPUAsWhite ? PlayerColor::White : PlayerColor::Black))
				stats.wins++;

			else if (result.winner == (testCPUAsWhite ? PlayerColor::Black : PlayerColor::White))
				stats.losses++;

			else
				stats.draws++;
		}

		// Calculate final stat
		stats.calculateStats();

		std::chrono::milliseconds totalDuration{0};
		int						  totalMoves = 0;
		for (const auto &game : games)
		{
			totalDuration += game.duration;
			totalMoves += game.moveCount;
		}

		if (stats.totalGames > 0)
		{
			stats.averageGameDuration = totalDuration / stats.totalGames;
			stats.averageMoveCount	  = totalMoves / stats.totalGames;
		}

		return stats;
	}


	void saveResultsToFile(const std::string &fileName, const CPUPerformanceStats &stats, const std::string &testDescription)
	{
		// Create directory if not exists yet
		fs::path resultDir = "CPU_VS_CPU_Results";

		if (!fs::exists(resultDir))
			fs::create_directories(resultDir);

		fs::path	  fullPath = resultDir / fileName;

		std::ofstream file(fullPath, std::ios::app);

		if (!file.is_open())
			return;

		file << "=== " << testDescription << " ===" << std::endl;
		file << "Total Games: " << stats.totalGames << std::endl;
		file << "Wins: " << stats.wins << std::endl;
		file << "Losses: " << stats.losses << std::endl;
		file << "Draws: " << stats.draws << std::endl;
		file << "Win Rate: " << std::fixed << std::setprecision(2) << stats.winRate << "%" << std::endl;
		file << "Average Game Duration: " << stats.averageGameDuration.count() << "ms" << std::endl;
		file << "Average Move Count: " << stats.averageMoveCount << std::endl;
		file << std::string(50, '-') << std::endl; // Separator line
		file << std::endl;

		file.close();
	}

private:
	PossibleMove selectCPUMove(std::shared_ptr<CPUPlayer> cpu, PlayerColor player)
	{
		auto allMoves = getAllLegalMovesForPlayer(player);
		if (allMoves.empty())
			return PossibleMove();

		// Select move based on CPU Difficulty
		auto config = cpu->getCPUConfiguration();

		switch (config.difficulty)
		{
		case CPUDifficulty::Random: return cpu->getRandomMove(allMoves);
		case CPUDifficulty::Easy: return (allMoves.size() > 20) ? cpu->getBestEvaluatedMove(allMoves) : cpu->getMiniMaxMove(allMoves, 3);
		case CPUDifficulty::Medium: return cpu->getAlphaBetaMove(allMoves, 3);
		case CPUDifficulty::Hard: return cpu->getAlphaBetaMove(allMoves, 6);
		default: return {};
		}
	}


	std::vector<PossibleMove> getAllLegalMovesForPlayer(PlayerColor player)
	{
		std::vector<PossibleMove> allMoves;

		// Get all pieces for the player and collect their possible moves
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			for (int x = 0; x < BOARD_SIZE; ++x)
			{
				Position pos{x, y};
				auto	&piece = mBoard->getPiece(pos);

				if (piece && piece->getColor() == player)
				{
					auto moves = mGeneration->getMovesForPosition(pos);
					allMoves.insert(allMoves.end(), moves.begin(), moves.end());
				}
			}
		}

		return allMoves;
	}
};


TEST_F(CPUPerformanceTests, Medium_VS_Easy)
{
	CPUConfiguration easyConfig;
	easyConfig.difficulty	= CPUDifficulty::Easy;
	easyConfig.enabled		= true;
	easyConfig.thinkingTime = std::chrono::milliseconds(100);

	CPUConfiguration mediumConfig;
	mediumConfig.difficulty	  = CPUDifficulty::Medium;
	mediumConfig.enabled	  = true;
	mediumConfig.thinkingTime = std::chrono::milliseconds(100);

	// Test medium vs easy
	auto stats				  = runMultipleGames(mediumConfig, easyConfig, 10, true); // Reducing the move counts since we currently focus on the other tests

	std::cout << "Medium vs Easy Results:" << std::endl;
	std::cout << "Win Rate: " << stats.winRate << "%" << std::endl;
	std::cout << "Total Games: " << stats.totalGames << std::endl;

	saveResultsToFile("result_medium_vs_easy.txt", stats, "Medium vs Easy CPU Configuration Performance Tests");

	// The results of this test are saved in the file
	SUCCEED();
}



} // namespace PerformanceTests
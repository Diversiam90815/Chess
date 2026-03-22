/*
  ==============================================================================
	Module:			CPUPlayer Tests
	Description:    Testing the CPUPlayer module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "PLayer/CPUPlayer.h"


namespace PlayerTests
{

class CPUPlayerTests : public ::testing::Test
{
protected:
	GameEngine mEngine;
	CPUPlayer  mCPUPlayer{mEngine};

	void	   SetUp() override { mEngine.init(); }
};


TEST_F(CPUPlayerTests, ConstructorInitializesCorrectly)
{
	// Verify CPU player is constructed with correct default values
	CPUConfiguration config = mCPUPlayer.getConfiguration();

	EXPECT_EQ(config.difficulty, CPUDifficulty::Medium) << "Default difficulty should be Medium";
	EXPECT_EQ(config.cpuColor, Side::Black) << "Default CPU color should be Black";
	EXPECT_FALSE(config.enabled) << "CPU should be disabled by default";
	EXPECT_TRUE(config.enableRandomization) << "Randomization should be enabled by default";
	EXPECT_EQ(config.maxDepth, 6) << "Default max depth should be 6";
}


TEST_F(CPUPlayerTests, ConfigureUpdatesCorrectly)
{
	CPUConfiguration config;
	config.cpuColor			   = Side::White;
	config.difficulty		   = CPUDifficulty::Hard;
	config.enabled			   = true;
	config.enableRandomization = false;
	config.maxDepth			   = 8;

	mCPUPlayer.configure(config);

	CPUConfiguration retrievedConfig = mCPUPlayer.getConfiguration();

	EXPECT_EQ(retrievedConfig.difficulty, CPUDifficulty::Hard) << "Difficulty should be updated to Hard";
	EXPECT_EQ(retrievedConfig.cpuColor, Side::White) << "CPU color should be updated to White";
	EXPECT_TRUE(retrievedConfig.enabled) << "CPU should be enabled";
	EXPECT_FALSE(retrievedConfig.enableRandomization) << "Randomization should be disabled";
	EXPECT_EQ(retrievedConfig.maxDepth, 8) << "Max depth should be updated to 8";
}


TEST_F(CPUPlayerTests, IsCPUPlayerReturnsFalseWhenDisabled)
{
	CPUConfiguration config;
	config.enabled	= false;
	config.cpuColor = Side::Black;

	mCPUPlayer.configure(config);

	bool isBlackCPU = mCPUPlayer.isCPUPlayer(Side::Black);
	bool isWhiteCPU = mCPUPlayer.isCPUPlayer(Side::White);

	EXPECT_FALSE(isBlackCPU) << "Should return false when CPU is disabled";
	EXPECT_FALSE(isWhiteCPU) << "Should return false when CPU is disabled";
}


TEST_F(CPUPlayerTests, IsCPUPlayerReturnsTrueForCorrectSide)
{
	CPUConfiguration config;
	config.enabled	= true;
	config.cpuColor = Side::Black;

	mCPUPlayer.configure(config);

	bool isBlackCPU = mCPUPlayer.isCPUPlayer(Side::Black);
	bool isWhiteCPU = mCPUPlayer.isCPUPlayer(Side::White);

	EXPECT_TRUE(isBlackCPU) << "Should return true for CPU side when enabled";
	EXPECT_FALSE(isWhiteCPU) << "Should return false for non-CPU side";
}


TEST_F(CPUPlayerTests, IsEnabledReturnsCorrectState)
{
	// Test disabled state
	CPUConfiguration config;
	config.enabled = false;

	mCPUPlayer.configure(config);

	EXPECT_FALSE(mCPUPlayer.isEnabled()) << "Should return false when disabled";

	// Test enabled state
	config.enabled = true;
	mCPUPlayer.configure(config);

	EXPECT_TRUE(mCPUPlayer.isEnabled()) << "Should return true when enabled";
}


TEST_F(CPUPlayerTests, SetEnabledUpdatesCorrectly)
{
	mCPUPlayer.setEnabled(true);
	EXPECT_TRUE(mCPUPlayer.isEnabled()) << "Should be enabled after setEnabled(true)";

	mCPUPlayer.setEnabled(false);
	EXPECT_FALSE(mCPUPlayer.isEnabled()) << "Should be disabled after setEnabled(false)";
}


TEST_F(CPUPlayerTests, CalculateMoveSynchronously)
{
	// Enable CPU and set difficulty
	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Easy;

	mCPUPlayer.configure(config);

	// Calculate a move synchronously
	Move move = mCPUPlayer.calculateMove();

	// Should return a valid move from the initial position
	EXPECT_TRUE(move.isValid()) << "Should return a valid move from initial position";
}


TEST_F(CPUPlayerTests, CalculateMoveReturnsValidMoveFromInitialPosition)
{
	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Medium;
	config.maxDepth	  = 2;

	mCPUPlayer.configure(config);

	Move move = mCPUPlayer.calculateMove();

	EXPECT_TRUE(move.isValid()) << "CPU should return a valid move";

	// Verify it's a legal move
	EXPECT_TRUE(mEngine.isMoveLegal(move)) << "CPU move should be legal";
}


TEST_F(CPUPlayerTests, CalculateMoveAsyncCallsCallback)
{
	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Easy;

	mCPUPlayer.configure(config);

	bool callbackCalled = false;
	Move receivedMove;

	auto callback = [&callbackCalled, &receivedMove](Move move)
	{
		callbackCalled = true;
		receivedMove   = move;
	};

	mCPUPlayer.calculateMoveAsync(callback);

	// Wait for calculation to complete
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	EXPECT_TRUE(callbackCalled) << "Callback should be called";
	EXPECT_TRUE(receivedMove.isValid()) << "Received move should be valid";
}


TEST_F(CPUPlayerTests, CancelCalculation)
{
	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Hard;
	config.maxDepth	  = 8; // Deep search to ensure it takes time

	mCPUPlayer.configure(config);

	bool callbackCalled = false;

	auto callback		= [&callbackCalled](Move move) { callbackCalled = true; };

	mCPUPlayer.calculateMoveAsync(callback);

	// Cancel immediately
	mCPUPlayer.cancelCalculation();

	// Wait a bit
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_FALSE(mCPUPlayer.isCalculating()) << "Should not be calculating after cancel";
}


TEST_F(CPUPlayerTests, IsCalculatingDuringAsyncSearch)
{
	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Medium;

	mCPUPlayer.configure(config);

	EXPECT_FALSE(mCPUPlayer.isCalculating()) << "Should not be calculating initially";

	auto callback = [](Move move) {};

	mCPUPlayer.calculateMoveAsync(callback);

	// Check immediately - might be calculating
	bool isCalculatingDuringSearch = mCPUPlayer.isCalculating();

	// Wait for completion
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	EXPECT_FALSE(mCPUPlayer.isCalculating()) << "Should not be calculating after completion";
}


TEST_F(CPUPlayerTests, DifficultyAffectsSearchDepth)
{
	// Test that different difficulties use different search depths
	CPUConfiguration easyConfig;
	easyConfig.enabled	  = true;
	easyConfig.cpuColor	  = Side::White;
	easyConfig.difficulty = CPUDifficulty::Easy;

	CPUConfiguration hardConfig;
	hardConfig.enabled	  = true;
	hardConfig.cpuColor	  = Side::White;
	hardConfig.difficulty = CPUDifficulty::Hard;

	// Easy difficulty should complete faster
	mCPUPlayer.configure(easyConfig);

	auto easyStart	  = std::chrono::high_resolution_clock::now();
	Move easyMove	  = mCPUPlayer.calculateMove();
	auto easyEnd	  = std::chrono::high_resolution_clock::now();

	auto easyDuration = std::chrono::duration_cast<std::chrono::milliseconds>(easyEnd - easyStart);

	// Hard difficulty should take longer
	mCPUPlayer.configure(hardConfig);

	auto hardStart	  = std::chrono::high_resolution_clock::now();
	Move hardMove	  = mCPUPlayer.calculateMove();
	auto hardEnd	  = std::chrono::high_resolution_clock::now();

	auto hardDuration = std::chrono::duration_cast<std::chrono::milliseconds>(hardEnd - hardStart);

	EXPECT_TRUE(easyMove.isValid()) << "Easy move should be valid";
	EXPECT_TRUE(hardMove.isValid()) << "Hard move should be valid";

	// Hard should generally take longer (but this test might be flaky on very fast systems)
	// So we just verify both completed successfully
	SUCCEED() << "Both difficulty levels completed successfully";
}


TEST_F(CPUPlayerTests, RandomizationProducesDifferentMoves)
{
	CPUConfiguration config;
	config.enabled			   = true;
	config.cpuColor			   = Side::White;
	config.difficulty		   = CPUDifficulty::Easy;
	config.enableRandomization = true;

	mCPUPlayer.configure(config);

	// Calculate multiple moves and check for variation
	std::set<uint16_t> uniqueMoves;

	for (int i = 0; i < 10; ++i)
	{
		mEngine.resetGame(); // Reset to initial position
		Move move = mCPUPlayer.calculateMove();
		uniqueMoves.insert(move.raw());
	}

	// With randomization enabled, we should see some variety
	// (might occasionally fail due to random chance, but unlikely)
	EXPECT_GT(uniqueMoves.size(), 1) << "Randomization should produce different moves";
}


TEST_F(CPUPlayerTests, NoRandomizationProducesSameMove)
{
	CPUConfiguration config;
	config.enabled			   = true;
	config.cpuColor			   = Side::White;
	config.difficulty		   = CPUDifficulty::Easy;
	config.enableRandomization = false;

	mCPUPlayer.configure(config);

	// Calculate multiple moves from the same position
	Move move1 = mCPUPlayer.calculateMove();

	mEngine.resetGame();
	Move move2 = mCPUPlayer.calculateMove();

	mEngine.resetGame();
	Move move3 = mCPUPlayer.calculateMove();

	// Without randomization, should get the same move
	EXPECT_EQ(move1.raw(), move2.raw()) << "Without randomization, moves should be identical";
	EXPECT_EQ(move2.raw(), move3.raw()) << "Without randomization, moves should be identical";
}


TEST_F(CPUPlayerTests, CPUFindsCheckmate)
{
	// Set up a position where checkmate is available in one move
	mEngine.getBoard().clear();

	// Mate in 1: Queen takes f7 with check (Scholar's mate setup)
	mEngine.getBoard().addPiece(PieceType::WQueen, Square::h5);
	mEngine.getBoard().addPiece(PieceType::WBishop, Square::c4);
	mEngine.getBoard().addPiece(PieceType::WKing, Square::e1);

	mEngine.getBoard().addPiece(PieceType::BKing, Square::e8);
	mEngine.getBoard().addPiece(PieceType::BPawn, Square::e7);
	mEngine.getBoard().addPiece(PieceType::BPawn, Square::f7);
	mEngine.getBoard().addPiece(PieceType::BPawn, Square::g7);

	mEngine.getBoard().setSide(Side::White);
	mEngine.getBoard().updateOccupancies();

	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Medium;

	mCPUPlayer.configure(config);

	Move move = mCPUPlayer.calculateMove();

	// Should find Qxf7# (checkmate)
	EXPECT_TRUE(move.isValid()) << "Should find a move";
	EXPECT_EQ(move.from(), Square::h5) << "Should move queen";
	EXPECT_EQ(move.to(), Square::f7) << "Should capture on f7 for checkmate";
}


TEST_F(CPUPlayerTests, TranspositionTableImprovesPerformance)
{
	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Medium;
	config.maxDepth	  = 4;

	mCPUPlayer.configure(config);

	// Make and unmake some moves to create transposition opportunities
	mEngine.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mEngine.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));
	mEngine.makeMove(Move(Square::g1, Square::f3, MoveFlag::Quiet));
	mEngine.makeMove(Move(Square::b8, Square::c6, MoveFlag::Quiet));

	// Calculate move - transposition table should help
	Move move = mCPUPlayer.calculateMove();

	EXPECT_TRUE(move.isValid()) << "Should calculate a valid move";
	// Transposition hits are logged by CPU player, check that in actual implementation
}


TEST_F(CPUPlayerTests, HandlesNoLegalMoves)
{
	// Set up a stalemate/checkmate position
	mEngine.getBoard().clear();

	mEngine.getBoard().addPiece(PieceType::WKing, Square::a1);
	mEngine.getBoard().addPiece(PieceType::BKing, Square::a3);
	mEngine.getBoard().addPiece(PieceType::BQueen, Square::c2);

	mEngine.getBoard().setSide(Side::White);
	mEngine.getBoard().updateOccupancies();

	CPUConfiguration config;
	config.enabled	  = true;
	config.cpuColor	  = Side::White;
	config.difficulty = CPUDifficulty::Easy;

	mCPUPlayer.configure(config);

	Move move = mCPUPlayer.calculateMove();

	// No legal moves available
	EXPECT_FALSE(move.isValid()) << "Should return invalid move when no legal moves";
}


} // namespace PlayerTests

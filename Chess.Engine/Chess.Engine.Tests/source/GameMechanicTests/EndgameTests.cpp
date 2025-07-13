/*
  ==============================================================================
	Module:			Endgame Tests
	Description:    Testing the endgame conditions and winner determination
  ==============================================================================
*/


#include <gtest/gtest.h>

#include "GameManager.h"
#include "StateMachine.h"
#include "MoveExecution.h"
#include "MoveValidation.h"
#include "MoveGeneration.h"


namespace GameMechanicTests
{

class EndgameTests : public ::testing::Test
{
protected:
	GameManager					   *mGameManager;
	std::shared_ptr<StateMachine>	mStateMachine;
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;


	void							SetUp() override
	{
		mGameManager = GameManager::GetInstance();
		mGameManager->init();

		mStateMachine = StateMachine::GetInstance();

		// Get access to internal components for setup
		mBoard		  = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
	}


	void TearDown() override
	{
		mGameManager->resetGame();
		mStateMachine->ReleaseInstance();
		GameManager::ReleaseInstance();
	}


	// Helper method to setup a checkmate position
	void SetupCheckmatePosition()
	{
		mBoard->removeAllPiecesFromBoard();

		// Setup Scholar's mate position
		Position blackKingPos = {4, 0}; // e8

		// Black pieces
		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->setPiece({3, 0}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));  // d8
		mBoard->setPiece({5, 0}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::Black)); // f8
		mBoard->setPiece({3, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));	  // d7
		mBoard->setPiece({4, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));	  // e7
		mBoard->setPiece({6, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));	  // g7

		// White pieces - queen delivers checkmate at f7
		mBoard->setPiece({5, 1}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));  // f7
		mBoard->setPiece({2, 4}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White)); // c4 (supports queen)

		mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);
	}


	// Helper method to setup a stalemate position
	void SetupStalematePosition()
	{
		mBoard->removeAllPiecesFromBoard();

		// Setup simple stalemate position
		Position blackKingPos  = {7, 0}; // h8
		Position whiteQueenPos = {6, 2}; // g6

		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->setPiece(whiteQueenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
		mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);
	}


	// Helper method to simulate a checkmate move
	void SimulateCheckmateMove()
	{
		// Create a move that results in checkmate
		PossibleMove checkmateMove;
		checkmateMove.start = {5, 2}; // f6
		checkmateMove.end	= {5, 1}; // f7
		checkmateMove.type	= MoveType::Checkmate | MoveType::Normal;

		Move executedMove(checkmateMove);
		executedMove.player		= PlayerColor::White;
		executedMove.type		= MoveType::Checkmate | MoveType::Normal;
		executedMove.movedPiece = PieceType::Queen;

		// Add the move to history to simulate it was executed
		mExecution->addMoveToHistory(executedMove);
	}
};


TEST_F(EndgameTests, GetWinnerReturnsNulloptWhenNoMoves)
{
	auto winner = mGameManager->getWinner();

	EXPECT_FALSE(winner.has_value()) << "getWinner should return nullopt when no moves have been made";
}


TEST_F(EndgameTests, GetWinnerReturnsNulloptForNonCheckmateMoves)
{
	// Set up a normal move (no checkmate) from e2 -> e4
	PossibleMove normalMove;
	normalMove.start = {4, 6};
	normalMove.end	 = {
		  4,
		  4,
	  };
	normalMove.type = MoveType::Normal;

	Move executedMove(normalMove);
	executedMove.player		= PlayerColor::White;
	executedMove.type		= MoveType::Normal;
	executedMove.movedPiece = PieceType::Pawn;

	mExecution->addMoveToHistory(executedMove);

	// Get the winner
	auto winner = mGameManager->getWinner();

	EXPECT_FALSE(winner.has_value()) << "getWinner should return nullopt for non-checkmate moves";
}


TEST_F(EndgameTests, GetWinnerReturnsCorrectPlayerForWhiteCheckmate)
{
	// Simulate a checkmate move by white
	PossibleMove checkmateMove;
	checkmateMove.start = {5, 2}; // f6
	checkmateMove.end	= {5, 1}; // f7
	checkmateMove.type	= MoveType::Normal | MoveType::Checkmate;

	Move executedMove(checkmateMove);
	executedMove.player		= PlayerColor::White;
	executedMove.type		= MoveType::Checkmate | MoveType::Normal;
	executedMove.movedPiece = PieceType::Queen;

	mExecution->addMoveToHistory(executedMove);

	auto winner = mGameManager->getWinner();

	ASSERT_TRUE(winner.has_value()) << "getWinner should return a player for checkmate moves";
	EXPECT_EQ(winner.value(), PlayerColor::White) << "Whites should be the winner after delivering checkmate";
}


TEST_F(EndgameTests, GetWinnerReturnsCoeectPlayerForBlackCheckmate)
{
	// Simulate a checkmate move by black
	PossibleMove checkmateMove;
	checkmateMove.start = {3, 1}; // d7
	checkmateMove.end	= {3, 2}; // d6
	checkmateMove.type	= MoveType::Checkmate | MoveType::Normal;

	Move executedMove(checkmateMove);
	executedMove.player		= PlayerColor::Black;
	executedMove.type		= MoveType::Checkmate | MoveType::Normal;
	executedMove.movedPiece = PieceType::Queen;

	mExecution->addMoveToHistory(executedMove);

	auto winner = mGameManager->getWinner();

	ASSERT_TRUE(winner.has_value()) << "getWinner should return a player for checkmate moves";
	EXPECT_EQ(winner.value(), PlayerColor::Black) << "Black should be the winner after delivering checkmate";
}






} // namespace GameMechanicTests
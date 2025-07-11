/*
  ==============================================================================
	Module:			Pawn Promotion Tests
	Description:    Testing pawn promotion functionality
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveExecution.h"
#include "MoveValidation.h"
#include "MoveGeneration.h"
#include "StateMachine.h"


namespace GameMechanicTests
{


class PawnPromotionTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;

	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->removeAllPiecesFromBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
	}

	void SetupWhitePawnPromotionPosition()
	{
		// Place a white pawn at a7 (one square away from promotion)
		Position pawnPos = {0, 1}; // a7
		mBoard->setPiece(pawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));

		// Place kings to avoid validation issues
		Position whiteKingPos = {4, 7}; // e1
		Position blackKingPos = {4, 0}; // e8
		mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);
		mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);
	}

	void SetupBlackPawnPromotionPosition()
	{
		// Place a black pawn at a2 (one square away from promotion)
		Position pawnPos = {0, 6}; // a2
		mBoard->setPiece(pawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

		// Place kings to avoid validation issues
		Position whiteKingPos = {4, 7}; // e1
		Position blackKingPos = {4, 0}; // e8
		mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);
		mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);
	}
};


TEST_F(PawnPromotionTests, ExecutePawnPromotionToQueen)
{
	SetupWhitePawnPromotionPosition();

	// Setup pawn promotion move : white pawn a7->a8
	Position	 start = {0, 1}; // a7
	Position	 end   = {0, 0}; // a8
	PossibleMove promotionMove{start, end, MoveType::PawnPromotion, PieceType::Queen};

	// Execute promotion
	bool		 result = mExecution->executePawnPromotion(promotionMove, PlayerColor::White);

	// Verify: Promotion was successfull
	EXPECT_TRUE(result) << "Pawn promotion should succeed";

	// Verify: pawn has been replaced with a queen at a8
	auto pieceAtA8 = mBoard->getPiece(end);
	ASSERT_NE(pieceAtA8, nullptr) << "There should be a piece at the promotion square";
	EXPECT_EQ(pieceAtA8->getType(), PieceType::Queen) << "The promoted piece should be a queen";
	EXPECT_EQ(pieceAtA8->getColor(), PlayerColor::White) << "The promoted piece should be white";

	// Verify: pawn was removed from a7
	EXPECT_TRUE(mBoard->isEmpty(start)) << "The original pawn position should be empty";
}


TEST_F(PawnPromotionTests, ExecutePawnPromotionToAllPieceTypes)
{
	// Define promotion pieces
	std::vector<PieceType> promotionPieces = {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight};

	// Check each promotion move
	for (auto promotionPiece : promotionPieces)
	{
		// Setup fresh board for each test
		SetUp();
		SetupWhitePawnPromotionPosition();

		// Create promotion move
		Position	 start = {0, 1}; // a7
		Position	 end   = {0, 0}; // a8
		PossibleMove promotionMove{start, end, MoveType::PawnPromotion, promotionPiece};

		// Execute pawn promotion
		bool		 result = mExecution->executePawnPromotion(promotionMove, PlayerColor::White);

		// Verify: promotion was successful
		EXPECT_TRUE(result) << "Pawn promotion to " << static_cast<int>(promotionPiece) << " should succeed";

		// Verify: pawn has been replaced with the correct piece type
		auto promotedPiece = mBoard->getPiece(end);
		ASSERT_NE(promotedPiece, nullptr) << "There should be a piece at the promotion square";
		EXPECT_EQ(promotedPiece->getType(), promotionPiece) << "The promoted piece should match the requested type";
		EXPECT_EQ(promotedPiece->getColor(), PlayerColor::White) << "The promoted piece should be white";
	}
}

TEST_F(PawnPromotionTests, ExecutePawnPromotionWithFullMove)
{
	SetupWhitePawnPromotionPosition();

	// Create promotion move: white pawn from a7 to a8, promoting to queen
	Position	 start = {0, 1}; // a7
	Position	 end   = {0, 0}; // a8
	PossibleMove promotionMove{start, end, MoveType::PawnPromotion, PieceType::Queen};

	// Execute the move through MoveExecution
	auto		 executedMove = mExecution->executeMove(promotionMove);

	// Verify: move was executed and recorded properly
	EXPECT_EQ(executedMove.movedPiece, PieceType::Pawn) << "The moved piece should be recorded as a pawn";
	EXPECT_EQ(executedMove.promotionType, PieceType::Queen) << "The promotion type should be recorded as queen";
	EXPECT_TRUE((executedMove.type & MoveType::PawnPromotion) == MoveType::PawnPromotion) << "Move should be marked as promotion";

	// Verify: pawn has been replaced with a queen at a8
	auto pieceAtA8 = mBoard->getPiece(end);
	ASSERT_NE(pieceAtA8, nullptr) << "There should be a piece at the promotion square";
	EXPECT_EQ(pieceAtA8->getType(), PieceType::Queen) << "The promoted piece should be a queen";
	EXPECT_EQ(pieceAtA8->getColor(), PlayerColor::White) << "The promoted piece should be white";
}


TEST_F(PawnPromotionTests, PawnPromotionWithCapture)
{
	SetupWhitePawnPromotionPosition();

	// Place a black piece at the promotion square
	Position end = {0, 0}; // a8
	mBoard->setPiece(end, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));

	// Create promotion with capture move
	Position	 start = {0, 1}; // a7
	PossibleMove promotionMove{start, end, MoveType::PawnPromotion | MoveType::Capture, PieceType::Queen};

	// Execute the move
	auto		 executedMove = mExecution->executeMove(promotionMove);

	// Verify: move was executed and recorded properly
	EXPECT_EQ(executedMove.movedPiece, PieceType::Pawn) << "The moved piece should be recorded as a pawn";
	EXPECT_EQ(executedMove.promotionType, PieceType::Queen) << "The promotion type should be recorded as queen";
	EXPECT_EQ(executedMove.capturedPiece, PieceType::Rook) << "The captured piece should be recorded as rook";
	EXPECT_TRUE((executedMove.type & MoveType::PawnPromotion) == MoveType::PawnPromotion) << "Move should be marked as promotion";
	EXPECT_TRUE((executedMove.type & MoveType::Capture) == MoveType::Capture) << "Move should be marked as capture";

	// Verify: promotion square has the promoted piece
	auto pieceAtPromotionSquare = mBoard->getPiece(end);
	ASSERT_NE(pieceAtPromotionSquare, nullptr) << "There should be a piece at the promotion square";
	EXPECT_EQ(pieceAtPromotionSquare->getType(), PieceType::Queen) << "The promoted piece should be a queen";
	EXPECT_EQ(pieceAtPromotionSquare->getColor(), PlayerColor::White) << "The promoted piece should be white";
}


TEST_F(PawnPromotionTests, BlackPawnPromotion)
{
	SetupBlackPawnPromotionPosition();

	// Create promotion move: black pawn from a2 to a1, promoting to queen
	Position	 start = {0, 6}; // a2
	Position	 end   = {0, 7}; // a1
	PossibleMove promotionMove{start, end, MoveType::PawnPromotion, PieceType::Queen};

	// Execute pawn promotion
	bool		 result = mExecution->executePawnPromotion(promotionMove, PlayerColor::Black);

	// Verify: the promotion was successful
	EXPECT_TRUE(result) << "Black pawn promotion should succeed";

	// Verify: the pawn has been replaced with a queen
	auto pieceAtA1 = mBoard->getPiece(end);
	ASSERT_NE(pieceAtA1, nullptr) << "There should be a piece at the promotion square";
	EXPECT_EQ(pieceAtA1->getType(), PieceType::Queen) << "The promoted piece should be a queen";
	EXPECT_EQ(pieceAtA1->getColor(), PlayerColor::Black) << "The promoted piece should be black";
}


TEST_F(PawnPromotionTests, InvalidPawnPromotion)
{
	SetupWhitePawnPromotionPosition();

	// Try to promote to king (which is not allowed)
	Position	 start = {0, 1}; // a7
	Position	 end   = {0, 0}; // a8
	PossibleMove invalidPromotionMove{start, end, MoveType::PawnPromotion, PieceType::King};

	// Execute pawn promotion
	bool		 result = mExecution->executePawnPromotion(invalidPromotionMove, PlayerColor::White);

	// Verify: the promotion failed
	EXPECT_FALSE(result) << "Promotion to king should fail";

	// Verify: the board state hasn't changed
	auto pieceAtA7 = mBoard->getPiece(start);
	ASSERT_NE(pieceAtA7, nullptr) << "The pawn should still be at the starting position";
	EXPECT_EQ(pieceAtA7->getType(), PieceType::Pawn) << "The piece should still be a pawn";

	EXPECT_TRUE(mBoard->isEmpty(end)) << "The promotion square should be empty";
}


} // namespace GameMechanicTests

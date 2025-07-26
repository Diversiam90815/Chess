/*
  ==============================================================================
	Module:			Move Evaluation Tests
	Description:    Testing the MoveEvaluation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Moves/Evaluation/MoveEvaluation.h"


namespace MoveTests
{

class MoveEvaluationTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveGeneration> mGeneration;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveEvaluation> mEvaluation;

	void							SetUp() override
	{
		mBoard		= std::make_shared<ChessBoard>();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);

		mBoard->initializeBoard();

		mEvaluation = std::make_shared<MoveEvaluation>(mBoard, mGeneration);
	}
};


TEST_F(MoveEvaluationTests, ConstructorInitializesCorrectly)
{
	EXPECT_NE(mEvaluation, nullptr) << "MoveEvaluation should be constructed successfully";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationNormalMoveReturnsZero)
{
	PossibleMove normalMove{{0, 1}, {0, 2}, MoveType::Normal};

	int			 score = mEvaluation->getBasicEvaluation(normalMove);

	EXPECT_EQ(score, 0) << "Normal move should have basic evaluation score of 0";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationCaptureMove)
{
	PossibleMove captureMove{{0, 1}, {0, 2}, MoveType::Capture};

	int			 score = mEvaluation->getBasicEvaluation(captureMove);

	EXPECT_EQ(score, 50) << "Capture move should have basic evaluation score of 50";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationCheckMove)
{
	PossibleMove checkMove{{0, 1}, {0, 2}, MoveType::Check};

	int			 score = mEvaluation->getBasicEvaluation(checkMove);

	EXPECT_EQ(score, 15) << "Check move should have basic evaluation score of 15";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationCheckmateMove)
{
	PossibleMove checkmateMove{{0, 1}, {0, 2}, MoveType::Checkmate};

	int			 score = mEvaluation->getBasicEvaluation(checkmateMove);

	EXPECT_EQ(score, 1000) << "Checkmate move should have basic evaluation score of 1000";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationPawnPromotion)
{
	PossibleMove promotionMove{{0, 1}, {0, 2}, MoveType::PawnPromotion};
	promotionMove.promotionPiece = PieceType::Rook;

	int score					 = mEvaluation->getBasicEvaluation(promotionMove);

	EXPECT_EQ(score, 800) << "Pawn promotion should have basic evaluation score of 800";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationPawnPromotionToQueen)
{
	PossibleMove promotionMove{{0, 1}, {0, 2}, MoveType::PawnPromotion};
	promotionMove.promotionPiece = PieceType::Queen;

	int score					 = mEvaluation->getBasicEvaluation(promotionMove);

	EXPECT_EQ(score, 900) << "Pawn promotion to Queen should have basic evaluation score of 900 (800 + 100 bonus)";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationKingsideCastling)
{
	PossibleMove castlingMove{{0, 1}, {0, 2}, MoveType::CastlingKingside};

	int			 score = mEvaluation->getBasicEvaluation(castlingMove);

	EXPECT_EQ(score, 30) << "Kingside castling should have basic evaluation score of 30";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationQueensideCastling)
{
	PossibleMove castlingMove{{0, 1}, {0, 2}, MoveType::CastlingQueenside};

	int			 score = mEvaluation->getBasicEvaluation(castlingMove);

	EXPECT_EQ(score, 30) << "Queenside castling should have basic evaluation score of 30";
}


TEST_F(MoveEvaluationTests, GetBasicEvaluationCombinedMoveTypes)
{
	PossibleMove combinedMove{{0, 1}, {0, 2}, MoveType::Capture | MoveType::Check};

	int			 score = mEvaluation->getBasicEvaluation(combinedMove);

	EXPECT_EQ(score, 65) << "Capture + Check should have combined score of 65 (50 + 15)";
}


TEST_F(MoveEvaluationTests, GetPieceValueReturnsCorrectValues)
{
	EXPECT_EQ(mEvaluation->getPieceValue(PieceType::Pawn), 100) << "Pawn value should be 100";
	EXPECT_EQ(mEvaluation->getPieceValue(PieceType::Knight), 320) << "Knight value should be 320";
	EXPECT_EQ(mEvaluation->getPieceValue(PieceType::Bishop), 330) << "Bishop value should be 330";
	EXPECT_EQ(mEvaluation->getPieceValue(PieceType::Rook), 500) << "Rook value should be 500";
	EXPECT_EQ(mEvaluation->getPieceValue(PieceType::Queen), 900) << "Queen value should be 900";
	EXPECT_EQ(mEvaluation->getPieceValue(PieceType::King), 0) << "King value should be 0 (capture is checkmate)";
}


TEST_F(MoveEvaluationTests, GetPositionValueInvalidPositionReturnsZero)
{
	Position invalidPos{-1, -1};

	int		 value = mEvaluation->getPositionValue(PieceType::Pawn, invalidPos, PlayerColor::White);

	EXPECT_EQ(value, 0) << "Invalid position should return 0";
}


TEST_F(MoveEvaluationTests, GetPositionValuePawnWhiteInitialRow)
{
	Position pawnPos{0, 6}; // a2

	int		 value = mEvaluation->getPositionValue(PieceType::Pawn, pawnPos, PlayerColor::White);

	// From PAWN_TABLE[6][4] (white perspective, row 6 = second rank for white)
	EXPECT_EQ(value, 5) << "Pawn on a2 should have position value from pawn table";
}


TEST_F(MoveEvaluationTests, GetPositionValuePawnBlackFlippedTable)
{
	Position pawnPos{4, 1}; // e7 for black

	int		 value = mEvaluation->getPositionValue(PieceType::Pawn, pawnPos, PlayerColor::Black);

	// For black, row should be flipped: 7 - 1 = 6, so PAWN_TABLE[6][4]
	EXPECT_EQ(value, -20) << "Black pawn on e7 should have same position value as white pawn on e2 due to table flipping";
}


TEST_F(MoveEvaluationTests, GetPositionValueKnightCenterSquare)
{
	Position knightPos{4, 4}; // e4

	int		 value = mEvaluation->getPositionValue(PieceType::Knight, knightPos, PlayerColor::White);

	// From KNIGHT_TABLE[4][4]
	EXPECT_EQ(value, 20) << "Knight on e4 should have high position value (center control)";
}


TEST_F(MoveEvaluationTests, EvaluateMaterialGainNonCaptureReturnsZero)
{
	PossibleMove normalMove{{0, 1}, {0, 2}, MoveType::Normal};

	int			 gain = mEvaluation->evaluateMaterialGain(normalMove);

	EXPECT_EQ(gain, 0) << "Non-capture move should have no material gain";
}


TEST_F(MoveEvaluationTests, EvaluateMaterialGainCaptureWithNoPiece)
{
	PossibleMove captureMove{{0, 1}, {4, 7}, MoveType::Capture}; // Capture on empty square

	int			 gain = mEvaluation->evaluateMaterialGain(captureMove);

	EXPECT_EQ(gain, 0) << "Capture of empty square should have no material gain";
}


TEST_F(MoveEvaluationTests, EvaluateMaterialGainCaptureWithPiece)
{
	// Setup a piece to be captured
	mBoard->setPiece({7, 7}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));

	PossibleMove captureMove{{0, 1}, {7, 7}, MoveType::Capture};

	int			 gain = mEvaluation->evaluateMaterialGain(captureMove);

	EXPECT_EQ(gain, 900) << "Capturing a queen should give material gain of 900";
}


TEST_F(MoveEvaluationTests, EvaluatePositionalGainWithNoPiece)
{
	PossibleMove move{{7, 7}, {6, 6}, MoveType::Normal}; // From empty square

	int			 gain = mEvaluation->evaluatePositionalGain(move, PlayerColor::White);

	EXPECT_EQ(gain, 0) << "Moving from empty square should have no positional gain";
}


TEST_F(MoveEvaluationTests, EvaluatePositionalGainPawnAdvancement)
{
	Position	 from{4, 6}; // e2
	Position	 to{4, 4};	 // e4
	PossibleMove move{from, to, MoveType::Normal};

	int			 gain = mEvaluation->evaluatePositionalGain(move, PlayerColor::White);

	// Should be positive as pawn moves from e2 to e4 (better position)
	EXPECT_GT(gain, 0) << "Pawn advancing from e2 to e4 should have positive positional gain";
}


TEST_F(MoveEvaluationTests, PositionTablesHaveCorrectDimensions)
{
	// Test that position tables are accessible and have correct structure
	// This is more of a compile-time check, but we can verify some basic properties

	// Test center squares have higher values for knights
	Position centerPos{3, 3}; // d4
	Position cornerPos{0, 0}; // a8

	int		 centerValue = mEvaluation->getPositionValue(PieceType::Knight, centerPos, PlayerColor::White);
	int		 cornerValue = mEvaluation->getPositionValue(PieceType::Knight, cornerPos, PlayerColor::White);

	EXPECT_GT(centerValue, cornerValue) << "Knight should prefer center squares over corner squares";
}


TEST_F(MoveEvaluationTests, BlackPlayerTableFlippingWorks)
{
	Position whiteKingPos{4, 7}; // e1 for white
	Position blackKingPos{4, 0}; // e8 for black

	int		 whiteKingValue = mEvaluation->getPositionValue(PieceType::King, whiteKingPos, PlayerColor::White);
	int		 blackKingValue = mEvaluation->getPositionValue(PieceType::King, blackKingPos, PlayerColor::Black);

	EXPECT_EQ(whiteKingValue, blackKingValue) << "King position values should be equal when accounting for table flipping";
}


} // namespace MoveTests
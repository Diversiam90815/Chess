/*
  ==============================================================================
	Module:			Light ChessBoard Tests
	Description:    Testing the LightChessBoard module
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Board/LightChessBoard.h"
#include "Board/ChessBoard.h"

namespace BoardTests
{

class LightChessBoardTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		mChessBoard = std::make_shared<ChessBoard>();
		mChessBoard->initializeBoard();
	}

	std::shared_ptr<ChessBoard> mChessBoard;
	LightChessBoard				mLightBoard;
};



// =============================================================================
// INITIALIZATION AND CONSTRUCTION TESTS
// =============================================================================


TEST_F(LightChessBoardTests, DefaultConstructorInitializesEmptyBoard)
{
	LightChessBoard lightBoard;

	// Test that all squares are empty
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			Position pos{x, y};
			EXPECT_TRUE(lightBoard.isEmpty(pos)) << "Default constructed board should be empty at (" << x << "," << y << ")";
		}
	}

	// Test initial game state
	EXPECT_EQ(lightBoard.getCurrentPlayer(), PlayerColor::White) << "Initial player should be White";
	EXPECT_EQ(lightBoard.getHalfMoveClock(), 0) << "Initial half-move clock should be 0";
	EXPECT_EQ(lightBoard.getFullMoveNumber(), 1) << "Initial full-move number should be 1";
}


TEST_F(LightChessBoardTests, CopyConstructorFromChessBoard)
{
	LightChessBoard lightBoard(*mChessBoard);

	// Verify pieces are copied correctly
	EXPECT_FALSE(lightBoard.isEmpty({0, 0})) << "a8 should have black rook";
	EXPECT_FALSE(lightBoard.isEmpty({4, 7})) << "e1 should have white king";
	EXPECT_FALSE(lightBoard.isEmpty({4, 0})) << "e8 should have black king";

	// Test piece types
	auto piece = lightBoard.getPiece({0, 0});
	EXPECT_EQ(piece.type, PieceType::Rook) << "a8 should contain rook";
	EXPECT_EQ(piece.color, PlayerColor::Black) << "a8 should contain black piece";

	// Test king positions
	EXPECT_EQ(lightBoard.getKingPosition(PlayerColor::White), Position({4, 7})) << "White king at e1";
	EXPECT_EQ(lightBoard.getKingPosition(PlayerColor::Black), Position({4, 0})) << "Black king at e8";
}


TEST_F(LightChessBoardTests, CopyConstructorFromLightChessBoard)
{
	LightChessBoard original(*mChessBoard);
	LightChessBoard copy(original);

	// Verify all pieces are copied
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			Position pos{x, y};
			EXPECT_EQ(original.isEmpty(pos), copy.isEmpty(pos)) << "Piece state should match at (" << x << "," << y << ")";

			if (!original.isEmpty(pos))
			{
				auto originalPiece = original.getPiece(pos);
				auto copyPiece	   = copy.getPiece(pos);
				EXPECT_EQ(originalPiece.type, copyPiece.type) << "Piece types should match";
				EXPECT_EQ(originalPiece.color, copyPiece.color) << "Piece colors should match";
			}
		}
	}

	// Verify game state is copied
	EXPECT_EQ(original.getCurrentPlayer(), copy.getCurrentPlayer()) << "Current player should match";
	EXPECT_EQ(original.getHalfMoveClock(), copy.getHalfMoveClock()) << "Half-move clock should match";
	EXPECT_EQ(original.getFullMoveNumber(), copy.getFullMoveNumber()) << "Full-move number should match";
}


TEST_F(LightChessBoardTests, AssignmentOperator)
{
	LightChessBoard original(*mChessBoard);
	LightChessBoard assigned;

	assigned = original;

	// Verify assignment worked
	EXPECT_EQ(original.getCurrentPlayer(), assigned.getCurrentPlayer()) << "Current player should match after assignment";
	EXPECT_EQ(original.getKingPosition(PlayerColor::White), assigned.getKingPosition(PlayerColor::White)) << "White king position should match";
	EXPECT_EQ(original.getKingPosition(PlayerColor::Black), assigned.getKingPosition(PlayerColor::Black)) << "Black king position should match";
}


TEST_F(LightChessBoardTests, InitializeStartingPosition)
{
	mLightBoard.initializeStartingPosition();

	// Test back rank pieces for White
	EXPECT_EQ(mLightBoard.getPiece({0, 7}).type, PieceType::Rook) << "a1 should have rook";
	EXPECT_EQ(mLightBoard.getPiece({1, 7}).type, PieceType::Knight) << "b1 should have knight";
	EXPECT_EQ(mLightBoard.getPiece({2, 7}).type, PieceType::Bishop) << "c1 should have bishop";
	EXPECT_EQ(mLightBoard.getPiece({3, 7}).type, PieceType::Queen) << "d1 should have queen";
	EXPECT_EQ(mLightBoard.getPiece({4, 7}).type, PieceType::King) << "e1 should have king";

	// Test pawns for White
	for (int x = 0; x < 8; x++)
	{
		EXPECT_EQ(mLightBoard.getPiece({x, 6}).type, PieceType::Pawn) << "White pawn should be at rank 2";
		EXPECT_EQ(mLightBoard.getPiece({x, 6}).color, PlayerColor::White) << "White pawns should be white";
	}

	// Test back rank pieces for Black
	EXPECT_EQ(mLightBoard.getPiece({0, 0}).type, PieceType::Rook) << "a8 should have rook";
	EXPECT_EQ(mLightBoard.getPiece({4, 0}).type, PieceType::King) << "e8 should have king";

	// Test king positions are set correctly
	EXPECT_EQ(mLightBoard.getKingPosition(PlayerColor::White), Position({4, 7})) << "White king at e1";
	EXPECT_EQ(mLightBoard.getKingPosition(PlayerColor::Black), Position({4, 0})) << "Black king at e8";
}



// =============================================================================
// PIECE ACCESS AND MANIPULATION TESTS
// =============================================================================


TEST_F(LightChessBoardTests, SetAndGetPiece)
{
	Position   pos{4, 4}; // e4
	LightPiece piece(PieceType::Queen, PlayerColor::White);

	mLightBoard.setPiece(pos, piece);

	auto retrieved = mLightBoard.getPiece(pos);
	EXPECT_EQ(retrieved.type, PieceType::Queen) << "Retrieved piece should be queen";
	EXPECT_EQ(retrieved.color, PlayerColor::White) << "Retrieved piece should be white";
}


TEST_F(LightChessBoardTests, SetPieceByCoordinates)
{
	LightPiece piece(PieceType::Rook, PlayerColor::Black);

	mLightBoard.setPiece(3, 5, piece);

	auto retrieved = mLightBoard.getPiece(3, 5);
	EXPECT_EQ(retrieved.type, PieceType::Rook) << "Retrieved piece should be rook";
	EXPECT_EQ(retrieved.color, PlayerColor::Black) << "Retrieved piece should be black";
}


TEST_F(LightChessBoardTests, RemovePiece)
{
	mLightBoard.initializeStartingPosition();

	Position pawnPos{4, 6}; // e2
	EXPECT_FALSE(mLightBoard.isEmpty(pawnPos)) << "e2 should initially have a pawn";

	mLightBoard.removePiece(pawnPos);
	EXPECT_TRUE(mLightBoard.isEmpty(pawnPos)) << "e2 should be empty after removal";
}


TEST_F(LightChessBoardTests, RemovePieceByCoordinates)
{
	mLightBoard.initializeStartingPosition();

	EXPECT_FALSE(mLightBoard.isEmpty(4, 6)) << "e2 should initially have a pawn";

	mLightBoard.removePiece(4, 6);
	EXPECT_TRUE(mLightBoard.isEmpty(4, 6)) << "e2 should be empty after removal";
}


TEST_F(LightChessBoardTests, IsEmpty)
{
	mLightBoard.initializeStartingPosition();

	EXPECT_FALSE(mLightBoard.isEmpty({4, 7})) << "e1 should not be empty (has king)";
	EXPECT_TRUE(mLightBoard.isEmpty({4, 4})) << "e4 should be empty initially";
	EXPECT_FALSE(mLightBoard.isEmpty(4, 6)) << "e2 should not be empty (has pawn)";
	EXPECT_TRUE(mLightBoard.isEmpty(4, 3)) << "e5 should be empty initially";
}


TEST_F(LightChessBoardTests, IsValidPosition)
{
	EXPECT_TRUE(mLightBoard.isValidPosition({0, 0})) << "a8 should be valid";
	EXPECT_TRUE(mLightBoard.isValidPosition({7, 7})) << "h1 should be valid";
	EXPECT_TRUE(mLightBoard.isValidPosition(4, 4)) << "e4 should be valid";

	EXPECT_FALSE(mLightBoard.isValidPosition({-1, 0})) << "Negative x should be invalid";
	EXPECT_FALSE(mLightBoard.isValidPosition({0, -1})) << "Negative y should be invalid";
	EXPECT_FALSE(mLightBoard.isValidPosition({8, 0})) << "x >= 8 should be invalid";
	EXPECT_FALSE(mLightBoard.isValidPosition({0, 8})) << "y >= 8 should be invalid";
	EXPECT_FALSE(mLightBoard.isValidPosition(-1, 4)) << "Negative coordinate should be invalid";
	EXPECT_FALSE(mLightBoard.isValidPosition(4, 8)) << "Out of bounds coordinate should be invalid";
}


TEST_F(LightChessBoardTests, Clear)
{
	mLightBoard.initializeStartingPosition();

	// Verify board has pieces
	EXPECT_FALSE(mLightBoard.isEmpty({4, 7})) << "Board should have pieces before clear";

	mLightBoard.clear();

	// Verify board is empty
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			EXPECT_TRUE(mLightBoard.isEmpty({x, y})) << "Board should be empty after clear at (" << x << "," << y << ")";
		}
	}
}



// =============================================================================
// GAME STATE TESTS
// =============================================================================


TEST_F(LightChessBoardTests, CurrentPlayerManagement)
{
	EXPECT_EQ(mLightBoard.getCurrentPlayer(), PlayerColor::White) << "Initial player should be White";

	mLightBoard.setCurrentPlayer(PlayerColor::Black);
	EXPECT_EQ(mLightBoard.getCurrentPlayer(), PlayerColor::Black) << "Current player should be Black after setting";

	mLightBoard.switchPlayer();
	EXPECT_EQ(mLightBoard.getCurrentPlayer(), PlayerColor::White) << "Player should switch back to White";

	mLightBoard.switchPlayer();
	EXPECT_EQ(mLightBoard.getCurrentPlayer(), PlayerColor::Black) << "Player should switch to Black again";
}


TEST_F(LightChessBoardTests, KingPositionManagement)
{
	Position whiteKingPos{4, 7}; // e1
	Position blackKingPos{4, 0}; // e8

	mLightBoard.updateKingPosition(whiteKingPos, PlayerColor::White);
	mLightBoard.updateKingPosition(blackKingPos, PlayerColor::Black);

	EXPECT_EQ(mLightBoard.getKingPosition(PlayerColor::White), whiteKingPos) << "White king position should be set";
	EXPECT_EQ(mLightBoard.getKingPosition(PlayerColor::Black), blackKingPos) << "Black king position should be set";

	// Test moving kings
	Position newWhiteKingPos{5, 7}; // f1
	mLightBoard.updateKingPosition(newWhiteKingPos, PlayerColor::White);
	EXPECT_EQ(mLightBoard.getKingPosition(PlayerColor::White), newWhiteKingPos) << "White king position should be updated";
}


TEST_F(LightChessBoardTests, MoveCounters)
{
	EXPECT_EQ(mLightBoard.getHalfMoveClock(), 0) << "Initial half-move clock should be 0";
	EXPECT_EQ(mLightBoard.getFullMoveNumber(), 1) << "Initial full-move number should be 1";

	mLightBoard.setHalfMoveClock(5);
	mLightBoard.setFullMoveNumber(10);

	EXPECT_EQ(mLightBoard.getHalfMoveClock(), 5) << "Half-move clock should be set to 5";
	EXPECT_EQ(mLightBoard.getFullMoveNumber(), 10) << "Full-move number should be set to 10";
}


TEST_F(LightChessBoardTests, EnPassantTarget)
{
	Position enPassantPos{4, 5}; // e3

	EXPECT_EQ(mLightBoard.getEnPassantTarget(), Position({-1, -1})) << "Initial en passant target should be invalid";

	mLightBoard.setEnPassantTarget(enPassantPos);
	EXPECT_EQ(mLightBoard.getEnPassantTarget(), enPassantPos) << "En passant target should be set";

	mLightBoard.setEnPassantTarget({-1, -1});
	EXPECT_EQ(mLightBoard.getEnPassantTarget(), Position({-1, -1})) << "En passant target should be cleared";
}



// =============================================================================
// CASTLING RIGHTS TESTS
// =============================================================================


TEST_F(LightChessBoardTests, CastlingRights)
{
	// Test initial castling rights (should be true by default)
	EXPECT_TRUE(mLightBoard.canCastleKingside(PlayerColor::White)) << "White should initially be able to castle kingside";
	EXPECT_TRUE(mLightBoard.canCastleQueenside(PlayerColor::White)) << "White should initially be able to castle queenside";
	EXPECT_TRUE(mLightBoard.canCastleKingside(PlayerColor::Black)) << "Black should initially be able to castle kingside";
	EXPECT_TRUE(mLightBoard.canCastleQueenside(PlayerColor::Black)) << "Black should initially be able to castle queenside";

	// Test setting castling rights
	mLightBoard.setCastlingRights(PlayerColor::White, false, true);
	EXPECT_FALSE(mLightBoard.canCastleKingside(PlayerColor::White)) << "White kingside castling should be disabled";
	EXPECT_TRUE(mLightBoard.canCastleQueenside(PlayerColor::White)) << "White queenside castling should remain enabled";

	mLightBoard.setCastlingRights(PlayerColor::Black, true, false);
	EXPECT_TRUE(mLightBoard.canCastleKingside(PlayerColor::Black)) << "Black kingside castling should be enabled";
	EXPECT_FALSE(mLightBoard.canCastleQueenside(PlayerColor::Black)) << "Black queenside castling should be disabled";
}


TEST_F(LightChessBoardTests, InferCastlingRights)
{
	mLightBoard.initializeStartingPosition();

	// Initially all castling should be possible
	mLightBoard.inferCastlingRights();
	EXPECT_TRUE(mLightBoard.canCastleKingside(PlayerColor::White)) << "White should be able to castle kingside";
	EXPECT_TRUE(mLightBoard.canCastleQueenside(PlayerColor::White)) << "White should be able to castle queenside";

	// Move white king - should disable all white castling
	mLightBoard.setPiece({4, 7}, LightPiece());									   // Remove king from e1
	mLightBoard.setPiece({5, 7}, LightPiece(PieceType::King, PlayerColor::White)); // Put king on f1
	mLightBoard.updateKingPosition({5, 7}, PlayerColor::White);

	mLightBoard.inferCastlingRights();
	EXPECT_FALSE(mLightBoard.canCastleKingside(PlayerColor::White)) << "White kingside castling should be disabled after king move";
	EXPECT_FALSE(mLightBoard.canCastleQueenside(PlayerColor::White)) << "White queenside castling should be disabled after king move";
}



// =============================================================================
// MOVE OPERATIONS TESTS
// =============================================================================


TEST_F(LightChessBoardTests, MakeAndUnmakeMove)
{
	mLightBoard.initializeStartingPosition();

	PossibleMove move{{4, 6}, {4, 4}, MoveType::Normal}; // e2-e4

	// Verify initial state
	EXPECT_FALSE(mLightBoard.isEmpty({4, 6})) << "e2 should have pawn initially";
	EXPECT_TRUE(mLightBoard.isEmpty({4, 4})) << "e4 should be empty initially";

	// Make move
	MoveUndo undoInfo = mLightBoard.makeMove(move);

	// Verify move was made
	EXPECT_TRUE(mLightBoard.isEmpty({4, 6})) << "e2 should be empty after move";
	EXPECT_FALSE(mLightBoard.isEmpty({4, 4})) << "e4 should have pawn after move";
	EXPECT_EQ(mLightBoard.getPiece({4, 4}).type, PieceType::Pawn) << "e4 should contain pawn";

	// Unmake move
	mLightBoard.unmakeMove(undoInfo);

	// Verify move was undone
	EXPECT_FALSE(mLightBoard.isEmpty({4, 6})) << "e2 should have pawn after undo";
	EXPECT_TRUE(mLightBoard.isEmpty({4, 4})) << "e4 should be empty after undo";
}


TEST_F(LightChessBoardTests, MakeCaptureMove)
{
	mLightBoard.clear();

	// Set up capture scenario
	mLightBoard.setPiece({4, 4}, LightPiece(PieceType::Pawn, PlayerColor::White));
	mLightBoard.setPiece({5, 3}, LightPiece(PieceType::Pawn, PlayerColor::Black));

	PossibleMove captureMove{{4, 4}, {5, 3}, MoveType::Capture};

	// Make capture
	MoveUndo	 undoInfo = mLightBoard.makeMove(captureMove);

	// Verify capture
	EXPECT_TRUE(mLightBoard.isEmpty({4, 4})) << "e4 should be empty after capture";
	EXPECT_FALSE(mLightBoard.isEmpty({5, 3})) << "f5 should have capturing piece";
	EXPECT_EQ(mLightBoard.getPiece({5, 3}).color, PlayerColor::White) << "f5 should have white piece";

	// Unmake capture
	mLightBoard.unmakeMove(undoInfo);

	// Verify restoration
	EXPECT_FALSE(mLightBoard.isEmpty({4, 4})) << "e4 should have white pawn after undo";
	EXPECT_FALSE(mLightBoard.isEmpty({5, 3})) << "f5 should have black pawn after undo";
	EXPECT_EQ(mLightBoard.getPiece({5, 3}).color, PlayerColor::Black) << "f5 should have black piece after undo";
}



// =============================================================================
// UTILITY AND ANALYSIS TESTS
// =============================================================================


TEST_F(LightChessBoardTests, GetPiecePositions)
{
	mLightBoard.initializeStartingPosition();

	auto whitePieces = mLightBoard.getPiecePositions(PlayerColor::White);
	auto blackPieces = mLightBoard.getPiecePositions(PlayerColor::Black);

	EXPECT_EQ(whitePieces.size(), 16) << "White should have 16 pieces initially";
	EXPECT_EQ(blackPieces.size(), 16) << "Black should have 16 pieces initially";

	// Test specific piece type positions
	auto whitePawns = mLightBoard.getPiecePositions(PlayerColor::White, PieceType::Pawn);
	auto whiteKings = mLightBoard.getPiecePositions(PlayerColor::White, PieceType::King);

	EXPECT_EQ(whitePawns.size(), 8) << "White should have 8 pawns";
	EXPECT_EQ(whiteKings.size(), 1) << "White should have 1 king";
}


TEST_F(LightChessBoardTests, GetMaterialValue)
{
	mLightBoard.initializeStartingPosition();

	int whiteValue = mLightBoard.getMaterialValue(PlayerColor::White);
	int blackValue = mLightBoard.getMaterialValue(PlayerColor::Black);

	EXPECT_GT(whiteValue, 0) << "White should have positive material value";
	EXPECT_EQ(whiteValue, blackValue) << "White and black should have equal material value initially";

	// Remove a piece and test again
	mLightBoard.removePiece({3, 7}); // Remove white queen

	int newWhiteValue = mLightBoard.getMaterialValue(PlayerColor::White);
	EXPECT_LT(newWhiteValue, whiteValue) << "White material value should decrease after losing queen";
}


TEST_F(LightChessBoardTests, GetPieceCount)
{
	mLightBoard.initializeStartingPosition();

	EXPECT_EQ(mLightBoard.getPieceCount(PlayerColor::White), 16) << "White should have 16 pieces";
	EXPECT_EQ(mLightBoard.getPieceCount(PlayerColor::Black), 16) << "Black should have 16 pieces";
	EXPECT_EQ(mLightBoard.getPieceCount(PlayerColor::White, PieceType::Pawn), 8) << "White should have 8 pawns";
	EXPECT_EQ(mLightBoard.getPieceCount(PlayerColor::Black, PieceType::Queen), 1) << "Black should have 1 queen";
}


TEST_F(LightChessBoardTests, IsSquareAttacked)
{
	mLightBoard.clear();

	// Set up attack scenario: white rook at a1, test if h1 is attacked
	mLightBoard.setPiece({0, 7}, LightPiece(PieceType::Rook, PlayerColor::White));

	EXPECT_TRUE(mLightBoard.isSquareAttacked({7, 7}, PlayerColor::White)) << "h1 should be attacked by rook on a1";
	EXPECT_FALSE(mLightBoard.isSquareAttacked({7, 6}, PlayerColor::White)) << "h2 should not be attacked by rook on a1";

	// Test knight attack
	mLightBoard.setPiece({2, 6}, LightPiece(PieceType::Knight, PlayerColor::Black));

	EXPECT_TRUE(mLightBoard.isSquareAttacked({4, 7}, PlayerColor::Black)) << "e1 should be attacked by knight on c2";
	EXPECT_TRUE(mLightBoard.isSquareAttacked({0, 7}, PlayerColor::Black)) << "a1 should be attacked by knight on c2";
}


TEST_F(LightChessBoardTests, IsInCheck)
{
	mLightBoard.clear();

	// Set up check scenario
	Position whiteKingPos{4, 7}; // e1
	Position blackRookPos{4, 0}; // e8

	mLightBoard.setPiece(whiteKingPos, LightPiece(PieceType::King, PlayerColor::White));
	mLightBoard.setPiece(blackRookPos, LightPiece(PieceType::Rook, PlayerColor::Black));
	mLightBoard.updateKingPosition(whiteKingPos, PlayerColor::White);

	EXPECT_TRUE(mLightBoard.isInCheck(PlayerColor::White)) << "White king should be in check from black rook";
	EXPECT_FALSE(mLightBoard.isInCheck(PlayerColor::Black)) << "Black should not be in check";
}


TEST_F(LightChessBoardTests, GetHashKey)
{
	mLightBoard.initializeStartingPosition();

	uint64_t hash1 = mLightBoard.getHashKey();
	uint64_t hash2 = mLightBoard.getHashKey();

	EXPECT_EQ(hash1, hash2) << "Hash should be consistent for same position";

	// Make a move and verify hash changes
	PossibleMove move{{4, 6}, {4, 4}, MoveType::Normal};
	mLightBoard.makeMove(move);

	uint64_t hash3 = mLightBoard.getHashKey();
	EXPECT_NE(hash1, hash3) << "Hash should change after move";
}


TEST_F(LightChessBoardTests, IsEndgame)
{
	mLightBoard.initializeStartingPosition();

	EXPECT_FALSE(mLightBoard.isEndgame()) << "Full board should not be endgame";

	// Remove most pieces to create endgame
	mLightBoard.clear();
	mLightBoard.setPiece({4, 7}, LightPiece(PieceType::King, PlayerColor::White));
	mLightBoard.setPiece({4, 0}, LightPiece(PieceType::King, PlayerColor::Black));
	mLightBoard.setPiece({0, 7}, LightPiece(PieceType::Rook, PlayerColor::White));

	EXPECT_TRUE(mLightBoard.isEndgame()) << "Position with few pieces should be endgame";
}


TEST_F(LightChessBoardTests, GetGamePhaseValue)
{
	mLightBoard.initializeStartingPosition();

	int openingPhase = mLightBoard.getGamePhaseValue();
	EXPECT_EQ(openingPhase, 1) << "Opening position should have Opening (1) phase value";

	// Create endgame position
	mLightBoard.clear();
	mLightBoard.setPiece({4, 7}, LightPiece(PieceType::King, PlayerColor::White));
	mLightBoard.setPiece({4, 0}, LightPiece(PieceType::King, PlayerColor::Black));

	int endgamePhase = mLightBoard.getGamePhaseValue();
	EXPECT_GT(endgamePhase, openingPhase) << "Endgame position should have higher (3) phase value than opening (1)";
}


} // namespace BoardTests
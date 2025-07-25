/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Evaluation of moves
  ==============================================================================
*/


#include "MoveEvaluation.h"


MoveEvaluation::MoveEvaluation(std::shared_ptr<ChessBoard> chessboard) : mBoard(chessboard) {}


int MoveEvaluation::getBasicEvaluation(const PossibleMove &move)
{
	int score = 0;

	// Capture bonus
	if ((move.type & MoveType::Capture) == MoveType::Capture)
		score += 50;

	// Check bonus
	if ((move.type & MoveType::Check) == MoveType::Check)
		score += 15;

	// Checkmate bonus
	if ((move.type & MoveType::Checkmate) == MoveType::Checkmate)
		score += 1000;

	// Promotion bonus
	if ((move.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	{
		score += 800;

		// Additional bonus for promoting to queen
		if (move.promotionPiece == PieceType::Queen)
			score += 100;
	}

	// Castling bonus
	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		score += 30;

	return score;
}


int MoveEvaluation::getMediumEvaluation(const PossibleMove &move, PlayerColor player)
{
	int score = getBasicEvaluation(move);

	// Material evaluation
	score += evaluateMaterialGain(move);

	// Positional evaluation
	score += evaluatePositionalGain(move, player);

	return score;
}


int MoveEvaluation::getAdvancedEvaluation(const PossibleMove &move, PlayerColor player)
{
	// TODO: Implement with more tactical analysis,...
	return getMediumEvaluation(move, player);
}


int MoveEvaluation::getPositionValue(PieceType piece, const Position &pos, PlayerColor player)
{
	// Ensure position is valid
	if (!pos.isValid())
		return 0;

	int row = pos.y;
	int col = pos.x;

	// For black pieces: flip the table
	if (player == PlayerColor::Black)
		row = 7 - row;

	switch (piece)
	{
	case PieceType::Pawn: return PAWN_TABLE[row][col];
	case PieceType::Knight: return KNIGHT_TABLE[row][col];
	case PieceType::Bishop: return BISHOP_TABLE[row][col];
	case PieceType::Rook: return ROOK_TABLE[row][col];
	case PieceType::Queen: return QUEEN_TABLE[row][col];
	case PieceType::King: return KING_TABLE[row][col];
	default: return 0;
	}
}


int MoveEvaluation::evaluateMaterialGain(const PossibleMove &move)
{
	if ((move.type & MoveType::Capture) != MoveType::Capture)
		return 0;

	auto capturedPiece = mBoard->getPiece(move.end);

	if (!capturedPiece)
		return 0;

	return getPieceValue(capturedPiece->getType());
}


int MoveEvaluation::evaluatePositionalGain(const PossibleMove &move, PlayerColor player)
{
	int	  positionalScore = 0;

	auto &movingPiece	  = mBoard->getPiece(move.start);

	if (!movingPiece)
		return 0;

	PieceType piece = movingPiece->getType();

	// Add positional value for destination
	positionalScore += getPositionValue(piece, move.end, player);

	// Subtract position value for leaving current square
	positionalScore -= getPositionValue(piece, move.start, player);

	return positionalScore;
}


constexpr int MoveEvaluation::getPieceValue(PieceType piece)
{
	switch (piece)
	{
	case PieceType::Pawn: return PAWN_VALUE;
	case PieceType::Knight: return KNIGHT_VALUE;
	case PieceType::Bishop: return BISHOP_VALUE;
	case PieceType::Rook: return ROOK_VALUE;
	case PieceType::Queen: return QUEEN_VALUE;
	case PieceType::King: return 0; // King capture is checkmate
	default: return 0;
	}
}

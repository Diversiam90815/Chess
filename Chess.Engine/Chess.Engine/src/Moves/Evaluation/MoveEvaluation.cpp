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

	auto &capturedPiece = mBoard->getPiece(move.end);

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


int MoveEvaluation::evaluateThreadLevel(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


int MoveEvaluation::evaluateKingSafety(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


int MoveEvaluation::evaluateCenterControl(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


int MoveEvaluation::evaluatePawnStructure(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


int MoveEvaluation::evaluatePieceActivity(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


int MoveEvaluation::evaluateDefensivePatterns(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


bool MoveEvaluation::createsPin(const PossibleMove &move, PlayerColor player)
{
	return false;
}


bool MoveEvaluation::createsFork(const PossibleMove &move, PlayerColor player)
{
	return false;
}


bool MoveEvaluation::createsSkewer(const PossibleMove &move, PlayerColor player)
{
	return false;
}


bool MoveEvaluation::blocksEnemyThreats(const PossibleMove &move, PlayerColor player)
{
	return false;
}


int MoveEvaluation::getStrategicEvaluation(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


int MoveEvaluation::getTacticalEvaluation(const PossibleMove &move, PlayerColor player)
{
	return 0;
}


GamePhase MoveEvaluation::determineGamePhase() const
{
	return GamePhase();
}


int MoveEvaluation::calculateMobility(PlayerColor player) const
{
	return 0;
}


int MoveEvaluation::calculateKingSafetyScore(PlayerColor player) const
{
	return 0;
}


int MoveEvaluation::calculatePawnStructureScore(PlayerColor player) const
{
	return 0;
}


bool MoveEvaluation::isPasssedPawn(const Position &pos, PlayerColor player) const
{
	return false;
}


bool MoveEvaluation::isIsolatedPawn(const Position &pos, PlayerColor player) const
{
	return false;
}


bool MoveEvaluation::isDoublePawn(const Position &pos, PlayerColor player) const
{
	return false;
}


bool MoveEvaluation::isInCenter(const Position &pos) const
{
	return false;
}


bool MoveEvaluation::isNearKing(const Position &pos) const
{
	return false;
}


std::vector<Position> MoveEvaluation::getAttackedSquares(const Position &piecePos, PlayerColor player) const
{
	return std::vector<Position>();
}


bool MoveEvaluation::wouldExposeKing(const PossibleMove &move, PlayerColor player) const
{
	return false;
}


int MoveEvaluation::countAttackers(const Position &target, PlayerColor attackerPlayer) const
{
	return 0;
}


PlayerColor MoveEvaluation::getOpponnentColor(PlayerColor player) const
{
	return PlayerColor();
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

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
		score += CAPTURE_BONUS;

	// Check bonus
	if ((move.type & MoveType::Check) == MoveType::Check)
		score += CHECK_BONUS;

	// Checkmate bonus
	if ((move.type & MoveType::Checkmate) == MoveType::Checkmate)
		score += CHECKMATE_BONUS;

	// Promotion bonus
	if ((move.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	{
		score += PROMOTION_BONUS;

		// Additional bonus for promoting to queen
		if (move.promotionPiece == PieceType::Queen)
			score += 100;
	}

	// Castling bonus
	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		score += CASTLE_BONUS;

	return score;
}


int MoveEvaluation::getMediumEvaluation(const PossibleMove &move, PlayerColor player)
{
	int score = getBasicEvaluation(move);

	score += evaluateMaterialGain(move);		   // Material evaluation
	score += evaluatePositionalGain(move, player); // Positional evaluation
	score += evaluateCenterControl(move, player);  // Center COntrol
	score += evaluateKingSafety(move, player);	   // King Safety
	score += evaluatePieceActivity(move, player);  // Piece activity

	return score;
}


int MoveEvaluation::getAdvancedEvaluation(const PossibleMove &move, PlayerColor player)
{
	int score = getMediumEvaluation(move, player);

	score += getTacticalEvaluation(move, player);	  // Advanced technical evaluation
	score += getStrategicEvaluation(move, player);	  // Advanced strategic evaluation
	score += evaluateThreadLevel(move, player);		  // Threat analysis
	score += evaluateDefensivePatterns(move, player); // Defensive patterns

	return score;
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
	int score = 0;

	// Reward moves that control/occupy center squares
	if (isInCenter(move.end))
		score += CENTER_CONTROL_BONUS;

	// Additional: bonus for pieces attacking center squares
	auto attackedSquares = getAttackedSquares(move.end, player);

	for (const auto &square : attackedSquares)
	{
		if (isInCenter(square))
			score += CENTER_CONTROL_BONUS / 2;
	}

	return score;
}


int MoveEvaluation::evaluatePawnStructure(const PossibleMove &move, PlayerColor player)
{
	int	  score		  = 0;
	auto &movingPiece = mBoard->getPiece(move.start);

	if (!movingPiece || movingPiece->getType() != PieceType::Pawn)
		return 0;

	// Check for pawn structure improvements
	if (isPasssedPawn(move.end, player))
		score += 50;

	if (isIsolatedPawn(move.end, player))
		score -= 20;

	if (isDoublePawn(move.end, player))
		score -= 15;

	return score;
}


int MoveEvaluation::evaluatePieceActivity(const PossibleMove &move, PlayerColor player)
{
	auto &piece = mBoard->getPiece(move.start);

	if (!piece)
		return 0;

	// Count squares piece can attack from new position
	auto attackedSquares = getAttackedSquares(move.end, player);
	int	 mobility		 = static_cast<int>(attackedSquares.size());

	// Reward increased mobility
	return mobility * 2;
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
	return (pos.x >= 2 && pos.x <= 5 && pos.y >= 2 && pos.y <= 5);
}


bool MoveEvaluation::isNearKing(const Position &pos, const Position &kingPos) const
{
	return (abs(pos.x - kingPos.x) <= 2 && abs(pos.y - kingPos.y) <= 2);
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
	int count = 0;

	for (int y = 0; y < BOARD_SIZE; ++y)
	{
		for (int x = 0; x < BOARD_SIZE; ++x)
		{
			Position pos{x, y};
			auto	&piece = mBoard->getPiece(pos);

			if (!piece && piece->getColor() != attackerPlayer)
				continue;

			auto attackedSquares = getAttackedSquares(pos, attackerPlayer);

			for (const auto &square : attackedSquares)
			{
				if (square == target)
				{
					count++;
					break;
				}
			}
		}
	}

	return count;
}


PlayerColor MoveEvaluation::getOpponnentColor(PlayerColor player) const
{
	return (player == PlayerColor::White) ? PlayerColor::White : PlayerColor::Black;
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

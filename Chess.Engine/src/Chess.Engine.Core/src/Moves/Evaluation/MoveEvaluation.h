/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Evaluation of moves
  ==============================================================================
*/

#pragma once

#include <array>
#include <vector>
#include <future>

#include "Parameters.h"
#include "Move.h"
#include "ChessBoard.h"
#include "Generation/MoveGeneration.h"


/// <summary>
/// Enumerates the different phases of a game.
/// </summary>
enum class GamePhase
{
	Opening	   = 1,
	MiddleGame = 2,
	EndGame	   = 3
};


/**
 * @brief Provides layered (basic / medium / advanced) evaluation scores for moves
 *        and exposes granular heuristics for tactical and strategic analysis.
 *
 * Returned scores: Positive values favor the side executing the move.
 */
class MoveEvaluation
{
public:
	MoveEvaluation(Chessboard& board, std::shared_ptr<MoveGeneration> generation);
	~MoveEvaluation() = default;

	int		  getBasicEvaluation(const PossibleMove &move);

	int		  getMediumEvaluation(const PossibleMove &move, PlayerColor player);

	int		  getAdvancedEvaluation(const PossibleMove &move, PlayerColor player);

	//int		  getPieceValue(PieceType piece) const;
	//int		  getPositionValue(PieceType piece, const Position &pos, PlayerColor player) const;
	int		  evaluateMaterialGain(const PossibleMove &move);
	int		  evaluatePositionalGain(const PossibleMove &move, PlayerColor player);
	int		  evaluateThreatLevel(const PossibleMove &move, PlayerColor player);
	int		  evaluateKingSafety(const PossibleMove &move, PlayerColor player);
	int		  evaluateCenterControl(const PossibleMove &move, PlayerColor player);
	int		  evaluatePawnStructure(const PossibleMove &move, PlayerColor player);
	int		  evaluatePieceActivity(const PossibleMove &move, PlayerColor player);
	int		  evaluateDefensivePatterns(const PossibleMove &move, PlayerColor player);
	int		  evaluateOpeningPrinciples(const PossibleMove &move, PlayerColor player);

	bool	  createsPin(const PossibleMove &move, PlayerColor player);
	bool	  createsFork(const PossibleMove &move, PlayerColor player);
	bool	  createsSkewer(const PossibleMove &move, PlayerColor player);
	bool	  blocksEnemyThreats(const PossibleMove &move, PlayerColor player);

	int		  getStrategicEvaluation(const PossibleMove &move, PlayerColor player);
	int		  getTacticalEvaluation(const PossibleMove &move, PlayerColor player);

	/**
	 * @brief	Infer phase of game (used to weight heuristics).
	 */
	GamePhase determineGamePhase() const;

	bool	  isPasssedPawn(const Position &pos, PlayerColor player) const;
	bool	  isIsolatedPawn(const Position &pos, PlayerColor player) const;
	bool	  isDoublePawn(const Position &pos, PlayerColor player) const;
	bool	  isInCenter(const Position &pos) const;


private:
	struct ThreatAnalysis
	{
		std::vector<Position> threatenedPieces;
		int					  kingThreats = 0;

		ThreatAnalysis(const std::vector<Position> &threats, const Position &kingPos)
		{
			threatenedPieces = threats;
			for (const auto &threat : threats)
			{
				if (threat == kingPos)
					kingThreats++;
			}
		}
	};

	ThreatAnalysis		  calculateCurrentThreats(PlayerColor opponent, PlayerColor player);
	ThreatAnalysis		  calculateThreatsAfterMove(const PossibleMove &move, PlayerColor player, PlayerColor opponent);
	bool				  analyzeThreatReduction(const ThreatAnalysis &before, const ThreatAnalysis &after, const Position &ourKing, const PossibleMove &move, PlayerColor player);
	bool				  physicallyBlocksAttack(const PossibleMove &move, PlayerColor player, Chessboard &board);
	std::vector<Position> calculateThreatsOnBoard(PlayerColor opponent, PlayerColor player, Chessboard &board);
	std::vector<Position> calculateThreatsOnBoard(PlayerColor opponent, PlayerColor player);

	int					  calculateMobility(PlayerColor player) const;
	int					  calculateKingSafetyScore(PlayerColor player) const;
	int					  calculatePawnStructureScore(PlayerColor player) const;
	bool				  isNearKing(const Position &pos, const Position &kingPos) const;
	std::vector<Position> getAttackedSquares(const Position &piecePos, PlayerColor player) const;
	bool				  wouldExposeKing(const PossibleMove &move, PlayerColor player) const;
	int					  countAttackers(const Position &target, PlayerColor attackerPlayer) const;

	bool				  isDevelopmentMove(const PossibleMove &move, PlayerColor player) const;
	bool				  isPrematureQueenMove(const PossibleMove &move, PlayerColor player) const;
	bool				  controlsCenterSquares(const PossibleMove &move, PlayerColor player) const;

	PlayerColor			  getOpponentColor(PlayerColor player) const;
	bool				  areCollinear(const Position &pos1, const Position &pos2, PieceType pieceType);

	//PieceType			  getPieceTypeFromPosition(const Position &pos) const;
	PlayerColor			  getPieceColorFromPosition(const Position &pos) const;


	Chessboard&		mBoard;
	std::shared_ptr<MoveGeneration> mGeneration;
};

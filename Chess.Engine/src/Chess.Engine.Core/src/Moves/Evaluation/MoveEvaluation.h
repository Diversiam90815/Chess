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
#include "LightChessBoard.h"


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
	MoveEvaluation(std::shared_ptr<ChessBoard> chessboard, std::shared_ptr<MoveGeneration> generation);
	~MoveEvaluation() = default;

	/**
	 * @brief	Fast evaluation (material + immediate heuristics).
	 * @param	move -> Possible move to be evaluated
	 * @return	score value
	 */
	int		  getBasicEvaluation(const PossibleMove &move);

	/**
	 * @brief	Intermediate evaluation including positional aspects.
	 * @param	move -> Possible move to be evaluated
	 * @param	player -> our player color
	 * @param	lightBoard -> optional reference to a LightChessBoard instance for faster calculation (deprecated soon).
	 * @return	score value
	 */
	int		  getMediumEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);

	/**
	 * @brief	Advanced evaluation factoring broader strategic dimensions.
	 * @param	move -> Possible move to be evaluated
	 * @param	player -> our player color
	 * @param	lightBoard -> optional reference to a LightChessBoard instance for faster calculation (deprecated soon).
	 * @return	score value
	 */
	int		  getAdvancedEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);

	int		  getPieceValue(PieceType piece) const;
	int		  getPositionValue(PieceType piece, const Position &pos, PlayerColor player, const LightChessBoard *lightBoard = nullptr) const;
	int		  evaluateMaterialGain(const PossibleMove &move, const LightChessBoard *lightBoard = nullptr);
	int		  evaluatePositionalGain(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  evaluateThreatLevel(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  evaluateKingSafety(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  evaluateCenterControl(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  evaluatePawnStructure(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  evaluatePieceActivity(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  evaluateDefensivePatterns(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  evaluateOpeningPrinciples(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);

	bool	  createsPin(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	bool	  createsFork(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	bool	  createsSkewer(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	bool	  blocksEnemyThreats(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);

	int		  getStrategicEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	int		  getTacticalEvaluation(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr);

	/**
	 * @brief	Infer phase of game (used to weight heuristics).
	 */
	GamePhase determineGamePhase(const LightChessBoard *lightBoard = nullptr) const;

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

	ThreatAnalysis		  calculateCurrentThreats(PlayerColor opponent, PlayerColor player, const LightChessBoard *lightBoard = nullptr);
	ThreatAnalysis		  calculateThreatsAfterMove(const PossibleMove &move, PlayerColor player, PlayerColor opponent, const LightChessBoard *lightBoard = nullptr);
	bool				  analyzeThreatReduction(const ThreatAnalysis &before, const ThreatAnalysis &after, const Position &ourKing, const PossibleMove &move, PlayerColor player);
	bool				  physicallyBlocksAttack(const PossibleMove &move, PlayerColor player, ChessBoard &board);
	std::vector<Position> calculateThreatsOnBoard(PlayerColor opponent, PlayerColor player, ChessBoard &board);
	std::vector<Position> calculateThreatsOnBoard(PlayerColor opponent, PlayerColor player, LightChessBoard &board);

	int					  calculateMobility(PlayerColor player) const;
	int					  calculateKingSafetyScore(PlayerColor player) const;
	int					  calculatePawnStructureScore(PlayerColor player) const;
	bool				  isNearKing(const Position &pos, const Position &kingPos) const;
	std::vector<Position> getAttackedSquares(const Position &piecePos, PlayerColor player, const LightChessBoard *board = nullptr) const;
	bool				  wouldExposeKing(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr) const;
	int					  countAttackers(const Position &target, PlayerColor attackerPlayer, const LightChessBoard *lightBoard = nullptr) const;

	bool				  isDevelopmentMove(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr) const;
	bool				  isPrematureQueenMove(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr) const;
	bool				  controlsCenterSquares(const PossibleMove &move, PlayerColor player, const LightChessBoard *lightBoard = nullptr) const;

	PlayerColor			  getOpponentColor(PlayerColor player) const;
	bool				  areCollinear(const Position &pos1, const Position &pos2, PieceType pieceType);

	PieceType			  getPieceTypeFromPosition(const Position &pos, const LightChessBoard *lightBoard = nullptr) const;
	PlayerColor			  getPieceColorFromPosition(const Position &pos, const LightChessBoard *lightBoard = nullptr) const;


	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveGeneration> mGeneration;
};

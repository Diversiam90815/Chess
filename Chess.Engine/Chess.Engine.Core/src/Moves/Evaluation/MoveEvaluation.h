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

#include "Move.h"
#include "ChessBoard.h"
#include "Generation/MoveGeneration.h"


enum class GamePhase
{
	Opening	   = 1,
	MiddleGame = 2,
	EndGame	   = 3
};


class MoveEvaluation
{
public:
	MoveEvaluation(std::shared_ptr<ChessBoard> chessboard, std::shared_ptr<MoveGeneration> generation);
	~MoveEvaluation() = default;

	int					 getBasicEvaluation(const PossibleMove &move);
	int					 getMediumEvaluation(const PossibleMove &move, PlayerColor player);
	int					 getAdvancedEvaluation(const PossibleMove &move, PlayerColor player);

	static constexpr int getPieceValue(PieceType piece);
	int					 getPositionValue(PieceType piece, const Position &pos, PlayerColor player) const;
	int					 evaluateMaterialGain(const PossibleMove &move);
	int					 evaluatePositionalGain(const PossibleMove &move, PlayerColor player);
	int					 evaluateThreatLevel(const PossibleMove &move, PlayerColor player);
	int					 evaluateKingSafety(const PossibleMove &move, PlayerColor player);
	int					 evaluateCenterControl(const PossibleMove &move, PlayerColor player);
	int					 evaluatePawnStructure(const PossibleMove &move, PlayerColor player);
	int					 evaluatePieceActivity(const PossibleMove &move, PlayerColor player);
	int					 evaluateDefensivePatterns(const PossibleMove &move, PlayerColor player);

	bool				 createsPin(const PossibleMove &move, PlayerColor player);
	bool				 createsFork(const PossibleMove &move, PlayerColor player);
	bool				 createsSkewer(const PossibleMove &move, PlayerColor player);
	bool				 blocksEnemyThreats(const PossibleMove &move, PlayerColor player);

	int					 getStrategicEvaluation(const PossibleMove &move, PlayerColor player);
	int					 getTacticalEvaluation(const PossibleMove &move, PlayerColor player);

	GamePhase			 determineGamePhase() const;


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
	bool				  physicallyBlocksAttack(const PossibleMove &move, PlayerColor player, ChessBoard &board);
	std::vector<Position> calculateThreatsOnBoard(ChessBoard &board, PlayerColor opponent, PlayerColor player);


	int					  calculateMobility(PlayerColor player) const;
	int					  calculateKingSafetyScore(PlayerColor player) const;
	int					  calculatePawnStructureScore(PlayerColor player) const;
	bool				  isPasssedPawn(const Position &pos, PlayerColor player) const;
	bool				  isIsolatedPawn(const Position &pos, PlayerColor player) const;
	bool				  isDoublePawn(const Position &pos, PlayerColor player) const;
	bool				  isInCenter(const Position &pos) const;
	bool				  isNearKing(const Position &pos, const Position &kingPos) const;
	std::vector<Position> getAttackedSquares(const Position &piecePos, PlayerColor player) const;
	bool				  wouldExposeKing(const PossibleMove &move, PlayerColor player) const;
	int					  countAttackers(const Position &target, PlayerColor attackerPlayer) const;
	PlayerColor			  getOpponentColor(PlayerColor player) const;

	bool				  areCollinear(const Position &pos1, const Position &pos2, PieceType pieceType);

	std::shared_ptr<ChessBoard>						   mBoard;
	std::shared_ptr<MoveGeneration>					   mGeneration;

	static constexpr int							   PAWN_VALUE			   = 100;
	static constexpr int							   KNIGHT_VALUE			   = 320;
	static constexpr int							   BISHOP_VALUE			   = 330;
	static constexpr int							   ROOK_VALUE			   = 500;
	static constexpr int							   QUEEN_VALUE			   = 900;

	static constexpr int							   CHECKMATE_BONUS		   = 10000;
	static constexpr int							   CHECK_BONUS			   = 50;
	static constexpr int							   CAPTURE_BONUS		   = 25;
	static constexpr int							   CASTLE_BONUS			   = 60;
	static constexpr int							   PROMOTION_BONUS		   = 800;
	static constexpr int							   CENTER_CONTROL_BONUS	   = 15;
	static constexpr int							   KING_SAFETY_WEIGHT	   = 3;
	static constexpr int							   MOBILITY_WEIGHT		   = 2;
	static constexpr int							   THREAT_WEIGHT		   = 4;

	static constexpr int							   FORK_BONUS			   = 150;
	static constexpr int							   PIN_BONUS			   = 100;
	static constexpr int							   SKEWER_BONUS			   = 120;
	static constexpr int							   DISCOVERED_ATTACK_BONUS = 80;
	static constexpr int							   THREAT_BLOCK_BONUS	   = 60;

	// Position value tables (from white's perspective, flip for black)
	static constexpr std::array<std::array<int, 8>, 8> PAWN_TABLE_MG		   = {{{0, 0, 0, 0, 0, 0, 0, 0},
																				   {50, 50, 50, 50, 50, 50, 50, 50},
																				   {10, 10, 20, 30, 30, 20, 10, 10},
																				   {5, 5, 10, 27, 27, 10, 5, 5},
																				   {0, 0, 0, 25, 25, 0, 0, 0},
																				   {5, -5, -10, 0, 0, -10, -5, 5},
																				   {5, 10, 10, -25, -25, 10, 10, 5},
																				   {0, 0, 0, 0, 0, 0, 0, 0}}};

	// Endgame tables (king activity is more important)
	static constexpr std::array<std::array<int, 8>, 8> PAWN_TABLE_EG		   = {{{0, 0, 0, 0, 0, 0, 0, 0},
																				   {80, 80, 80, 80, 80, 80, 80, 80},
																				   {50, 50, 50, 50, 50, 50, 50, 50},
																				   {30, 30, 30, 30, 30, 30, 30, 30},
																				   {20, 20, 20, 20, 20, 20, 20, 20},
																				   {10, 10, 10, 10, 10, 10, 10, 10},
																				   {10, 10, 10, 10, 10, 10, 10, 10},
																				   {0, 0, 0, 0, 0, 0, 0, 0}}};

	static constexpr std::array<std::array<int, 8>, 8> KNIGHT_TABLE_MG		   = {{{-50, -40, -30, -30, -30, -30, -40, -50},
																				   {-40, -20, 0, 0, 0, 0, -20, -40},
																				   {-30, 0, 10, 15, 15, 10, 0, -30},
																				   {-30, 5, 15, 20, 20, 15, 5, -30},
																				   {-30, 0, 15, 20, 20, 15, 0, -30},
																				   {-30, 5, 10, 15, 15, 10, 5, -30},
																				   {-40, -20, 0, 5, 5, 0, -20, -40},
																				   {-50, -40, -30, -30, -30, -30, -40, -50}}};

	static constexpr std::array<std::array<int, 8>, 8> KNIGHT_TABLE_EG		   = {{{-50, -40, -30, -30, -30, -30, -40, -50},
																				   {-40, -20, 0, 5, 5, 0, -20, -40},
																				   {-30, 0, 10, 15, 15, 10, 0, -30},
																				   {-30, 5, 15, 20, 20, 15, 5, -30},
																				   {-30, 0, 15, 20, 20, 15, 0, -30},
																				   {-30, 5, 10, 15, 15, 10, 5, -30},
																				   {-40, -20, 0, 5, 5, 0, -20, -40},
																				   {-50, -40, -30, -30, -30, -30, -40, -50}}};

	static constexpr std::array<std::array<int, 8>, 8> BISHOP_TABLE			   = {{{-20, -10, -10, -10, -10, -10, -10, -20},
																				   {-10, 0, 0, 0, 0, 0, 0, -10},
																				   {-10, 0, 5, 10, 10, 5, 0, -10},
																				   {-10, 5, 5, 10, 10, 5, 5, -10},
																				   {-10, 0, 10, 10, 10, 10, 0, -10},
																				   {-10, 10, 10, 10, 10, 10, 10, -10},
																				   {-10, 5, 0, 0, 0, 0, 5, -10},
																				   {-20, -10, -10, -10, -10, -10, -10, -20}}};

	static constexpr std::array<std::array<int, 8>, 8> ROOK_TABLE			   = {{{0, 0, 0, 0, 0, 0, 0, 0},
																				   {5, 10, 10, 10, 10, 10, 10, 5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {-5, 0, 0, 0, 0, 0, 0, -5},
																				   {0, 0, 0, 5, 5, 0, 0, 0}}};

	static constexpr std::array<std::array<int, 8>, 8> QUEEN_TABLE			   = {{{-20, -10, -10, -5, -5, -10, -10, -20},
																				   {-10, 0, 0, 0, 0, 0, 0, -10},
																				   {-10, 0, 5, 5, 5, 5, 0, -10},
																				   {-5, 0, 5, 5, 5, 5, 0, -5},
																				   {0, 0, 5, 5, 5, 5, 0, -5},
																				   {-10, 5, 5, 5, 5, 5, 0, -10},
																				   {-10, 0, 5, 0, 0, 0, 0, -10},
																				   {-20, -10, -10, -5, -5, -10, -10, -20}}};

	static constexpr std::array<std::array<int, 8>, 8> KING_TABLE_MG		   = {{{-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-30, -40, -40, -50, -50, -40, -40, -30},
																				   {-20, -30, -30, -40, -40, -30, -30, -20},
																				   {-10, -20, -20, -20, -20, -20, -20, -10},
																				   {20, 20, 0, 0, 0, 0, 20, 20},
																				   {20, 30, 10, 0, 0, 10, 30, 20}}};

	static constexpr std::array<std::array<int, 8>, 8> KING_TABLE_EG		   = {{{-50, -40, -30, -20, -20, -30, -40, -50},
																				   {-30, -20, -10, 0, 0, -10, -20, -30},
																				   {-30, -10, 20, 30, 30, 20, -10, -30},
																				   {-30, -10, 30, 40, 40, 30, -10, -30},
																				   {-30, -10, 30, 40, 40, 30, -10, -30},
																				   {-30, -10, 20, 30, 30, 20, -10, -30},
																				   {-30, -30, 0, 0, 0, 0, -30, -30},
																				   {-50, -30, -30, -30, -30, -30, -30, -50}}};
};

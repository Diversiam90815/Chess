/*
  ==============================================================================
	Module:         MoveNotation
	Description:    Transforming the Move class into a Standart Algebraic Notation string
  ==============================================================================
*/

#pragma once

#include <string>

#include "Move.h"
#include "MoveType.h"


/**
 * @brief Utility for generating and formatting move notation strings.
 */
class MoveNotation
{
public:
	MoveNotation();
	~MoveNotation();

	/**
	 * @brief Produce SAN (Standard Algebraic Notation) for a move.
	 */
	std::string generateStandardAlgebraicNotation(Move &move);

	/**
	 * @brief Convert a castling move into its SAN form (O-O / O-O-O).
	 */
	std::string castlingToSAN(Move &move);

	/**
	 * @brief Get coordinate string form (e.g. "e4") for a position.
	 */
	std::string getPositionString(Position &pos);

	/**
	 * @brief File letter (a-h) from position.
	 */
	char		getFileFromPosition(Position &pos);

	/**
	 * @brief Rank digit (1-8) from position.
	 */
	char		getRankFromPosition(Position &pos);

	/**
	 * @brief Character representing piece type (uppercase except pawn).
	 */
	char		getPieceType(PieceType type);
};
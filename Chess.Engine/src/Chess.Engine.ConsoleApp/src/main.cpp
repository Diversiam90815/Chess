/*
  ==============================================================================
	Module:         ConsoleApp - main
	Description:    Console App for the Chess Engine for debugging
  ==============================================================================
*/

#include <iostream>

#include "Bitboard.h"


static void printBitboard(U64 bitboard)
{
	printf("\n");

	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 8; file++)
		{
			// convert file & rank into square index
			int square = rank * 8 + file;

			// print ranks
			if (!file)
				printf("  %d ", 8 - rank);

			// print bit state (either 1 or 0)
			printf(" %d", BitUtils::getBit(bitboard, square) ? 1 : 0);
		}

		printf("\n");
	}

	printf("\n     a b c d e f g h\n\n");
	printf("     Bitboard: %llud\n\n", bitboard);
}



int main()
{
	std::cout << "Console app starting..\n";

	AttackTables *attackTables = new AttackTables();

	attackTables->initLeaperAttacks();
	attackTables->initSliderAttacks(0);
	attackTables->initSliderAttacks(1);

	U64 occupancy = 0ULL;

	BitUtils::setBit(occupancy, c5);
	BitUtils::setBit(occupancy, f3);
	BitUtils::setBit(occupancy, g7);
	BitUtils::setBit(occupancy, d3);

	printBitboard(occupancy);

	printBitboard(attackTables->getBishopAttacks(d4, occupancy));

	printBitboard(attackTables->getRookAttacks(d5, occupancy));

	delete attackTables;

	std::cout << "Done.\n";
	return 0;
}

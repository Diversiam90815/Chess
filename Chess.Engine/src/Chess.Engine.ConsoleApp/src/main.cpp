/*
  ==============================================================================
	Module:         ConsoleApp - main
	Description:    Console App for the Chess Engine for debugging
  ==============================================================================
*/

#include <iostream>

#include "Bitboard.h"
#include "MoveHelper.h"


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
			printf(" %d", get_bit(bitboard, square) ? 1 : 0);
		}

		printf("\n");
	}

	printf("\n     a b c d e f g h\n\n");
	printf("     Bitboard: %llud\n\n", bitboard);
}



int main()
{
	std::cout << "Console app starting..\n";

	MoveHelper helper{};
	helper.initLeaperAttacks();

	U64 blocker = 0ULL;
	set_bit(blocker, e4);
	set_bit(blocker, f7);
	set_bit(blocker, e5);

	printBitboard(blocker);

	printf("Bit count: %d \n", countBits(blocker));

	printf("Index: %d\n", getLeastSignificantFirstBitIndex(blocker));
	printf("Coordinate: %s\n", square_to_coordinates[getLeastSignificantFirstBitIndex(blocker)]);

	// Test bitboard
	U64 test = 0ULL;
	set_bit(test, getLeastSignificantFirstBitIndex(blocker));
	printBitboard(test);

	std::cout << "Done.\n";
	return 0;
}

#include <iostream>

#include "Bitboard.h"
#include "MoveHelper.h"


void printBitboard(U64 bitboard)
{
	printf("\n");

	// loop over board ranks
	for (int rank = 0; rank < 8; rank++)
	{
		// loop over board files
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

		// print new line every rank
		printf("\n");
	}

	// print board files
	printf("\n     a b c d e f g h\n\n");

	// print bitboard as unsigned decimal number
	printf("     Bitboard: %llud\n\n", bitboard);
}



int main()
{
    std::cout << "Console app starting..\n";
	
	MoveHelper helper;
	helper.initLeaperAttacks();

	for (int i = 0; i <64; ++i)
		printBitboard(helper.maskKingAttacks(i));

    std::cout << "Done.";
    return 0;
}

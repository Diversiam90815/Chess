/*
  ==============================================================================
	Module:         ConsoleApp - main
	Description:    Console App for the Chess Engine for debugging
  ==============================================================================
*/

#include <iostream>

#include "Bitboard.h"
#include "BitboardHelper.h"


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

void printBoard()
{
	Bitboard board;
	board.init();

	printf("\n"); // print offset

	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			// init square
			int square = rank * 8 + file;

			// print ranks
			if (!file)
				printf(" %d ", 8 - rank);

			// define piece variable
			int piece = -1;

			// loop over all piece bitboards
			for (int bbPiece = 0; bbPiece < 12; bbPiece++)
			{
				if (BitUtils::getBit(board.mBitBoards[bbPiece], square))
					piece = bbPiece;
			}

// print different piece set depending on OS
#ifdef WIN32
			printf(" %c", (piece == -1) ? '.' : asciiPieces[piece]);
#else
			printf(" %s", (piece == -1) ? "." : unicodePieces[piece]);
#endif
		}

		printf("\n");					 // print new line every rank
	}

	printf("\n    a b c d e f g h\n\n"); // print board files

	printf("   Side:       %s\n", board.side == Side::White ? "White" : board.side == Side::Black ? "Black" : "--");

	printf("   Enpassant:     %s\n", (board.enPassant != no_square) ? square_to_coordinates[board.enPassant] : "no");

	printf("   Castling:    %c%c%c%c\n\n", (board.castle & wk) ? 'K' : '-', (board.castle & wq) ? 'K' : '-', (board.castle & wk) ? 'Q' : '-', (board.castle & bk) ? 'k' : '-',
		   (board.castle & bq) ? 'q' : '-');

}



int main()
{
	std::cout << "Console app starting..\n";

	printBoard();

	std::cout << "Done.\n";
	return 0;
}

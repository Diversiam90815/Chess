/*
  ==============================================================================
	Module:         ConsoleApp - main
	Description:    Console App for the Chess Engine for debugging
  ==============================================================================
*/

#include <iostream>

#include "Chessboard.h"
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


static void printBoard(Chessboard &board)
{
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
				if (BitUtils::getBit(board.pieces()[bbPiece], square))
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

	printf("   Side:       %s\n", board.getCurrentSide() == Side::White ? "White" : board.getCurrentSide() == Side::Black ? "Black" : "--");

	printf("   Enpassant:     %s\n", (board.getCurrentEnPassantSqaure() != no_square) ? square_to_coordinates[board.getCurrentEnPassantSqaure()] : "no");

	printf("   Castling:    %c%c%c%c\n\n", static_cast<int>(board.getCurrentCastlingRights() & Castling::WK) ? 'K' : '-',
		   static_cast<int>(board.getCurrentCastlingRights() & Castling::WQ) ? 'Q' : '-', static_cast<int>(board.getCurrentCastlingRights() & Castling::BK) ? 'k' : '-',
		   static_cast<int>(board.getCurrentCastlingRights() & Castling::BQ) ? 'q' : '-');
}


static void printAttackedSquares(Chessboard &board, Side side)
{
	printf("\n");

	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;

			if (!file)
				printf("  %d ", 8 - rank);

			// check whether the current square is attacked
			bool squareAttacked = board.isSquareAttacked(square, side);
			printf(" %d", squareAttacked ? 1 : 0);
		}

		printf("\n");
	}

	printf("\n     a b c d e f g h\n\n");
}



int main()
{
	std::cout << "Console app starting..\n";

	Chessboard *board = new Chessboard();

	board->init();

	printAttackedSquares(*board, Side::White);

	delete board;

	std::cout << "Done.\n";
	return 0;
}

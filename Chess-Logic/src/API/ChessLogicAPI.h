/*
/*
  ==============================================================================

	Class:          ChessLogicAPI

	Description:    Plain C API for the Chess logic

  ==============================================================================
*/


#include "ChessLogicAPIDefines.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


	CHESS_API void	Init();

	CHESS_API void	Deinit();


	CHESS_API void	SetDelegate(PFN_CALLBACK pDelegate);

	CHESS_API float GetWindowScalingFactor(HWND hwnd);

	CHESS_API void	SetUnvirtualizedAppDataPath(const char* appDataPath);


	CHESS_API int	GetNumPossibleMoves();

	CHESS_API bool	GetPossibleMoveAtIndex(int index, PossibleMoveInstance *possibleMoveInstance);

	CHESS_API void	ExecuteMove(const PossibleMoveInstance &moveInstance);

	CHESS_API void	ChangeMoveState(int moveState);

	CHESS_API void	HandleMoveStateChanged(const PossibleMoveInstance &moveInstance);

	// CHESS_API bool GetChessBoard(ChessBoard &board);

	CHESS_API void	GetPieceInPosition(PositionInstance posInstance, PieceTypeInstance *pieceTypeInstance);

	CHESS_API bool	GetBoardState(int *boardState);


	CHESS_API void	StartGame();

	CHESS_API void	EndGame();

	CHESS_API void	ResetGame();


#ifdef __cplusplus
}
#endif // __cplusplus

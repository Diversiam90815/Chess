/*
  ==============================================================================
	Module:         ChessLogicAPI
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

	CHESS_API void	SetUnvirtualizedAppDataPath(const char *appDataPath);


	CHESS_API int	GetNumPossibleMoves();

	CHESS_API bool	GetPossibleMoveAtIndex(int index, PossibleMoveInstance *possibleMoveInstance);

	CHESS_API void	ChangeMoveState(int moveState);

	CHESS_API void	HandleMoveStateChanged(const PossibleMoveInstance &moveInstance);


	CHESS_API void	OnSquareSelected(PositionInstance positionInstance);

	CHESS_API void	OnPawnPromotionChosen(PieceTypeInstance promotionInstance);


	CHESS_API void	StartGame();

	CHESS_API void	ResetGame();

	CHESS_API void	UndoMove();

	CHESS_API int	GetEndgameState();


	CHESS_API void	LogInfoWithCaller(const char *message, const char *method, const char *className, const int lineNumber);

	CHESS_API void	LogErrorWithCaller(const char *message, const char *method, const char *className, const int lineNumber);

	CHESS_API void	LogWarningWithCaller(const char *message, const char *method, const char *className, const int lineNumber);


	CHESS_API void	SetCurrentBoardTheme(const char *theme);

	CHESS_API char *GetCurrentBoardTheme();

	CHESS_API void	SetCurrentPieceTheme(const char *theme);

	CHESS_API char *GetCurrentPieceTheme();


	CHESS_API void	SetLocalPlayerName(const char *name);

	CHESS_API char *GetRemotePlayerName();

	CHESS_API int	GetNetworkAdapterCount();

	CHESS_API bool	GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter);

	CHESS_API int	GetSavedAdapterID();

	CHESS_API bool	ChangeCurrentAdapter(int ID);


#ifdef __cplusplus
}
#endif // __cplusplus

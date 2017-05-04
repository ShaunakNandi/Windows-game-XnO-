// XnO.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "XnO.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_XNO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XNO)); //for keyboard hotkeys

    MSG msg;

    // Main message loop: messages from the msg queue
    while (GetMessage(&msg, nullptr, 0, 0)) //infinite loop that begins when win is created and ends only on quit
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;  //receives msg from msg queue
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance; //current instance running for a particular fn
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XNO)); //icon for app
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_XNO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, //maximise, minimise, width, ht of window is stored here
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return

//global variables for the code
const int CELL_SIZE = 100; //pixels
HBRUSH hbrush1, hbrush2;	//just giving two colors for each player
int playerTurn = 1;	//can be 1 or 2 only
int gameBoard[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};	//hold memory weather or not that cell was clicked
int winner = 0;
int wins[3];

BOOL GetGameBoardRect(HWND hwnd, RECT *pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc))
	{
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;


		pRect -> left = (width - CELL_SIZE * 3) / 2;
		pRect -> top = (height - CELL_SIZE * 3) / 2;

		pRect -> right = pRect -> left + CELL_SIZE * 3;
		pRect -> bottom = pRect -> top + CELL_SIZE * 3;
		return TRUE;	
	}

	SetRectEmpty(pRect);
	return FALSE;
}

//function to draw lines
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int GetCellNumberFromPoint(HWND hwnd, int x, int y)
{
	RECT rc;
	POINT pt;
	pt.x = x;
	pt.y = y;

	if (GetGameBoardRect(hwnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			//user clicked inside the board on some cell
			//normalise (0 to 3*CELL_SIZE)
			x = pt.x - rc.left; //using left edge as ref for location of mouse
			y = pt.y - rc.top;

			//finding col and row numbers
			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			//convert to index (0 to 8)
			return column + row * 3;

		}
	}
	return -1; //mouse clicked outside board etc
}

BOOL GetCellRect(HWND hWnd, int index, RECT * pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);
	if (index < 0 || index > 8)
		return FALSE;

	if (GetGameBoardRect(hWnd, &rcBoard))
	{
		//convert index(0 to 8) into (row, col)
		int y = index / 3;	//row no.
		int x = index % 3;	//col no.

		pRect->left = rcBoard.left + x*CELL_SIZE +1;
		pRect->top = rcBoard.top + y*CELL_SIZE +1;
		pRect->right = pRect -> left + CELL_SIZE -1;
		pRect->bottom = pRect -> top + CELL_SIZE -1;

		return TRUE;
	}

	return FALSE;
}


/*
Return 
0 - No Winner
1 - player wins
2 - player wins
3 - it's a draw
*/

/*
 0,1,2
 3,4,5,
 6,7,8,
 */

int GetWinner(int wins[3])
{
	int cells[] = { 0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7, 2,5,8, 0,4,8, 2,4,6 };

	//check winner
	for (int i = 0; i < ARRAYSIZE(cells); i += 3)
	{
		if ((0 != gameBoard[cells[i]]) && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
		{
			//we have a winner
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}
	}

	//check if we have any cells left empty
	for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
		if (0 == gameBoard[i])
			return 0;	//continue to play

	return 3;	//It's a draw
}

//function to display which player's turn is impending at the bottom centre of client screen
void ShowTurn(HWND hWnd, HDC hdc)
{
	RECT rc;
	const WCHAR szTurn1[] = L"Player 1";
	const WCHAR szTurn2[] = L"Player 2";
	const WCHAR * pszTurnText = NULL;
	

	switch (winner)
	{
		case 0:	//continue to play
			pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
			break;
		case 1:	//player 1 wins
			pszTurnText = L"Player 1 is the winner!";
			break;
		case 2:	//player 2 wins
			pszTurnText = L"Player 2 is the winner!";
			break;
		case 3:	//it's a draw
			pszTurnText = L"It's a draw!";
			break;
	}
	if (NULL != pszTurnText && GetClientRect(hWnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
		hbrush1 = CreateSolidBrush(RGB(255, 0, 0));
		hbrush2 = CreateSolidBrush(RGB(0, 255, 0));
		}
    case WM_COMMAND: //for exiting app ie. destroying main window
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEWGAME:
				{
					int ret = MessageBox(hWnd, L"Are you sure you want to start a new game?",
						L"New Game", MB_YESNO | MB_ICONQUESTION);
					if (IDYES == ret)
						{
							//reset and start a new game
							playerTurn = 1;
							winner = 0; //reset winner to 'no winner'
							ZeroMemory(gameBoard, sizeof(gameBoard));	//clear the gameboard to all 0s so that mouse can again be clicked
							//force  a paint message
							InvalidateRect(hWnd, NULL, TRUE);	//post WM_PAINT to our windowProc. It gets queued in our reg Queue
							UpdateWindow(hWnd);		//forces immediate handling of WM_PAINT thus finally rendering a new board on confirming "YES"
						}
				}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	//detect a mouse click whether inside the board or not
	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			if (playerTurn == 0)
				break;	//handle clicks only when either player has a move
			int index = GetCellNumberFromPoint(hWnd, xPos, yPos);	//indexing the cells in the board to detect where the mouse was clicked
			
			HDC hdc = GetDC(hWnd);
			if (NULL != hdc)
			{
				/*
				WCHAR temp[100];
				wsprintf(temp, L"Inddex = %d", index);
				TextOut(hdc, xPos, yPos, temp, lstrlen(temp));
				*/
				
				//get cell dimension fom its index
				if (index != -1)
				{
					RECT rcCell;
					if ((gameBoard[index] == 0) && GetCellRect(hWnd, index, &rcCell))	//continue to mark that cell if it was not previously taken
					{
						
						gameBoard[index] = playerTurn;	//reserves cell for that player
						FillRect(hdc, &rcCell, (playerTurn ==1) ? hbrush1: hbrush2);

						//check for a winner
						winner = GetWinner(wins);
						if (winner == 1 || winner == 2)
						{
							//we have a winner
							MessageBox(hWnd, (winner == 1) ? L"Player 1 is the winner!" : L"Player 2 is the winner!",
								L"You win!!",
								MB_OK | MB_ICONINFORMATION);
							playerTurn = 0;
						}

						else if (winner == 3)
						{
							//It's  a draw
							MessageBox(hWnd, 
								L"No one wins this time!",
								L"It's a draw!",
								MB_OK | MB_ICONEXCLAMATION);
							playerTurn = 0;
						}

						else if (winner == 0)
						{
							playerTurn = (playerTurn == 1) ? 2 : 1;
						}
						//switch between each player's turn
						//playerTurn = (playerTurn == 1) ? 2 : 1;	//should be kept inside, prevents switch until a fresh cell is clicked

						ShowTurn(hWnd, hdc);	//display impending player's turn
					}

					
				}				
				ReleaseDC(hWnd, hdc);
			}

		}
		break;
	case WM_GETMINMAXINFO:	//f1 for help on built in functions. this case is to impose a min size for the board with some padding around the board
		{
			MINMAXINFO * pMinMax = (MINMAXINFO*)lParam;
			
			pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;	//padding
			pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
		}
		break;
    case WM_PAINT: //drawing the app. main work is done here
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc;

			//BOOL GetGameBoardRect(HWND hwnd, RECT *pRect)
            // TODO: Add any drawing code that uses hdc here...

			//Rectangle(hdc, 0, 0, 100, 100); //top-left to bottom right

			if (GetGameBoardRect(hWnd, &rc))
			{
				RECT rcClient;

				if (GetClientRect(hWnd, &rcClient))
				{
					const WCHAR szPlayer1[] = L"Player 1";
					const WCHAR szPlayer2[] = L"Player 2";

					SetBkMode(hdc, TRANSPARENT);	//making the bkgrnd transparent

					//draw player 1 and player 2 text
					SetTextColor(hdc, RGB(0, 255, 255));
					TextOut(hdc, 16, 16, szPlayer1, ARRAYSIZE(szPlayer1));
					SetTextColor(hdc, RGB(0, 0, 255));
					TextOut(hdc, rcClient.right - 72, 16, szPlayer2, ARRAYSIZE(szPlayer2));
					FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));  //no black border around the board
					//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

					ShowTurn(hWnd, hdc);	//display impending player's turn
				}

				for (int i = 0; i < 4; i++)
				{
					//drawing the vert lines in the board
					DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);

					//drawing the hori lines
					DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
				}

				//retain status of all occupied cells even after board is resized
				RECT rcCell;
				for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
				{
					if ((0 != gameBoard[i]) && GetCellRect(hWnd, i, &rcCell))	//continue to mark that cell if it was not previously taken
					{
						FillRect(hdc, &rcCell, (gameBoard[i] == 2) ? hbrush2 : hbrush1);
					}
				}
			}
			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:  //exits the infinite loop
		DeleteObject(hbrush1);
		DeleteObject(hbrush2);
        PostQuitMessage(0); //msg posted into msg queue
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

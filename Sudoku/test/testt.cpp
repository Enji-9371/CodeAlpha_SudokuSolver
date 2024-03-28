/*Write a program that solves Sudoku puzzles. Implement
an algorithm to solve puzzles of varying difficulty
levels. You can also create a graphical user interface
(GUI) for user interaction.*/

//Note to the user: To avoid any linking errors, follow these steps before running the project.
        //Right-click on the project in the Solution Explorer and select "Properties."
        //In the properties window, navigate to "Configuration Properties" > "Linker" > "System."
        //In the "SubSystem" dropdown, select "Windows (/SUBSYSTEM:WINDOWS)." > "Ok"

#include <Windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

const int N = 9; // Size of Sudoku grid
const int CELL_SIZE = 40; // Size of each cell in pixels
const int PADDING = 5; // Padding between cells

HWND g_editBoxes[N][N];
HWND g_solveButton;
HWND g_mainWindow;

bool solveSudoku(vector<vector<int>>& grid); // Forward declaration of solveSudoku function

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void SolveSudoku();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register the window class
    const wchar_t* CLASS_NAME = L"SudokuSolverWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    g_mainWindow = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Sudoku Solver",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, N * (CELL_SIZE + PADDING) + 20, N * (CELL_SIZE + PADDING) + 100,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (g_mainWindow == NULL) 
    {
        return 0;
    }

    // Create the grid of edit boxes
    for (int i = 0; i < N; ++i) 
    {
        for (int j = 0; j < N; ++j) 
        {
            g_editBoxes[i][j] = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
                10 + j * (CELL_SIZE + PADDING), 10 + i * (CELL_SIZE + PADDING), CELL_SIZE, CELL_SIZE,
                g_mainWindow, NULL, hInstance, NULL);
        }
    }

    // Create the solve button
    g_solveButton = CreateWindow(L"BUTTON", L"Solve", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        10, N * (CELL_SIZE + PADDING) + 20, 100, 30, g_mainWindow, NULL, hInstance, NULL);

    // Show the window
    ShowWindow(g_mainWindow, nCmdShow);
    UpdateWindow(g_mainWindow);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (lParam == (LPARAM)g_solveButton && HIWORD(wParam) == BN_CLICKED)
        {
            SolveSudoku();
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void SolveSudoku() 
{
    // Retrieve values from edit boxes and solve Sudoku
    std::vector<std::vector<int>> grid(N, std::vector<int>(N, 0));
    for (int i = 0; i < N; ++i) 
    {
        for (int j = 0; j < N; ++j) 
        {
            wchar_t buffer[2];
            GetWindowText(g_editBoxes[i][j], buffer, 2);
            grid[i][j] = _wtoi(buffer);
        }
    }

    // Solve the Sudoku
    if (solveSudoku(grid)) 
    {
        // Display the solution in edit boxes
        for (int i = 0; i < N; ++i) 
        {
            for (int j = 0; j < N; ++j) 
            {
                SetWindowText(g_editBoxes[i][j], to_wstring(grid[i][j]).c_str());
            }
        }
    }
    else {
        MessageBox(g_mainWindow, L"No solution exists.", L"Sudoku Solution", MB_OK);
    }
}

// to check if the number can be placed at the given position
bool isSafe(const vector<vector<int>>& grid, int row, int col, int num) 
{
    // Check if the number is already present in the row or column
    for (int i = 0; i < N; ++i)
    {
        if (grid[row][i] == num || grid[i][col] == num)
            return false;
    }

    // Check if the number is already present in the 3x3 grid
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; ++i) 
    {
        for (int j = 0; j < 3; ++j) 
        {
            if (grid[i + startRow][j + startCol] == num)
                return false;
        }
    }

    return true;
}

// to solve the Sudoku grid
bool solveSudoku(vector<vector<int>>& grid)
{
    int row = -1, col = -1;
    bool isEmpty = true;

    // Find the first empty cell in the grid
    for (int i = 0; i < N; ++i) 
    {
        for (int j = 0; j < N; ++j) 
        {
            if (grid[i][j] == 0) 
            {
                row = i;
                col = j;
                isEmpty = false;
                break;
            }
        }
        if (!isEmpty)
            break;
    }

    // If no empty cell found, the Sudoku is solved
    if (isEmpty)
        return true;

    // Try placing numbers from 1 to 9
    for (int num = 1; num <= 9; ++num) 
    {
        if (isSafe(grid, row, col, num)) 
        {
            grid[row][col] = num;
            // Recursively solve the Sudoku
            if (solveSudoku(grid))
                return true;
            // If the current configuration doesn't lead to a solution, backtrack
            grid[row][col] = 0;
        }
    }
    return false;
}

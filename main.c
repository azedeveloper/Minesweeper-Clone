#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 900
#define SCREEN_TITLE "Minesweeper"

#define GRID_SIZE 10
#define CELL_SIZE 60
#define BORDER_SIZE 2

typedef struct {
  bool hasMine;
  bool isClicked;
  bool hasFlag;
  int mineCount;
  bool hasBeenScored;
} Cell;

// Declare score as a global variable
int score = 90;

bool gameOver;

void initializeGrid(Cell grid[GRID_SIZE][GRID_SIZE], int numMines);
void revealCell(Cell grid[GRID_SIZE][GRID_SIZE], int row, int col);
int numClickedNonMines(Cell grid[GRID_SIZE][GRID_SIZE]);
void revealAllMines(Cell grid[GRID_SIZE][GRID_SIZE]);

void displaySplashScreen() {
  // Clear screen and draw "AZE" text with a large font
  ClearBackground(WHITE);
  DrawText("Game Over", SCREEN_WIDTH / 2 - MeasureText("Game Over", 80) / 2,
           SCREEN_HEIGHT / 2 - 80, 80, RED);

  // Wait for 1-2 seconds
  double startTime = GetTime();
  while ((GetTime() - startTime) < 2.0) {
    // Pause execution and wait for the timer to expire
  }
}
int main() {
  // Initialize a Window.
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);

  // Initialize the game grid with 10 mines
  Cell grid[GRID_SIZE][GRID_SIZE];
  int numMines = 10;
  initializeGrid(grid, numMines);

  // Count the number of non-mine cells on the grid
  int numNonMines = GRID_SIZE * GRID_SIZE - numMines;

  // Center the grid on the screen
  int gridWidth = GRID_SIZE * CELL_SIZE + (GRID_SIZE + 1) * BORDER_SIZE;
  int gridHeight = GRID_SIZE * CELL_SIZE + (GRID_SIZE + 1) * BORDER_SIZE;
  int offsetX = (SCREEN_WIDTH - gridWidth) / 2;
  int offsetY = (SCREEN_HEIGHT - gridHeight) / 2;

  // Initialize the game timer
  float gameTime = 0.0f;

  // Initialize game over and game won flags
  gameOver = false;
  bool gameWon = false;


  // Render loop.
  while (!WindowShouldClose()) {
    // Update

    // Handle mouse input
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !gameOver && !gameWon) {
      int row = (GetMouseY() - offsetY) / CELL_SIZE;
      int col = (GetMouseX() - offsetX) / CELL_SIZE;
      if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        revealCell(grid, row, col);
        if (grid[row][col].hasMine) {
          gameOver = true;
        }
      }
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !gameOver && !gameWon) {
      int row = (GetMouseY() - offsetY) / CELL_SIZE;
      int col = (GetMouseX() - offsetX) / CELL_SIZE;
      if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        if (!grid[row][col].hasFlag) {
          grid[row][col].hasFlag = true;
        } else if(grid[row][col].hasFlag) {
          grid[row][col].hasFlag = false;
        }
      }
    }

    // Handle keyboard input
    if (IsKeyPressed(KEY_R)) {
      initializeGrid(grid, numMines);
      gameTime = 0.0f;
      score = 90;
      gameOver = false;
      gameWon = false;
    }

    // Update the game timer
    if (!gameOver && !gameWon) {
      gameTime += GetFrameTime();
    }

    // Check if the game has been won
    if (!gameOver && !gameWon) {
      int numClickedNonMineCells = numClickedNonMines(grid);
      if (numClickedNonMineCells == numNonMines) {
        gameWon = true;
      }
    }

    // If the game is over or won, reveal all the mines
    if (gameOver || gameWon) {
      revealAllMines(grid);
    }

    // Draw

    BeginDrawing();

    ClearBackground(WHITE);

    // Draw the game grid
    for (int row = 0; row < GRID_SIZE; row++) {
      for (int col = 0; col < GRID_SIZE; col++) {
        int x = offsetX + col * CELL_SIZE + (col + 1) * BORDER_SIZE;
        int y = offsetY + row * CELL_SIZE + (row + 1) * BORDER_SIZE;
        Rectangle rect = {x, y, CELL_SIZE, CELL_SIZE};

        // Draw the cell with different colors depending on its state
        if (grid[row][col].isClicked) {
          if (grid[row][col].hasMine) {
            DrawRectangleRec(rect, RED);
          } else {
            DrawText(TextFormat("%d", grid[row][col].mineCount), x + 10, y + 10,
                     40, BLACK);
          }
        } else {
          DrawRectangleLinesEx(rect, BORDER_SIZE, GRAY);
        }

        // Draw a flag on the cell if it has one
        if (grid[row][col].hasFlag) {
          Vector2 center = {x + CELL_SIZE / 2.0f, y + CELL_SIZE / 2.0f};
          float radius = CELL_SIZE * 0.3f;
          DrawCircleV(center, radius, BLACK);
          DrawTriangle(
              (Vector2){center.x, center.y - 0.5f * radius},
              (Vector2){center.x - 0.7f * radius, center.y + 0.5f * radius},
              (Vector2){center.x + 0.7f * radius, center.y + 0.5f * radius},
              RED);
        }
      }
    }

    // Draw the timer and score
    char timeText[20];
    sprintf(timeText, "Time: %d:%02d", (int)gameTime / 60, (int)gameTime % 60);
    DrawText(timeText, 20, SCREEN_HEIGHT - 50, 40, BLACK);

    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    DrawText(scoreText, 20, SCREEN_HEIGHT - 100, 40, BLACK);

    DrawText("Press R to restart", SCREEN_WIDTH - MeasureText("Press R to restart", 36), SCREEN_HEIGHT - 100, 35, BLACK);
    DrawText("Right Click to place flag", SCREEN_WIDTH - MeasureText("Right Click to place flag", 36) , SCREEN_HEIGHT - 50, 35, BLACK);

    // Draw the game over or game won text if necessary
    if (gameOver) {
      DrawText("Game Over!", SCREEN_WIDTH / 2 - MeasureText("Game Over!", 80) / 2,
               SCREEN_HEIGHT / 2 - 400, 80, BLACK);
    } else if (gameWon) {
      DrawText("You Won!", SCREEN_WIDTH / 2 - MeasureText("You Won!", 80) / 2,
               SCREEN_HEIGHT / 2 - 400, 80, GREEN);
    }

    EndDrawing();
  }

  // Clean up resources
  CloseWindow();

  return 0;
}

void initializeGrid(Cell grid[GRID_SIZE][GRID_SIZE], int numMines) {
  // Reset the score counter
  score = 90;

  // Seed the random number generator
  srand((unsigned int)time(NULL));

  // Initialize all cells to have no mine and not be clicked or flagged
  for (int row = 0; row < GRID_SIZE; row++) {
    for (int col = 0; col < GRID_SIZE; col++) {
      grid[row][col].hasMine = false;
      grid[row][col].isClicked = false;
      grid[row][col].hasFlag = false;
      grid[row][col].mineCount = 0;
      grid[row][col].hasBeenScored =
          false;  // Initialize hasBeenScored to false
    }
  }

 
  int numMinesPlaced = 0;
  while (numMinesPlaced < numMines) {
    int row = rand() % GRID_SIZE;
    int col = rand() % GRID_SIZE;
    if (!grid[row][col].hasMine) {
      grid[row][col].hasMine = true;
      numMinesPlaced++;
    }
  }


  for (int row = 0; row < GRID_SIZE; row++) {
    for (int col = 0; col < GRID_SIZE; col++) {
      if (!grid[row][col].hasMine) {
        int count = 0;
        for (int i = row - 1; i <= row + 1; i++) {
          for (int j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < GRID_SIZE && j >= 0 && j < GRID_SIZE &&
                grid[i][j].hasMine) {
              count++;
            }
          }
        }
        grid[row][col].mineCount = count;
      }
    }
  }
}

void revealCell(Cell grid[GRID_SIZE][GRID_SIZE], int row, int col) {

  if (grid[row][col].isClicked || grid[row][col].hasFlag || gameOver == true) {
    return;
  }


  if (grid[row][col].hasMine) {
    grid[row][col].isClicked = true;
    gameOver = true;
    return;
  }


  grid[row][col].isClicked = true;

  if (!grid[row][col].hasMine && !grid[row][col].hasBeenScored) {
    score--;
    grid[row][col].hasBeenScored = true;
  }


  if (grid[row][col].mineCount > 0) {
    return;
  }


  for (int i = row - 1; i <= row + 1; i++) {
    for (int j = col - 1; j <= col + 1; j++) {
      if (i >= 0 && i < GRID_SIZE && j >= 0 && j < GRID_SIZE) {
        revealCell(grid, i, j);
      }
    }
  }
}

int numClickedNonMines(Cell grid[GRID_SIZE][GRID_SIZE]) {
  int count = 0;
  for (int row = 0; row < GRID_SIZE; row++) {
    for (int col = 0; col < GRID_SIZE; col++) {
      if (grid[row][col].isClicked && !grid[row][col].hasMine) {
        count++;
      }
    }
  }
  return count;
}

void revealAllMines(Cell grid[GRID_SIZE][GRID_SIZE]) {
  for (int row = 0; row < GRID_SIZE; row++) {
    for (int col = 0; col < GRID_SIZE; col++) {
      if (grid[row][col].hasMine) {
        grid[row][col].isClicked = true;
      }
    }
  }
}

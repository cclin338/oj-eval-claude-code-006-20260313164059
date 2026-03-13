#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

// Game state variables
char board[35][35];      // Current visible state of the board
int visited_count;       // Count of visited cells
int marked_count;        // Count of marked cells

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // Initialize all global variables
  visited_count = 0;
  marked_count = 0;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      board[i][j] = '?';
    }
  }

  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  for (int i = 0; i < rows; i++) {
    std::string line;
    std::cin >> line;
    for (int j = 0; j < columns; j++) {
      board[i][j] = line[j];
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};

  // Strategy 1: Auto-explore cells where all mines are marked
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (board[i][j] >= '1' && board[i][j] <= '8') {
        int mine_count = board[i][j] - '0';
        int marked_neighbors = 0;
        int unknown_neighbors = 0;

        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (board[ni][nj] == '@') {
              marked_neighbors++;
            } else if (board[ni][nj] == '?') {
              unknown_neighbors++;
            }
          }
        }

        // If all mines are marked, auto-explore
        if (marked_neighbors == mine_count && unknown_neighbors > 0) {
          Execute(i, j, 2);
          return;
        }
      }
    }
  }

  // Strategy 2: Mark obvious mines
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (board[i][j] >= '1' && board[i][j] <= '8') {
        int mine_count = board[i][j] - '0';
        int marked_neighbors = 0;
        int unknown_neighbors = 0;
        int unknown_r = -1, unknown_c = -1;

        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (board[ni][nj] == '@') {
              marked_neighbors++;
            } else if (board[ni][nj] == '?') {
              unknown_neighbors++;
              unknown_r = ni;
              unknown_c = nj;
            }
          }
        }

        // If remaining unknowns equals remaining mines, mark them all
        if (unknown_neighbors > 0 && marked_neighbors + unknown_neighbors == mine_count) {
          for (int k = 0; k < 8; k++) {
            int ni = i + dr[k];
            int nj = j + dc[k];
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && board[ni][nj] == '?') {
              Execute(ni, nj, 1);
              return;
            }
          }
        }
      }
    }
  }

  // Strategy 3: Visit safe cells (cells with 0 adjacent mines marked and all neighbors accounted for)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (board[i][j] == '0') {
        // Visit any unknown neighbor
        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && board[ni][nj] == '?') {
            Execute(ni, nj, 0);
            return;
          }
        }
      }
    }
  }

  // Strategy 4: Advanced constraint satisfaction
  // Use subset reasoning between adjacent constraint cells
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (board[i][j] >= '1' && board[i][j] <= '8') {
        int mine_count = board[i][j] - '0';

        // Get unknown neighbors for this cell
        int unknowns1[8], u1_count = 0;
        int marked1 = 0;

        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (board[ni][nj] == '?') {
              unknowns1[u1_count++] = ni * columns + nj;
            } else if (board[ni][nj] == '@') {
              marked1++;
            }
          }
        }

        int remaining1 = mine_count - marked1;

        // Compare with adjacent numbered cells
        for (int di = -1; di <= 1; di++) {
          for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di;
            int nj = j + dj;

            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns &&
                board[ni][nj] >= '1' && board[ni][nj] <= '8') {

              int mine_count2 = board[ni][nj] - '0';
              int unknowns2[8], u2_count = 0;
              int marked2 = 0;

              for (int k = 0; k < 8; k++) {
                int nni = ni + dr[k];
                int nnj = nj + dc[k];
                if (nni >= 0 && nni < rows && nnj >= 0 && nnj < columns) {
                  if (board[nni][nnj] == '?') {
                    unknowns2[u2_count++] = nni * columns + nnj;
                  } else if (board[nni][nnj] == '@') {
                    marked2++;
                  }
                }
              }

              int remaining2 = mine_count2 - marked2;

              // Find cells that are in unknowns1 but not in unknowns2
              for (int a = 0; a < u1_count; a++) {
                bool in_u2 = false;
                for (int b = 0; b < u2_count; b++) {
                  if (unknowns1[a] == unknowns2[b]) {
                    in_u2 = true;
                    break;
                  }
                }

                if (!in_u2) {
                  // This cell is in constraint 1 but not in constraint 2
                  // If all of constraint2's unknowns are also in constraint1,
                  // and remaining1 == remaining2, then cells only in constraint1 are safe

                  bool all_u2_in_u1 = true;
                  for (int b = 0; b < u2_count; b++) {
                    bool found = false;
                    for (int c = 0; c < u1_count; c++) {
                      if (unknowns2[b] == unknowns1[c]) {
                        found = true;
                        break;
                      }
                    }
                    if (!found) {
                      all_u2_in_u1 = false;
                      break;
                    }
                  }

                  if (all_u2_in_u1 && remaining1 == remaining2 && u2_count > 0) {
                    // Cells only in u1 are safe
                    int r = unknowns1[a] / columns;
                    int c = unknowns1[a] % columns;
                    Execute(r, c, 0);
                    return;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // Strategy 5: Probability-based guessing
  // Calculate probability for each unknown cell
  double best_prob = 2.0; // Higher is worse (probability of being a mine)
  int best_r = -1, best_c = -1;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (board[i][j] == '?') {
        // Calculate mine probability for this cell
        double prob_sum = 0.0;
        int constraint_count = 0;

        for (int k = 0; k < 8; k++) {
          int ni = i + dr[k];
          int nj = j + dc[k];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns &&
              board[ni][nj] >= '1' && board[ni][nj] <= '8') {

            int mine_count = board[ni][nj] - '0';
            int marked = 0;
            int unknown = 0;

            for (int m = 0; m < 8; m++) {
              int nni = ni + dr[m];
              int nnj = nj + dc[m];
              if (nni >= 0 && nni < rows && nnj >= 0 && nnj < columns) {
                if (board[nni][nnj] == '@') {
                  marked++;
                } else if (board[nni][nnj] == '?') {
                  unknown++;
                }
              }
            }

            if (unknown > 0) {
              double prob = (double)(mine_count - marked) / unknown;
              if (prob >= 0.0 && prob <= 1.0) {
                prob_sum += prob;
                constraint_count++;
              }
            }
          }
        }

        double avg_prob = constraint_count > 0 ? prob_sum / constraint_count : 0.5;

        // Prefer cells with lower probability
        if (best_r == -1 || avg_prob < best_prob) {
          best_prob = avg_prob;
          best_r = i;
          best_c = j;
        }
      }
    }
  }

  if (best_r != -1) {
    Execute(best_r, best_c, 0);
    return;
  }
}

#endif
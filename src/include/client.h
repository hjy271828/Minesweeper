#ifndef CLIENT_H
#define CLIENT_H

#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

int possible_delta_r[25] = {0, -1, 0, 1, -1, -1, 1, 1, -1, -2, -2, -1, 1, 2, 2, 1, 0, -2, 0, 2};
int possible_delta_c[25] = {1, 0, -1, 0, 1, -1, -1, 1, 2, 1, -1, -2, -2, -1, 1, 2, 2, 0, -2, 0};
std::mt19937 rng(114514);
extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.
int total_seen_mines;
int total_unknown;
std::vector<std::vector<int>> seen_unknown;
std::vector<std::vector<int>> seen_mines;
std::vector<std::vector<char>> current_map;
// You MUST NOT use any other external variables except for rows, columns and total_mines.
/**
 * @brief The definition of function Execute(int, int, int)
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
 * @details This function is designed to initialize the client state. It should be called at the beginning of the game,
 * after InitMap() has read the map scale. It reads and executes the first step provided by the input (see README).
 */
void InitGame() {
  // TODO (student): Initialize all your global variables!
  seen_unknown.assign(rows, std::vector<int>(columns, 0));
  seen_mines.assign(rows, std::vector<int>(columns, 0));
  current_map.assign(rows, std::vector<char>(columns, 0));
  total_seen_mines = 0;
  total_unknown = 0;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      seen_mines[r][c] = 0;
      seen_unknown[r][c] = 0;
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
  // TODO (student): Implement me!
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      std::cin >> current_map[r][c];
      if (current_map[r][c] == '@') {
        total_seen_mines++;
      } else if (current_map[r][c] == '?') {
        total_unknown++;
      }
    }
  }
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      int seen_mines_r_c = 0;
      int seen_unknown_r_c = 0;
      for (int near_row = r - 1; near_row <= r + 1; ++near_row) {
        for (int near_column = c - 1; near_column <= c + 1; ++near_column) {
          if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1) {
            if (current_map[near_row][near_column] == '@') {
              seen_mines_r_c++;
            } else if (current_map[near_row][near_column] == '?') {
              seen_unknown_r_c++;
            }
          }
        }
      }
      seen_mines[r][c] = seen_mines_r_c;
      seen_unknown[r][c] = seen_unknown_r_c;
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
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (current_map[r][c] != '@' && current_map[r][c] != '?') {
        if (seen_mines[r][c] == (current_map[r][c] - '0') && seen_unknown[r][c] != 0) {
          Execute(r, c, 2);
          return;
        }
        if (seen_unknown[r][c] + seen_mines[r][c] == (current_map[r][c] - '0' && seen_unknown[r][c] != 0)) {
          for (int near_row = r - 1; near_row <= r + 1; ++near_row) {
            for (int near_column = c - 1; near_column <= c + 1; ++near_column) {
              if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1) {
                if (current_map[near_row][near_column] == '?') {
                  Execute(near_row, near_column, 1);
                  return;
                }
              }
            }
          }
        }

        // 找周边的格点，一起判断
        for (int i = 0; i < 19; ++i) {
          int point2_r = possible_delta_r[i] + r;
          int point2_c = possible_delta_c[i] + c;
          if (point2_r >= 0 && point2_r <= rows - 1 && point2_c >= 0 && point2_c <= columns - 1) {
            int unknown_not_in_point2_range = 0;
            int least_unknown_mine_in_point2_range = 0;
            for (int near_row = r - 1; near_row <= r + 1; ++near_row) {
              for (int near_column = c - 1; near_column <= c + 1; ++near_column) {
                if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                    current_map[near_row][near_column] == '?') {
                  if (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) {
                    unknown_not_in_point2_range++;
                  }
                }
              }
            }
            least_unknown_mine_in_point2_range =
                (current_map[r][c] - '0') - seen_mines[r][c] - unknown_not_in_point2_range;
            if (current_map[point2_r][point2_c] - '0' == least_unknown_mine_in_point2_range) {
              for (int near_row = point2_r - 1; near_row <= point2_r + 1; ++near_row) {
                for (int near_column = point2_c - 1; near_column <= point2_c + 1; ++near_column) {
                  if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                      current_map[near_row][near_column] == '?') {
                    if (abs(r - near_row) > 1 || abs(c - near_column) > 1) {
                      Execute(near_row, near_column, 0);
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
  }

  // 随机大法
  int try_this_row = rng() % rows;
  int try_this_column = rng() % columns;
  Execute(try_this_row, try_this_column, 0);
}
/*
两个有重叠地区的格子
*/
#endif

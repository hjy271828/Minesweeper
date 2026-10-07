#ifndef CLIENT_H
#define CLIENT_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

struct point {
  int r;
  int c;
  double mine_probability;
  int no_number_around;
};

bool my_compare(point pt1, point pt2) { return pt1.mine_probability < pt2.mine_probability; }

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
  total_seen_mines = 0;
  total_unknown = 0;
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
  for (int point1_r = 0; point1_r < rows; ++point1_r) {
    for (int point1_c = 0; point1_c < columns; ++point1_c) {
      if (current_map[point1_r][point1_c] != '@' && current_map[point1_r][point1_c] != '?') {
        if (seen_mines[point1_r][point1_c] == (current_map[point1_r][point1_c] - '0') &&
            seen_unknown[point1_r][point1_c] != 0) {
          Execute(point1_r, point1_c, 2);
          return;
        }
        if (seen_unknown[point1_r][point1_c] + seen_mines[point1_r][point1_c] ==
                current_map[point1_r][point1_c] - '0' &&
            seen_unknown[point1_r][point1_c] != 0) {
          for (int near_row = point1_r - 1; near_row <= point1_r + 1; ++near_row) {
            for (int near_column = point1_c - 1; near_column <= point1_c + 1; ++near_column) {
              if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1) {
                if (current_map[near_row][near_column] == '?') {
                  Execute(near_row, near_column, 1);
                  return;
                }
              }
            }
          }
        }

        if ((current_map[point1_r][point1_c] - '0') <= seen_mines[point1_r][point1_c]) continue;
        // 用两个格点信息，一起判断
        for (int i = 0; i <= 19; ++i) {
          int point2_r = possible_delta_r[i] + point1_r;
          int point2_c = possible_delta_c[i] + point1_c;
          if (point2_r >= 0 && point2_r <= rows - 1 && point2_c >= 0 && point2_c <= columns - 1 &&
              current_map[point2_r][point2_c] >= '0' && current_map[point2_r][point2_c] <= '9' &&
              (current_map[point2_r][point2_c] - '0') > seen_mines[point2_r][point2_c]) {
            int unknown_in_p1_range_but_not_in_p2_range = 0;
            int unknown_in_p12_range = 0;
            int least_unseen_mine_in_p12_range = 0;
            for (int near_row = point1_r - 1; near_row <= point1_r + 1; ++near_row) {
              for (int near_column = point1_c - 1; near_column <= point1_c + 1; ++near_column) {
                if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                    current_map[near_row][near_column] == '?') {
                  if (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) {
                    unknown_in_p1_range_but_not_in_p2_range++;
                  } else {
                    unknown_in_p12_range++;
                  }
                }
              }
            }

            least_unseen_mine_in_p12_range = (current_map[point1_r][point1_c] - '0') - seen_mines[point1_r][point1_c] -
                                             unknown_in_p1_range_but_not_in_p2_range;
            if (current_map[point2_r][point2_c] - '0' ==
                least_unseen_mine_in_p12_range + seen_mines[point2_r][point2_c]) {
              for (int near_row = point2_r - 1; near_row <= point2_r + 1; ++near_row) {
                for (int near_column = point2_c - 1; near_column <= point2_c + 1; ++near_column) {
                  if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                      current_map[near_row][near_column] == '?') {
                    if (abs(point1_r - near_row) > 1 || abs(point1_c - near_column) > 1) {
                      Execute(near_row, near_column, 0);
                      return;
                    }
                  }
                }
              }
            }
            int most_unseen_mine_in_p12_range =
                std::min((current_map[point1_r][point1_c] - '0') - seen_mines[point1_r][point1_c],
                         std::min((current_map[point2_r][point2_c] - '0') - seen_mines[point2_r][point2_c],
                                  unknown_in_p12_range));
            if ((current_map[point1_r][point1_c] - '0') - seen_mines[point1_r][point1_c] -
                    most_unseen_mine_in_p12_range >=
                unknown_in_p1_range_but_not_in_p2_range) {
              for (int near_row = point1_r - 1; near_row <= point1_r + 1; ++near_row) {
                for (int near_column = point1_c - 1; near_column <= point1_c + 1; ++near_column) {
                  if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                      current_map[near_row][near_column] == '?') {
                    if (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) {
                      Execute(near_row, near_column, 1);
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

  // 用三个格点信息，一起判断
  for (int point1_r = 0; point1_r < rows; ++point1_r) {
    for (int point1_c = 0; point1_c < columns; ++point1_c) {
      if (current_map[point1_r][point1_c] != '@' && current_map[point1_r][point1_c] != '?') {
        for (int i = 0; i <= 19; ++i) {
          int point2_r = possible_delta_r[i] + point1_r;
          int point2_c = possible_delta_c[i] + point1_c;
          if (point2_r >= 0 && point2_r <= rows - 1 && point2_c >= 0 && point2_c <= columns - 1 &&
              current_map[point2_r][point2_c] >= '0' && current_map[point2_r][point2_c] <= '9') {
            int unknown_in_p1_range_but_not_in_p2_range = 0;
            int unknown_not_in_p1_range_but_in_p2_range = 0;
            int unknown_in_p12_range = 0;
            int least_unseen_mine_in_p12_range = 0;
            for (int near_row = point1_r - 1; near_row <= point1_r + 1; ++near_row) {
              for (int near_column = point1_c - 1; near_column <= point1_c + 1; ++near_column) {
                if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                    current_map[near_row][near_column] == '?') {
                  if (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) {
                    unknown_in_p1_range_but_not_in_p2_range++;
                  } else {
                    unknown_in_p12_range++;
                  }
                }
              }
            }
            for (int near_row = point2_r - 1; near_row <= point2_r + 1; ++near_row) {
              for (int near_column = point2_c - 1; near_column <= point2_c + 1; ++near_column) {
                if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                    current_map[near_row][near_column] == '?') {
                  if (abs(point1_r - near_row) > 1 || abs(point1_c - near_column) > 1) {
                    unknown_not_in_p1_range_but_in_p2_range++;
                  }
                }
              }
            }
            least_unseen_mine_in_p12_range = (current_map[point1_r][point1_c] - '0') - seen_mines[point1_r][point1_c] -
                                             unknown_in_p1_range_but_not_in_p2_range;

            for (int j = 0; j <= 19; ++j) {
              int point3_r = possible_delta_r[j] + point2_r;
              int point3_c = possible_delta_c[j] + point2_c;
              if (point3_r == point1_r && point3_c == point1_c) {
                continue;
              }
              if (point3_r >= 0 && point3_r <= rows - 1 && point3_c >= 0 && point3_c <= columns - 1 &&
                  current_map[point3_r][point3_c] >= '0' && current_map[point3_r][point3_c] <= '9') {
                int most_unseen_mine_not_in_p1_range_but_in_p2_range =
                    std::min((current_map[point2_r][point2_c] - '0') - seen_mines[point2_r][point2_c] -
                                 least_unseen_mine_in_p12_range,
                             unknown_not_in_p1_range_but_in_p2_range);
                int least_unseen_mine_not_in_p1_range_but_in_p23_range = 0;
                int unknown_in_p3_range_st_in_p1_range_or_not_in_p2_range = 0;
                int unknown_not_in_p1_range_but_in_p23_range = 0;
                int unknown_not_in_p2_range_but_in_p3_range = 0;
                int unknown_in_p123_range = 0;
                int unknown_in_p23_range = 0;
                int unknown_in_p2_range_but_not_in_p1_range_and_not_in_p3_range = 0;
                for (int near_row = point3_r - 1; near_row <= point3_r + 1; ++near_row) {
                  for (int near_column = point3_c - 1; near_column <= point3_c + 1; ++near_column) {
                    if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                        current_map[near_row][near_column] == '?') {
                      if (abs(point2_r - near_row) <= 1 && abs(point2_c - near_column) <= 1) {
                        unknown_in_p23_range++;
                        if (abs(point1_r - near_row) <= 1 && abs(point1_c - near_column) <= 1) {
                          unknown_in_p123_range++;
                        }
                      }
                      if (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) {
                        unknown_not_in_p2_range_but_in_p3_range++;
                      }
                      if ((abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) ||
                          abs(point1_r - near_row) <= 1 && abs(point1_c - near_column) <= 1) {
                        unknown_in_p3_range_st_in_p1_range_or_not_in_p2_range++;
                      }
                      if ((abs(point1_r - near_row) > 1 || abs(point1_c - near_column) > 1) &&
                          abs(point2_r - near_row) <= 1 && abs(point2_c - near_column) <= 1) {
                        unknown_not_in_p1_range_but_in_p23_range++;
                      }
                    }
                  }
                }
                for (int near_row = point2_r - 1; near_row <= point2_r + 1; ++near_row) {
                  for (int near_column = point2_c - 1; near_column <= point2_c + 1; ++near_column) {
                    if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                        current_map[near_row][near_column] == '?') {
                      if ((abs(point3_r - near_row) > 1 || abs(point3_c - near_column) > 1) &&
                          (abs(point1_r - near_row) > 1 || abs(point1_c - near_column) > 1)) {
                        unknown_in_p2_range_but_not_in_p1_range_and_not_in_p3_range++;
                      }
                    }
                  }
                }
                least_unseen_mine_not_in_p1_range_but_in_p23_range =
                    (current_map[point3_r][point3_c] - '0') - seen_mines[point3_r][point3_c] -
                    unknown_in_p3_range_st_in_p1_range_or_not_in_p2_range;
                if (least_unseen_mine_not_in_p1_range_but_in_p23_range >=
                    most_unseen_mine_not_in_p1_range_but_in_p2_range) {
                  for (int near_row = point2_r - 1; near_row <= point2_r + 1; ++near_row) {
                    for (int near_column = point2_c - 1; near_column <= point2_c + 1; ++near_column) {
                      if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                          current_map[near_row][near_column] == '?') {
                        if ((abs(point1_r - near_row) > 1 || abs(point1_c - near_column) > 1) &&
                            (abs(point3_r - near_row) > 1 || abs(point3_c - near_column) > 1)) {
                          Execute(near_row, near_column, 0);
                          return;
                        }
                      }
                    }
                  }
                }

                int most_unseen_mine_not_in_p1_range_but_in_p23_range =
                    std::min(unknown_not_in_p1_range_but_in_p23_range,
                             (current_map[point3_r][point3_c] - '0') - seen_mines[point3_r][point3_c]);
                int most_unseen_mine_in_p12_range =
                    std::min((current_map[point1_r][point1_c] - '0') - seen_mines[point1_r][point1_c],
                             std::min((current_map[point2_r][point2_c] - '0') - seen_mines[point2_r][point2_c],
                                      unknown_in_p12_range));
                int least_unseen_mine_in_p2_range_but_not_in_p1_range_and_not_in_p3_range =
                    (current_map[point2_r][point2_c] - '0') - seen_mines[point2_r][point2_c] -
                    (most_unseen_mine_in_p12_range + most_unseen_mine_not_in_p1_range_but_in_p23_range);
                if (least_unseen_mine_in_p2_range_but_not_in_p1_range_and_not_in_p3_range ==
                        unknown_in_p2_range_but_not_in_p1_range_and_not_in_p3_range &&
                    unknown_in_p2_range_but_not_in_p1_range_and_not_in_p3_range != 0) {
                  for (int near_row = point2_r - 1; near_row <= point2_r + 1; ++near_row) {
                    for (int near_column = point2_c - 1; near_column <= point2_c + 1; ++near_column) {
                      if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                          current_map[near_row][near_column] == '?') {
                        if ((abs(point1_r - near_row) > 1 || abs(point1_c - near_column) > 1) &&
                            (abs(point3_r - near_row) > 1 || abs(point3_c - near_column) > 1)) {
                          std::cerr << "new logic:" << '\n' << near_row << ' ' << near_column << '\n';
                          std::cerr << "point1:" << '\n' << point1_r << ' ' << point1_c << '\n';
                          std::cerr << "point2:" << '\n' << point2_r << ' ' << point2_c << '\n';
                          std::cerr << "point3:" << '\n' << point3_r << ' ' << point3_c << '\n';
                          Execute(near_row, near_column, 1);
                          return;
                        }
                      }
                    }
                  }
                }

                int least_unseen_mine_in_p23_range =
                    (current_map[point2_r][point2_c] - '0') - seen_mines[point2_r][point2_c] -
                    unknown_in_p2_range_but_not_in_p1_range_and_not_in_p3_range - most_unseen_mine_in_p12_range;
                if (current_map[point3_r][point3_c] - '0' - seen_mines[point3_r][point3_c] <=
                        least_unseen_mine_in_p23_range &&
                    unknown_not_in_p2_range_but_in_p3_range) {
                  for (int near_row = point3_r - 1; near_row <= point3_r + 1; ++near_row) {
                    for (int near_column = point3_c - 1; near_column <= point3_c + 1; ++near_column) {
                      if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                          current_map[near_row][near_column] == '?') {
                        if (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) {
                          Execute(near_row, near_column, 0);
                          return;
                        }
                      }
                    }
                  }
                }

                int most_unseen_mine_in_p123_range = std::min(unknown_in_p123_range, most_unseen_mine_in_p12_range);
                int most_unseen_mine_in_p23_range =
                    std::min(unknown_in_p23_range,
                             most_unseen_mine_not_in_p1_range_but_in_p23_range + most_unseen_mine_in_p123_range);
                if (current_map[point3_r][point3_c] - '0' - seen_mines[point3_r][point3_c] -
                            most_unseen_mine_in_p23_range >=
                        unknown_not_in_p2_range_but_in_p3_range &&
                    unknown_not_in_p2_range_but_in_p3_range) {
                  for (int near_row = point3_r - 1; near_row <= point3_r + 1; ++near_row) {
                    for (int near_column = point3_c - 1; near_column <= point3_c + 1; ++near_column) {
                      if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                          current_map[near_row][near_column] == '?') {
                        if (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) {
                          Execute(near_row, near_column, 1);
                          return;
                        }
                      }
                    }
                  }
                }

                // 用四个格点信息，一起判断
                for (int k = 0; k <= 19; ++k) {
                  int point4_r = possible_delta_r[k] + point3_r;
                  int point4_c = possible_delta_c[k] + point3_c;
                  if (point4_r >= 0 && point4_r <= rows - 1 && point4_c >= 0 && point4_c <= columns - 1 &&
                      current_map[point4_r][point4_c] >= '0' && current_map[point4_r][point4_c] <= '9') {
                    int unknown_in_p4_range_st_not_in_p3_range_or_in_p23_range = 0;
                    int unknown_in_p34_range_but_not_in_p2_range = 0;
                    for (int near_row = point4_r - 1; near_row <= point4_r + 1; ++near_row) {
                      for (int near_column = point4_c - 1; near_column <= point4_c + 1; ++near_column) {
                        if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                            current_map[near_row][near_column] == '?') {
                          if ((abs(point3_r - near_row) > 1 || abs(point3_c - near_column) > 1) ||
                              ((abs(point3_r - near_row) <= 1 && abs(point3_c - near_column) <= 1) &&
                               (abs(point2_r - near_row) <= 1 && abs(point2_c - near_column) <= 1))) {
                            unknown_in_p4_range_st_not_in_p3_range_or_in_p23_range++;
                          }
                          if ((abs(point3_r - near_row) <= 1 && abs(point3_c - near_column) <= 1) &&
                              (abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1)) {
                            unknown_in_p34_range_but_not_in_p2_range++;
                          }
                        }
                      }
                    }
                    int least_unseen_mine_in_p34_range_but_not_in_p2_range =
                        (current_map[point4_r][point4_c] - '0') - seen_mines[point4_r][point4_c] -
                        unknown_in_p4_range_st_not_in_p3_range_or_in_p23_range;
                    int unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range = 0;
                    int saved_r_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range = 0;
                    int saved_c_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range = 0;
                    for (int near_row = point3_r - 1; near_row <= point3_r + 1; ++near_row) {
                      for (int near_column = point3_c - 1; near_column <= point3_c + 1; ++near_column) {
                        if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                            current_map[near_row][near_column] == '?') {
                          if ((abs(point2_r - near_row) > 1 || abs(point2_c - near_column) > 1) &&
                              (abs(point4_r - near_row) > 1 || abs(point4_c - near_column) > 1)) {
                            unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range++;
                            saved_r_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range = near_row;
                            saved_c_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range = near_column;
                          }
                        }
                      }
                    }
                    if (unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range &&
                        least_unseen_mine_in_p23_range + least_unseen_mine_in_p34_range_but_not_in_p2_range >=
                            (current_map[point3_r][point3_c] - '0') - seen_mines[point3_r][point3_c]) {
                      Execute(saved_r_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range,
                              saved_c_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range, 0);
                      return;
                    }

                    int most_unseen_mine_in_p34_range_but_not_in_p2_range =
                        std::min(unknown_in_p34_range_but_not_in_p2_range,
                                 (current_map[point4_r][point4_c] - '0') - seen_mines[point4_r][point4_c]);
                    if (unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range &&
                        (current_map[point3_r][point3_c] - '0') - seen_mines[point3_r][point3_c] >=
                            unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range +
                                most_unseen_mine_in_p34_range_but_not_in_p2_range + most_unseen_mine_in_p23_range) {
                      Execute(saved_r_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range,
                              saved_c_unknown_in_p3_range_but_not_in_p2_range_and_not_in_p4_range, 1);
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

  // 找是雷概率最小的不确定点进行尝试
  std::vector<point> remaining_unknown_points;
  std::vector<point> remaining_unknown_points_no_number_around;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (current_map[r][c] == '?') {
        point pt;
        pt.r = r;
        pt.c = c;
        pt.no_number_around = 0;
        double pt_mine_probability = 0;
        bool no_number_around = 1;
        for (int near_row = r - 1; near_row <= r + 1; ++near_row) {
          for (int near_column = c - 1; near_column <= c + 1; ++near_column) {
            if (near_row >= 0 && near_row <= rows - 1 && near_column >= 0 && near_column <= columns - 1 &&
                current_map[near_row][near_column] != '?' && current_map[near_row][near_column] != '@' &&
                seen_unknown[near_row][near_column]) {
              no_number_around = 0;
              double mine_probability_around =
                  ((double)(current_map[near_row][near_column] - '0') - seen_mines[near_row][near_column]) /
                  seen_unknown[near_row][near_column];
              if (mine_probability_around > pt_mine_probability) {
                pt_mine_probability = mine_probability_around;
                // std::cerr << "current point:\n"
                //           << r << ' ' << c << '\n'
                //           << "near point:\n"
                //           << near_row << ' ' << near_column << ' ' << mine_probability_around << '\n';
              }
            }
          }
        }
        pt.mine_probability = pt_mine_probability;
        if (no_number_around) {
          pt.no_number_around = 1;
          pt.mine_probability = ((double)total_mines - total_seen_mines) / total_unknown;
          remaining_unknown_points_no_number_around.push_back(pt);
        }
        remaining_unknown_points.push_back(pt);
      }
    }
  }

  int most_unseen_mine_in_no_number_around_area = total_mines - total_seen_mines;
  for (int start_r = 0; start_r <= 2; ++start_r) {
    for (int start_c = 0; start_c <= 2; ++start_c) {
      int count_unseen_mine_in_no_number_around_area = total_mines - total_seen_mines;
      for (int r = start_r; r < rows; r += 3) {
        for (int c = start_c; c < columns; c += 3) {
          if (r >= 0 && r <= rows - 1 && c >= 0 && c <= columns - 1 && current_map[r][c] >= '0' &&
              current_map[r][c] <= '9') {
            count_unseen_mine_in_no_number_around_area -= (current_map[r][c] - '0') - seen_mines[r][c];
          }
        }
      }
      if (most_unseen_mine_in_no_number_around_area > count_unseen_mine_in_no_number_around_area) {
        most_unseen_mine_in_no_number_around_area = count_unseen_mine_in_no_number_around_area;
        // std::cerr << "current start_r:" << start_r << " current_start_c:" << start_c << '\n';
      }
    }
  }

  int remaining_unknown_points_no_number_around_count = remaining_unknown_points_no_number_around.size();
  int remaining_unknown_points_count = remaining_unknown_points.size();

  if (most_unseen_mine_in_no_number_around_area == 0 && remaining_unknown_points_no_number_around_count) {
    std::cerr << "remaining_unknown_points\n"
              << "total_mines:" << total_mines << '\n'
              << "total_seen_mines:" << total_seen_mines << '\n'
              << "total_unknown:" << total_unknown << '\n'
              << "most_unseen_mine_in_no_number_around_area:" << most_unseen_mine_in_no_number_around_area << '\n'
              << "remaining_unknown_points_no_number_around_count:" << remaining_unknown_points_no_number_around_count
              << '\n';
    Execute(remaining_unknown_points_no_number_around[0].r, remaining_unknown_points_no_number_around[0].c, 0);
    return;
  }
  if (remaining_unknown_points_count == total_mines - total_seen_mines) {
    Execute(remaining_unknown_points[0].r, remaining_unknown_points[0].c, 1);
    return;
  }
  //   for (int j = 0; j < remaining_unknown_points_count; ++j) {
  //     for (int i = 0; i < remaining_unknown_points_no_number_around_count; ++i) {
  //       if (remaining_unknown_points[j].r == remaining_unknown_points_no_number_around[i].r &&
  //           remaining_unknown_points[j].c == remaining_unknown_points_no_number_around[i].c &&
  //           remaining_unknown_points_no_number_around_count) {
  //         remaining_unknown_points[j].mine_probability = std::min(
  //             remaining_unknown_points[j].mine_probability,
  //             (double)most_unseen_mine_in_no_number_around_area /
  //             remaining_unknown_points_no_number_around_count);
  //       }
  //     }
  //   }
  std::sort(remaining_unknown_points.begin(), remaining_unknown_points.end(), my_compare);
  std::cerr << "remaining_unknown_points\n"
            << "total_mines:" << total_mines << '\n'
            << "total_seen_mines:" << total_seen_mines << '\n'
            << "total_unknown:" << total_unknown << '\n';
  for (int i = 0; i < remaining_unknown_points_count; ++i) {
    std::cerr << remaining_unknown_points[i].r << ' ' << remaining_unknown_points[i].c << ' '
              << remaining_unknown_points[i].mine_probability << '\n';
  }
  if (remaining_unknown_points[remaining_unknown_points_count - 1].mine_probability >
      1 - remaining_unknown_points[0].mine_probability) {
    Execute(remaining_unknown_points[remaining_unknown_points_count - 1].r,
            remaining_unknown_points[remaining_unknown_points_count - 1].c, 1);
    return;
  }
  Execute(remaining_unknown_points[0].r, remaining_unknown_points[0].c, 0);
  return;
}

#endif

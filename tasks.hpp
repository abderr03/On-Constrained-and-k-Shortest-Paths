#pragma once

#include <algorithm>
#include <climits>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <stdio.h>
#include <utility>
#include <unordered_set>
#include <vector>

// Some macro and alias definitions.
#define pii std::pair<int, int>
#define tii std::tuple<int, int, int>

// Useful functions (implemented in 'utils.cpp').
void print_array(std::vector<int> &v);

void path(std::vector<int> &pred,
          std::vector<int> &path,
          int source, int target);
void path_delay(std::vector<std::vector<pii>> &pred,
                std::vector<int> &path,
                int source, int target, int b);

bool relax(int u, int v, int w,
           std::vector<int> &d,
           std::vector<int> &pred);
bool relax_delay(int u, int v, int w, int delay, int l,
                 std::vector<std::vector<int>> &d,
                 std::vector<std::vector<pii>> &pred);

void random_permutation(int n,
                        std::vector<int> &permutation,
                        std::vector<int> &rev_permutation);

// Function headers for task 1 (implemented in '1-task.cpp'):
void dijkstra(std::vector<std::vector<tii>> &graph,
              std::vector<int> &d,
              std::vector<int> &pred,
              int source, int target);
void dijkstra_timed(std::vector<std::vector<tii>> &graph,
              std::vector<int> &d,
              std::vector<int> &pred,
              int source, int target,
              long long int &preprocess_time, long long int &computation_time);
void dijkstra_parallel(std::vector<std::vector<tii>> &graph,
                       std::vector<int> &d,
                       std::vector<int> &pred,
                       int source, int target);
void bellman_ford(std::vector<std::vector<tii>> &graph,
                  std::vector<int> &d,
                  std::vector<int> &pred,
                  int source, int target);
void bellman_ford_timed(std::vector<std::vector<tii>> &graph,
                  std::vector<int> &d,
                  std::vector<int> &pred,
                  int source, int target,
                  long long int &preprocess_time, long long int &computation_time);
void bellman_ford_yen(std::vector<std::vector<tii>> &graph,
                      std::vector<int> &d,
                      std::vector<int> &pred,
                      int source, int target);
void bellman_ford_yen_timed(std::vector<std::vector<tii>> &graph,
                      std::vector<int> &d,
                      std::vector<int> &pred,
                      int source, int target,
                      long long int &preprocess_time, long long int &computation_time);
void delta_stepping(std::vector<std::vector<tii>> &graph,
                    std::vector<int> &d,
                    std::vector<int> &pred,
                    int source, int target, int delta);
void delta_stepping_timed(std::vector<std::vector<tii>> &graph,
                    std::vector<int> &d,
                    std::vector<int> &pred,
                    int source, int target, int delta,
                    long long int &preprocess_time, long long int &computation_time);

// Function headers for task 2 (implemented in '2-task.cpp'):
void dijkstra_constrained(std::vector<std::vector<tii>> &graph,
                          std::vector<std::vector<int>> &d,
                          std::vector<std::vector<pii>> &pred,
                          int source, int target, int b);
void dijkstra_constrained_timed(std::vector<std::vector<tii>> &graph,
                          std::vector<std::vector<int>> &d,
                          std::vector<std::vector<pii>> &pred,
                          int source, int target, int b,
                          long long int &preprocess_time, long long int &computation_time);
void bellman_ford_constrained(std::vector<std::vector<tii>> &graph,
                              std::vector<std::vector<int>> &d,
                              std::vector<std::vector<pii>> &pred,
                              int source, int target, int b);

void bellman_ford_constrained_timed(std::vector<std::vector<tii>> &graph,
                              std::vector<std::vector<int>> &d,
                              std::vector<std::vector<pii>> &pred,
                              int source, int target, int b,
                              long long int &preprocess_time, long long int &computation_time);

// Function headers for task 3 (implemented in '3-task.cpp'):
std::vector<std::vector<int>> dijkstra_k_shortest_paths(std::vector<std::vector<tii>> &graph,
                                                        int source, int k);

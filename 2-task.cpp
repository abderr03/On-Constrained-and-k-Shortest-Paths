#include "tasks.hpp"
#include <chrono>
using namespace std::chrono;

void dijkstra_constrained(std::vector<std::vector<tii>> &graph,
                          std::vector<std::vector<int>> &d,
                          std::vector<std::vector<pii>> &pred, int source,
                          int target, int b) {
  d[source][0] = 0;
  std::priority_queue<tii, std::vector<tii>, std::greater<tii>> pq(
      {}, {{0, 0, source}});

  int u, dist, l, delay;
  while (!pq.empty()) {
    std::tie(l, dist, u) = pq.top();
    if (u == target)
      break;

    pq.pop();
    if (dist <= d[u][l]) {
      for (auto const &[w, z, v] : graph[u]) {
        delay = l + z;
        if (delay <= b && relax_delay(u, v, w, delay, l, d, pred))
          pq.push({delay, d[v][delay], v});
      }
    }
  }
}

void dijkstra_constrained_timed(std::vector<std::vector<tii>> &graph,
                          std::vector<std::vector<int>> &d,
                          std::vector<std::vector<pii>> &pred, int source,
                          int target, int b,
                          long long int &preprocess_time, long long int &computation_time) {
  auto start = high_resolution_clock::now();
  d[source][0] = 0;
  std::priority_queue<tii, std::vector<tii>, std::greater<tii>> pq(
      {}, {{0, 0, source}});

  int u, dist, l, delay;

  auto stop = high_resolution_clock::now();
  preprocess_time = duration_cast<nanoseconds>(stop - start).count();

  start = high_resolution_clock::now();

  while (!pq.empty()) {
    std::tie(l, dist, u) = pq.top();
    if (u == target)
      break;

    pq.pop();
    if (dist <= d[u][l]) {
      for (auto const &[w, z, v] : graph[u]) {
        delay = l + z;
        if (delay <= b && relax_delay(u, v, w, delay, l, d, pred))
          pq.push({delay, d[v][delay], v});
      }
    }
  }
  
  stop = high_resolution_clock::now();
  computation_time = duration_cast<nanoseconds>(stop - start).count();
}

/* A variant of the Bellman-Ford algorithm for the constrains shortest path
 * problem using a dynamic programming approach. This algorithm runs in time O(n
 * m b) where n = |V|, m = |E| and b is the delay bound.
 */
void bellman_ford_constrained(std::vector<std::vector<tii>> &graph,
                              std::vector<std::vector<int>> &d,
                              std::vector<std::vector<pii>> &pred, int source,
                              int target, int b) {
  std::fill(d[source].begin(), d[source].end(), 0);
  std::fill(pred[source].begin(), pred[source].end(), pii{source, 0});
  int n = graph.size() - 1, l;

  for (int delay = 0; delay <= b; ++delay) {
    // We fill d[.][delay] using the Bellman-Ford algorithm (adapted).
    for (int _ = 1; _ < n; ++_) {
      for (int u = 1; u <= n; ++u) {
        for (auto const &[w, z, v] : graph[u]) {
          if (z <= delay) {
            l = delay - z;
            if (d[u][l] < INT_MAX)
              relax_delay(u, v, w, delay, l, d, pred);
          }
        }
      }
    }
  }
}


void bellman_ford_constrained_timed(std::vector<std::vector<tii>> &graph,
                              std::vector<std::vector<int>> &d,
                              std::vector<std::vector<pii>> &pred, int source,
                              int target, int b,
                              long long int &preprocess_time, long long int &computation_time) {
  auto start = high_resolution_clock::now();
  std::fill(d[source].begin(), d[source].end(), 0);
  std::fill(pred[source].begin(), pred[source].end(), pii{source, 0});
  int n = graph.size() - 1, l;

  auto stop = high_resolution_clock::now();
  preprocess_time = duration_cast<nanoseconds>(stop - start).count();

  start = high_resolution_clock::now();

  for (int delay = 0; delay <= b; ++delay) {
    // We fill d[.][delay] using the Bellman-Ford algorithm (adapted).
    for (int _ = 1; _ < n; ++_) {
      for (int u = 1; u <= n; ++u) {
        for (auto const &[w, z, v] : graph[u]) {
          if (z <= delay) {
            l = delay - z;
            if (d[u][l] < INT_MAX)
              relax_delay(u, v, w, delay, l, d, pred);
          }
        }
      }
    }
  }

  stop = high_resolution_clock::now();
  computation_time = duration_cast<nanoseconds>(stop - start).count();
}
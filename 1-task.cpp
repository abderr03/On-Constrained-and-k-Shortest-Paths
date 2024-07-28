#include "tasks.hpp"
#include <omp.h>
#include <chrono>
using namespace std::chrono;
/* Dijkstra's algorithm implementation using 'lazy deletion'.
 */
void dijkstra(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
              std::vector<int> &pred, int source, int target) {
  d[source] = 0;

  std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq(
      {}, {{0, source}});
  int l, u;
  while (!pq.empty()) {
    std::tie(l, u) = pq.top();
    if (u == target)
      return;

    pq.pop();
    if (l <= d[u]) {
      for (auto const &[w, _, v] : graph[u]) {
        if (relax(u, v, w, d, pred))
          pq.push({d[v], v});
      }
    }
  }
}

/* Dijkstra's algorithm implementation using 'lazy deletion'.
 */
void dijkstra_timed(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
              std::vector<int> &pred, int source, int target, 
              long long int &preprocess_time, long long int &computation_time) {
  auto start = std::chrono::high_resolution_clock::now();
  d[source] = 0;

  std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq(
      {}, {{0, source}});
  int l, u;

  auto stop = high_resolution_clock::now();
  preprocess_time = duration_cast<nanoseconds>(stop - start).count();

  start = high_resolution_clock::now();
  while (!pq.empty()) {
    std::tie(l, u) = pq.top();
    if (u == target)
      break;

    pq.pop();
    if (l <= d[u]) {
      for (auto const &[w, _, v] : graph[u]) {
        if (relax(u, v, w, d, pred))
          pq.push({d[v], v});
      }
    }
  }
  stop = high_resolution_clock::now();
  computation_time = duration_cast<nanoseconds>(stop - start).count();
}

/* A parallelization of the above Dijkstra's algorithm. Unfortunately, it seems
 * that the priority queue doesn't support parallel access... Largely inspired
 * from:
 * https://www.geeksforgeeks.org/parallel-dijkstras-algorithm-sssp-in-parallel/
 */
void dijkstra_parallel(std::vector<std::vector<tii>> &graph,
                       std::vector<int> &d, std::vector<int> &pred, int source,
                       int target) {
  d[source] = 0;
  int n = graph.size() - 1;

  std::vector<bool> visited(n + 1, false);
#pragma omp parallel for
  for (int i = 1; i <= n; ++i) {
    if (visited[target])
      continue;

    int u = -1, dist = INT_MAX;
    for (int v = 1; v <= n; ++v) {
      if (!visited[v] && d[v] < dist)
        u = v, dist = d[v];
    }
    visited[u] = 1;
    for (auto const &[w, _, v] : graph[u])
      relax(u, v, w, d, pred);
  }
}

/* Bellman-Ford algorithm implementation (without any optimizations).
 */
void bellman_ford(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
                  std::vector<int> &pred, int source, int target) {
  d[source] = 0;
  int n = graph.size() - 1;

  for (int _ = 1; _ < n; ++_) {
    for (int u = 1; u <= n; ++u) {
      if (d[u] < INT_MAX) {
        for (auto const &[w, _, v] : graph[u])
          relax(u, v, w, d, pred);
      }
    }
  }
}

void bellman_ford_timed(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
                  std::vector<int> &pred, int source, int target,
                  long long int &preprocess_time, long long int &computation_time) {
  auto start = high_resolution_clock::now();
  d[source] = 0;
  int n = graph.size() - 1;
  auto stop = high_resolution_clock::now();
  preprocess_time = duration_cast<nanoseconds>(stop - start).count();

  start = high_resolution_clock::now();
  for (int _ = 1; _ < n; ++_) {
    for (int u = 1; u <= n; ++u) {
      if (d[u] < INT_MAX) {
        for (auto const &[w, _, v] : graph[u])
          relax(u, v, w, d, pred);
      }
    }
  }
  stop = high_resolution_clock::now();
  computation_time = duration_cast<nanoseconds>(stop - start).count();
}

/* Bellman-Ford algorithm using Jin Y. Yen and M. J. Bannister & D. Eppstein
 * optimizations, see:
 *   - "An algorithm for finding shortest routes from all source nodes to a
 * given destination in general networks", Quarterly of Applied Mathematics,
 * Vol. 27, No. 4 (January 1970), pp. 526-530).
 *   - "Randomized Speedup of the Bellman–Ford Algorithm",
 *     2012 Proceedings of the Ninth Workshop on Analytic Algorithmics and
 * Combinatorics (ANALCO).
 *
 *
 * The following optimizations are implemented:
 *   - we stop the main loop when no further edge relaxation is done in the
 * inner loops.
 *   - at the i-th iteration, we relax only edges (u, v) for which d[u] has
 * changed in the last (i - 1)-th iteration or in the current iteration, this is
 * done via two boolean vectors ('to_relax' and 'queued').
 *   - we split the graph in two DAG in which edges (u, v) are such that s(u) >
 * s(v) (resp. s(v) > s(u)) where s is an uniformly chosen random permutation of
 * the nodes.
 */
void bellman_ford_yen(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
                      std::vector<int> &pred, int source, int target) {
  d[source] = 0;
  int n = graph.size() - 1;

  std::vector<int> permutation(n + 1), rev_permutation(n + 1);
  random_permutation(n, permutation, rev_permutation);

  // Sub-graphs (DAG) of G formed by edges (u, v) such that permutation[u] <
  // permutation[v] (resp. p...[u] > p...[v]).
  std::vector<std::vector<pii>> ef(n + 1), eb(n + 1);
  for (int u = 1; u <= n; ++u) {
    for (auto const &[w, _, v] : graph[u])
      permutation[u] < permutation[v] ? ef[u].push_back({w, v})
                                      : eb[u].push_back({w, v});
  }

  bool relaxation;
  std::vector<bool> to_relax(n + 1), queued(n + 1);
  to_relax[source] = 1;
  do {
    relaxation = false;
    for (auto const &u : rev_permutation) {
      if (to_relax[u] || queued[u]) {
        for (auto const &[w, v] : ef[u]) {
          if (relax(u, v, w, d, pred))
            queued[v] = relaxation = true;
        }
      }
    }

    for (auto u = rev_permutation.rbegin(); u != rev_permutation.rend(); ++u) {
      if (to_relax[*u] || queued[*u]) {
        for (auto const &[w, v] : eb[*u]) {
          if (relax(*u, v, w, d, pred))
            queued[v] = relaxation = true;
        }
      }
    }
    swap(to_relax, queued);
    std::fill(queued.begin(), queued.end(), 0);
  } while (relaxation);
}

void bellman_ford_yen_timed(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
                      std::vector<int> &pred, int source, int target,
                      long long int &preprocess_time, long long int &computation_time) {
  auto start = high_resolution_clock::now();
  d[source] = 0;
  int n = graph.size() - 1;

  std::vector<int> permutation(n + 1), rev_permutation(n + 1);
  random_permutation(n, permutation, rev_permutation);

  // Sub-graphs (DAG) of G formed by edges (u, v) such that permutation[u] <
  // permutation[v] (resp. p...[u] > p...[v]).
  std::vector<std::vector<pii>> ef(n + 1), eb(n + 1);
  
  for (int u = 1; u <= n; ++u) {
    for (auto const &[w, _, v] : graph[u])
      permutation[u] < permutation[v] ? ef[u].push_back({w, v})
                                      : eb[u].push_back({w, v});
  }

  bool relaxation;
  std::vector<bool> to_relax(n + 1), queued(n + 1);
  to_relax[source] = 1;

  auto stop = high_resolution_clock::now();
  preprocess_time = duration_cast<nanoseconds>(stop - start).count();

  start = high_resolution_clock::now();
  do {
    relaxation = false;
    for (auto const &u : rev_permutation) {
      if (to_relax[u] || queued[u]) {
        for (auto const &[w, v] : ef[u]) {
          if (relax(u, v, w, d, pred))
            queued[v] = relaxation = true;
        }
      }
    }

    for (auto u = rev_permutation.rbegin(); u != rev_permutation.rend(); ++u) {
      if (to_relax[*u] || queued[*u]) {
        for (auto const &[w, v] : eb[*u]) {
          if (relax(*u, v, w, d, pred))
            queued[v] = relaxation = true;
        }
      }
    }
    swap(to_relax, queued);
    std::fill(queued.begin(), queued.end(), 0);
  } while (relaxation);

  stop = high_resolution_clock::now();
  computation_time = duration_cast<nanoseconds>(stop - start).count();
}
/* An implementation of the Δ-stepping algorithm by U. Meyer and P. Sanders,
 * see:
 *    - "Δ-stepping: a parallelizable shortest path algorithm". Journal of
 * Algorithms. 1998 European Symposium on Algorithms.
 */
void relax_requests(int u, int delta, std::vector<std::vector<pii>> &edges,
                    std::vector<int> &d, std::vector<int> &pred,
                    std::map<int, std::unordered_set<int>> &B) {
  std::unordered_set<int> *Bi;
  int idx, old_idx;
  for (auto const &[w, v] : edges[u]) {
    if (d[v] != INT_MAX) {
      old_idx = d[v] / delta;
      Bi = &B[old_idx];
    } else
      Bi = 0;

    if (relax(u, v, w, d, pred)) {
      idx = d[v] / delta;
      if (!Bi || (Bi && idx != old_idx)) {
        if (Bi)
          Bi->erase(v);
        B[idx].insert(v);
      }
    }
  }
}

void delta_stepping(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
                    std::vector<int> &pred, int source, int target, int delta) {
  d[source] = 0;
  int n = graph.size() - 1;

  // Sub-graphs of G formed by 'light' (resp. 'heavy') edges (u, v) i.e. w(u,
  // v)
  // <= Δ (resp. w(u, v) > Δ).
  std::vector<std::vector<pii>> el(n + 1), eh(n + 1);
  for (int u = 1; u <= n; ++u) {
    for (auto const &[w, _, v] : graph[u])
      w <= delta ? el[u].push_back({w, v}) : eh[u].push_back({w, v});
  }

  std::map<int, std::unordered_set<int>> B;
  B[0].insert(source);
  while (!B.empty()) {
    int i = B.begin()->first;
    auto &Bi = B.begin()->second;

    std::unordered_set<int> R;
    do {
      auto bi = Bi;
      R.insert(Bi.begin(), Bi.end());
      Bi.clear();

      for (auto const &u : bi)
        relax_requests(u, delta, el, d, pred, B);
    } while (!Bi.empty());

    for (auto const &u : R)
      relax_requests(u, delta, eh, d, pred, B);
    B.erase(i);
  }
}

void delta_stepping_timed(std::vector<std::vector<tii>> &graph, std::vector<int> &d,
                    std::vector<int> &pred, int source, int target, int delta,
                    long long int &preprocess_time, long long int &computation_time) {
  auto start = high_resolution_clock::now();
  d[source] = 0;
  int n = graph.size() - 1;

  // Sub-graphs of G formed by 'light' (resp. 'heavy') edges (u, v) i.e. w(u,
  // v)
  // <= Δ (resp. w(u, v) > Δ).
  std::vector<std::vector<pii>> el(n + 1), eh(n + 1);
  for (int u = 1; u <= n; ++u) {
    for (auto const &[w, _, v] : graph[u])
      w <= delta ? el[u].push_back({w, v}) : eh[u].push_back({w, v});
  }

  std::map<int, std::unordered_set<int>> B;
  B[0].insert(source);

  auto stop = high_resolution_clock::now();
  preprocess_time = duration_cast<nanoseconds>(stop - start).count();

  start = high_resolution_clock::now();
  while (!B.empty()) {
    int i = B.begin()->first;
    auto &Bi = B.begin()->second;

    std::unordered_set<int> R;
    do {
      auto bi = Bi;
      R.insert(Bi.begin(), Bi.end());
      Bi.clear();

      for (auto const &u : bi)
        relax_requests(u, delta, el, d, pred, B);
    } while (!Bi.empty());

    for (auto const &u : R)
      relax_requests(u, delta, eh, d, pred, B);
    B.erase(i);
  }
  stop = high_resolution_clock::now();
  computation_time = duration_cast<nanoseconds>(stop - start).count();
}

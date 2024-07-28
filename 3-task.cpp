#include "tasks.hpp"

std::vector<std::vector<int>>
dijkstra_k_shortest_paths(std::vector<std::vector<tii>> &graph, int source,
                          int k) {
  std::vector<std::vector<int>> shortest_paths;
  std::priority_queue<std::vector<int>, std::vector<std::vector<int>>,
                      std::greater<std::vector<int>>>
      pq; // Priority queue of paths

  // Initialize path from source to itself
  std::vector<int> initial_path = {0, source}; // {distance, vertex}
  pq.push(initial_path);

  int u;
  while (!pq.empty() && (int)shortest_paths.size() < k) {
    std::vector<int> current_path = pq.top();
    pq.pop();

    // If the current path is not the initial path, add it to the list of
    // shortest paths
    if (current_path.size() != 2)
      shortest_paths.emplace_back(current_path);

    u = current_path.back();
    for (auto const &[w, _, v] : graph[u]) {
      // Create a new path extending the current path
      current_path[0] += w;
      current_path.push_back(v);

      // Push the new path into the priority queue
      pq.emplace(current_path);

      // Reset the current path to its initial state.
      current_path[0] -= w;
      current_path.pop_back();
    }
  }
  return shortest_paths;
}

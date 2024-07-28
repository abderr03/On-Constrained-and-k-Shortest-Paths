#include "tasks.hpp"

/* Build the path from 'source' to 'target' using a vector
 * (rotating / mirroring the vector is needed).
 */
void path(std::vector<int> &pred, std::vector<int> &path, int source,
          int target) {
  while (target != source) {
    path.push_back(target);
    target = pred[target];
  }
  path.push_back(source);
  std::reverse(path.begin(), path.end());
}

/* The same as above but adapted for task 2 with the delay constraint.
 */
void path_delay(std::vector<std::vector<pii>> &pred, std::vector<int> &path,
                int source, int target, int b) {
  while (target != source) {
    path.push_back(target);
    std::tie(target, b) = pred[target][b];
  }
  path.push_back(source);
  std::reverse(path.begin(), path.end());
}

/* Perform an edge relaxation i.e. update the shortest distance and predecessor
 * if needed. Return 'true' if a relaxation has been done, 'false' otherwise.
 */
bool relax(int u, int v, int w, std::vector<int> &d, std::vector<int> &pred) {
  int p = d[u] + w;
  if (p < d[v]) {
    d[v] = p;
    pred[v] = u;
    return true;
  }
  return false;
}

/* The same as above but adapted for task 2 with the delay constraint.
 */
bool relax_delay(int u, int v, int w, int delay, int l,
                 std::vector<std::vector<int>> &d,
                 std::vector<std::vector<pii>> &pred) {
  int p = d[u][l] + w;
  if (p < d[v][delay]) {
    d[v][delay] = p;
    pred[v][delay] = {u, l};
    return true;
  }
  return false;
}

/* Build a random permutation of [|1, n|] and its reserve permutation.
 * It's used to create a total order over the nodes of the input graph (the
 * first entry in the vector is 0 and left untouched - not used in the graph).
 */
void random_permutation(int n, std::vector<int> &permutation,
                        std::vector<int> &rev_permutation) {
  std::iota(permutation.begin(), permutation.end(), 0);
  std::shuffle(permutation.begin() + 1, permutation.end(),
               std::mt19937{std::random_device{}()});
  for (int i = 1; i <= n; ++i)
    rev_permutation[permutation[i]] = i;
}

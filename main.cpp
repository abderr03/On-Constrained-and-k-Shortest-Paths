#include "tasks.hpp"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <string>
#include <thread>
using namespace std::chrono;

int SLEEP_TIME = 5;
long long int preprocess_time = 0;
long long int computation_time = 0;

// The messages to print in the shell.
std::string tasks = "Choose a task number: 1, 2, 3 or 4.";
std::string task_1_algo =
    "Choose an algorithm to apply:\n  - 1: Dijkstra\n  - 2: Dijkstra "
    "parallel\n  - 3: Bellman-Ford (naive)\n  - 4: Bellman-Ford (optimized)\n  "
    "- 5: Delta-stepping";
std::string task_2_algo = "Choose an algorithm to apply:\n  - 1: "
                          "Dijkstra-like\n  - 2: Bellman-Ford variant parallel";
std::string task_3_k = "Choose the hyperparameter k.";
std::string input = "Your choice: ";
std::string repeat_message = "Number of repetitions.";
std::string delta_stepping_hyperparameter = "Choose the hyperparameter delta.";

/* The function is used to read the user input correctly given finite number of
 * outputs.
 */
int read_input(std::string message,
               std::unordered_set<std::string> valid_outputs) {
  std::string s;
  std::cout << message << std::endl;
  do {
    std::cout << input << std::flush;
    getline(std::cin, s);
  } while (valid_outputs.find(s) == valid_outputs.end());

  return atoi(s.c_str());
}

/* The function checks if s is an integer.
 */
bool is_integer(std::string &s) {
  try {
    stoi(s);
  } catch (std::invalid_argument const &) {
    return false;
  }
  return true;
}

/* The function is used to read a positive integer from the user input.
 */
int read_int(std::string message) {
  std::string s;
  std::cout << message << std::endl;
  do {
    std::cout << input << std::flush;
    getline(std::cin, s);
  } while (!is_integer(s) || stoi(s) <= 0);

  return stoi(s);
}

/*
  This file serves as a router to call the different tasks.

  Usage:
  ./main [input file]
*/
int main(int argc, char **argv) {
  std::cin.tie(0)->sync_with_stdio(0);
  if (argc < 2)
    printf("Not enough arguments.\nUsage: %s [input file]\n", argv[0]);
  else {
    std::ifstream file(argv[1]);
    if (file.is_open()) {
      // Read the input:
      int n, m, b, u, v, w, z;
      file >> n >> m >> b;

      // Reading arcs endpoints, their weight and delay.
      std::vector<std::vector<tii>> graph(n + 1);
      while (m--) {
        file >> u >> v >> w >> z;
        graph[u].push_back({w, z, v});
      }

      // Reading the task number.
      int num = read_input(tasks, {"1", "2", "3", "4"}), algo;

      // Reading the source and target (if necessary).
      std::unordered_set<std::string> valid_vertices;
      for (int i = 1; i <= n; ++i)
        valid_vertices.insert(std::to_string(i));
      int source = read_input("Source (1 to " + std::to_string(n) + ").",
                              valid_vertices);
      int target = read_input("Target (1 to " + std::to_string(n) + ").",
                              valid_vertices);
      int num_repeat = read_int(repeat_message);

      auto start = high_resolution_clock::now();
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<nanoseconds>(stop - start);
      std::vector<long long int> times;
      std::vector<long long int> preprocess_times;
      std::vector<long long int> computation_times;
      double total_time = 0.0;
      double total_preprocess_time = 0.0;
      double total_computation_time = 0.0;

      // Execute the selected task.
      if (num == 1) {
        algo = read_input(task_1_algo, {"1", "2", "3", "4", "5"});
        int delta = 0;
        if (algo == 5)
          delta = read_int(delta_stepping_hyperparameter);
        std::vector<int> d(n + 1, INT_MAX), pred(n + 1);

        for (int _ = 0; _ < num_repeat; ++_) {
          std::fill(d.begin(), d.end(), INT_MAX);
          std::fill(pred.begin(), pred.end(), 0);

          if (algo == 1) {
            start = high_resolution_clock::now();
            dijkstra_timed(graph, d, pred, source, target, preprocess_time,
                           computation_time);
            stop = high_resolution_clock::now();
          } else if (algo == 2) {
            start = high_resolution_clock::now();
            dijkstra_parallel(graph, d, pred, source, target);
            stop = high_resolution_clock::now();
          } else if (algo == 3) {
            start = high_resolution_clock::now();
            bellman_ford_timed(graph, d, pred, source, target, preprocess_time,
                               computation_time);
            stop = high_resolution_clock::now();
          } else if (algo == 4) {
            start = high_resolution_clock::now();
            bellman_ford_yen_timed(graph, d, pred, source, target,
                                   preprocess_time, computation_time);
            stop = high_resolution_clock::now();
          } else {
            start = high_resolution_clock::now();
            delta_stepping_timed(graph, d, pred, source, target, delta,
                                 preprocess_time, computation_time);
            stop = high_resolution_clock::now();
          }
          duration = duration_cast<nanoseconds>(stop - start);
          times.push_back(duration.count());
          preprocess_times.push_back(preprocess_time);
          computation_times.push_back(computation_time);
          total_time += (double)duration.count();
          total_preprocess_time += (double)preprocess_time;
          total_computation_time += (double)computation_time;
          std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        }
        total_time /= (double)num_repeat;
        total_preprocess_time /= (double)num_repeat;
        total_computation_time /= (double)num_repeat;

        std::vector<int> p;
        path(pred, p, source, target);

        std::cout << '\n' << "Times (in ns):";
        for (auto &t : times)
          std::cout << ' ' << t;

        std::cout << '\n' << "Preprocess Times (in ns):";
        for (auto &t : preprocess_times)
          std::cout << ' ' << t;

        std::cout << '\n' << "Computation Times (in ns):";
        for (auto &t : computation_times)
          std::cout << ' ' << t;

        std::cout << '\n'
                  << "Average time: " << total_time << "ns" << '\n'
                  << "Average preprocess time: " << total_preprocess_time
                  << "ns" << '\n'
                  << "Average computation time: " << total_computation_time
                  << "ns" << '\n'
                  << "Path:";
        for (auto const &v : p)
          std::cout << " " << v;
        std::cout << '\n' << "Length: " << d[target] << '\n' << std::endl;
      } else if (num == 2) {
        algo = read_input(task_2_algo, {"1", "2"});
        std::vector<std::vector<int>> d(n + 1,
                                        std::vector<int>(b + 1, INT_MAX));
        std::vector<std::vector<pii>> pred(n + 1, std::vector<pii>(b + 1));

        for (int _ = 0; _ < num_repeat; ++_) {
          std::fill(d.begin(), d.end(), std::vector<int>(b + 1, INT_MAX));
          std::fill(pred.begin(), pred.end(), std::vector<pii>(b + 1));

          if (algo == 1) {
            start = high_resolution_clock::now();
            dijkstra_constrained_timed(graph, d, pred, source, target, b, preprocess_time, computation_time);
            stop = high_resolution_clock::now();
          } else {
            start = high_resolution_clock::now();
            bellman_ford_constrained_timed(graph, d, pred, source, target, b, preprocess_time, computation_time);
            stop = high_resolution_clock::now();
          }
          duration = duration_cast<nanoseconds>(stop - start);
          times.push_back(duration.count());
          preprocess_times.push_back(preprocess_time);
          computation_times.push_back(computation_time);
          total_time += (double)duration.count();
          total_preprocess_time += (double)preprocess_time;
          total_computation_time += (double)computation_time;
          std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        }
        total_time /= (double)num_repeat;
        total_preprocess_time /= (double)num_repeat;
        total_computation_time /= (double)num_repeat;

        int i;
        for (i = 0; pred[target][i].first == 0; ++i)
          ;
        std::vector<int> path;
        path_delay(pred, path, source, target, i);

        std::cout << '\n' << "Times (in ns):";
        for (auto &t : times)
          std::cout << ' ' << t;

        std::cout << '\n' << "Preprocess Times (in ns):";
        for (auto &t : preprocess_times)
          std::cout << ' ' << t;

        std::cout << '\n' << "Computation Times (in ns):";
        for (auto &t : computation_times)
          std::cout << ' ' << t;

        std::cout << '\n'
                  << "Average time: " << total_time << "ns" << '\n'
                  << "Average preprocess time: " << total_preprocess_time
                  << "ns" << '\n'
                  << "Average computation time: " << total_computation_time
                  << "ns" << '\n'
                  << "Path:";
        for (auto const &v : path)
          std::cout << " " << v;
        std::cout << '\n'
                  << "Length: " << d[target][i] << '\n'
                  << "Delay: " << i << '\n'
                  << std::endl;
      } else if (num == 3) {
        int k = read_int(task_3_k);

        std::vector<std::vector<int>> paths;
        for (int _ = 0; _ < num_repeat; ++_) {
          paths.clear();

          start = high_resolution_clock::now();
          paths = dijkstra_k_shortest_paths(graph, source, k);
          stop = high_resolution_clock::now();
          duration = duration_cast<nanoseconds>(stop - start);
          times.push_back(duration.count());
          total_time += (double)duration.count();
          std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        }
        total_time /= (double)num_repeat;

        std::cout << '\n' << "Times (in ns):";
        for (auto &t : times)
          std::cout << ' ' << t;
        std::cout << '\n' << "Average time: " << total_time << "ns" << '\n';
        for (auto const &p : paths) {
          std::cout << '\n' << "Path:";
          for (size_t i = 1; i < p.size(); ++i)
            std::cout << " " << p[i];
          std::cout << '\n' << "Length: " << p[0] << '\n' << std::endl;
        }
      }
      file.close();
    } else
      printf("Error: cannot open file: %s", argv[1]);
  }
  return 0;
}

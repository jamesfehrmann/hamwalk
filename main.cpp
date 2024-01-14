// James Fehrmann
// Simple Hamiltonian Walk demonstration in C++
// License: MIT
// Editor: Neovim
// Date: 01/14/24

#include <chrono>
#include <iostream>
using namespace std::chrono;

time_point<high_resolution_clock> now() { return high_resolution_clock::now(); }

float getDeltaTime(time_point<high_resolution_clock> lastTime) {
    return duration_cast<duration<float>>(now() - lastTime).count();
}

time_point<high_resolution_clock> updateLastTime() { return now(); }

float max(float a, float b) {
    if (b > a)
        return b;
    return a;
}

float min(float a, float b) {
    if (b < a)
        return b;
    return a;
}

// time sink
void sleep(int ms) {
    float z = ms / 1000.f;
    time_point<high_resolution_clock> lastTime = updateLastTime();
    while (getDeltaTime(lastTime) < z) {
        // grab a snickers
    }
}
bool isSafe(int v, bool **graph, int path[], int pos) {
    if (graph[path[pos - 1]][v] == 0) {
        return false; // not connected
    }
    for (int i = 0; i < pos; ++i) {
        if (path[i] == v) {
            return false; // already in the path
        }
    }
    return true; // viable
}

void printPath(int *path, int n) {
    std::cout << "Hamiltonian: ";
    for (int i = 0; i < n; ++i) {
        std::cout << path[i] << " ";
    }
    std::cout << path[0] << std::endl;
}

// recursive hamiltonian walk
bool hamWalk(bool **graph, int n, int path[], int pos, bool sleepy) {
    if (sleepy) // artificially slow it down.
        sleep(10);

    // base case
    if (pos == n) {
        return (graph[path[pos - 1]][path[0]] == 1);
    }

    // recusive loop
    for (int v = 1; v < n; ++v) {
        if (isSafe(v, graph, path, pos)) {
            path[pos] = v;
            if (hamWalk(graph, n, path, pos + 1, sleepy) == true) {
                return true;
            }
            path[pos] = -1;
        }
    }
    return false;
}

bool hamiltonian(bool **graph, int n, bool sleepy, bool verbose) {
    int path[n];
    for (int i = 0; i < n; ++i)
        path[i] = -1;

    path[0] = 0;

    if (hamWalk(graph, n, path, 1, sleepy) == false) {
        if (verbose)
            std::cout << "Hamiltonian: Solution does not exist" << std::endl;
        return false;
    } else {
        if (verbose)
            printPath(path, n);
    }
    return true;
}

// create an nxn array and return a pointer to that array.
bool **makeArray(const int n) {
    bool **arr = new bool *[n];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            arr[i] = new bool[n];
        }
    }
    return arr;
}

void deleteArray(bool **arr, int n) {
    for (int i = 0; i < n; ++i) {
        delete[] arr[i];
    }
    delete[] arr;
}

// fill array with 0s and 1s to make graph.
// rate determines the probability of two points being connected.
// e.g., rate=15 should result in about 15% 1s, a space graph.
void makeGraph(bool **graph, int n, int rate) {
    for (int i = 0; i < n; ++i) {
        graph[i][i] = false;
        for (int j = i + 1; j < n; ++j) {
            graph[i][j] = std::rand() % 100 <= rate;
            graph[j][i] = graph[i][j];
        }
    }
}

void printGraph(bool **graph, int n) {
    std::cout << "{" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << "    {";
        for (int j = 0; j < n; ++j) {
            std::cout << graph[i][j];
            if (j < n - 1) {
                std::cout << ",";
            } else {
                std::cout << "}";
            }
        }
        if (i < n - 1) {
            std::cout << "," << std::endl;
        } else {
            std::cout << "\n};" << std::endl;
        }
    }
}

void printBreak() { std::cout << "-----------------------" << std::endl; }

int main() {
    int nMin = 3; // size of graph
    int nMax = 100;
    int nStep = 1;
    int r = 15; // connectivity rate
    int i = 5;  // how many trials
    bool verbose = false;

    time_point<high_resolution_clock> lastTime = updateLastTime();
    float deltaTime = getDeltaTime(lastTime);

    for (int n = nMin; n <= nMax; n += nStep) {
        float avgRunTime = 0;
        float totalRunTime = 0;
        float maxRunTime = 0;
        float minRunTime = 9999999999.f;
        bool **graph = makeArray(n);
        for (int p = 0; p < i; ++p) {
            bool solutionFound = false;
            // find a solution quickly, then use it with time sink
            while (!solutionFound) {
                makeGraph(graph, n, r);
                solutionFound = hamiltonian(graph, n, false, verbose);
            }
            std::cout << "+" << std::flush; // notify console a solution was found
            if (verbose)
                printGraph(graph, n);
            lastTime = updateLastTime();
            solutionFound = hamiltonian(graph, n, true, verbose);
            deltaTime = getDeltaTime(lastTime);
            std::cout << "*" << std::flush; // notify console a solution was timed.
            totalRunTime += deltaTime;
            maxRunTime = max(maxRunTime, deltaTime);
            minRunTime = min(minRunTime, deltaTime);
        }
        deleteArray(graph, n);
        avgRunTime = totalRunTime / i;
        std::cout << ",vertices=" << n
                  << ",tot=" << static_cast<int>(totalRunTime * 1000)
                  << ",avg=" << static_cast<int>(avgRunTime * 1000)
                  << ",min=" << static_cast<int>(minRunTime * 1000)
                  << ",max=" << static_cast<int>(maxRunTime * 1000) << std::endl;

        if (verbose)
            printBreak();
    }
    return 0;
}

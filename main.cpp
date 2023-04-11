#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>

constexpr int N = 4;
constexpr int MAX_DEPTH = 100;
constexpr int INF = std::numeric_limits<int>::max();

struct State {
    int board[N][N];
    int x, y;

    State() {
        int val = 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                board[i][j] = val++;
            }
        }
        board[N - 1][N - 1] = 0;
        x = N - 1;
        y = N - 1;
    }

    void print() const {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                std::cout << board[i][j] << "   ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

bool operator==(const State& a, const State& b) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (a.board[i][j] != b.board[i][j]) return false;
        }
    }
    return a.x == b.x && a.y == b.y;
}

int distancia_manhattan(const State& state) {
    int distance = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int value = state.board[i][j];
            if (value != 0) {
                int target_x = (value - 1) / N;
                int target_y = (value - 1) % N;
                distance += std::abs(i - target_x) + std::abs(j - target_y);
            }
        }
    }
    return distance;
}

std::vector<State> successors(const State& state) {
    std::vector<State> succ;
    int x = state.x;
    int y = state.y;
    if (x > 0) {
        State s = state;
        s.x = x - 1;
        s.board[x][y] = state.board[x - 1][y];
        s.board[x - 1][y] = state.board[x][y];
        succ.push_back(s);
    }
    if (x < N - 1) {
        State s = state;
        s.x = x + 1;
        s.board[x][y] = state.board[x + 1][y];
        s.board[x + 1][y] = state.board[x][y];
        succ.push_back(s);
    }
    if (y > 0) {
        State s = state;
        s.y = y - 1;
        s.board[x][y] = state.board[x][y - 1];
        s.board[x][y - 1] = state.board[x][y];
        succ.push_back(s);
    }
    if (y < N - 1) {
        State s = state;
        s.y = y + 1;
        s.board[x][y] = state.board[x][y + 1];
        s.board[x][y + 1] = state.board[x][y];
        succ.push_back(s);
    }
    return succ;
}

int search(State state,int g, int bound, int (*heuristic)(const State&), std::vector<State>& solution, bool& found_solution) {
    int f = g + heuristic(state);
    if (f > bound) {
        return f;
    }
    if (heuristic(state) == 0) {
        found_solution = true;
        solution.push_back(state);
        return f;
    }
    int min_t = INF;
    std::vector<State> succ = successors(state);
    for (const auto& s : succ) {
        if (s == state) {
            continue;
        }
        int t = search(s, g + 1, bound, heuristic, solution, found_solution);
        if (found_solution) {
            solution.push_back(state);
            return t;
        }
        min_t = std::min(min_t, t);
    }
    return min_t;
}

std::vector<State> ida_star_search(const State& initial, int (*heuristic)(const State&)) {
    std::vector<State> solution;
    int bound = heuristic(initial);
    while (true) {
        bool found_solution = false;
        int t = search(initial, 0, bound, heuristic, solution, found_solution);
        if (found_solution) {
            std::reverse(solution.begin(), solution.end());
            return solution;
        } else if (bound == INF) {
            return solution;
        } else {
            bound = t;
        }
    }
}

int main() {
    State initial;
    std::mt19937 rng(std::random_device{}());
    for (int i = 0; i < MAX_DEPTH; i++) {
        std::vector<State> succ = successors(initial);
        std::uniform_int_distribution<size_t> dist(0, succ.size() - 1);
        initial = succ[dist(rng)];
    }
    std::cout << "Tablero inicial:" << std::endl;
    initial.print();
    std::vector<State> solution = ida_star_search(initial, distancia_manhattan);
    if (solution.empty()) {
        std::cout << "No se encontró solución" << std::endl;
    } else {
        std::cout << "Solucion encontrada con costo " << solution.size() - 1 << ": " << std::endl;
        for (const auto& s : solution) {
            s.print();
        }
    }
    return 0;
}


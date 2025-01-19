#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <numeric>

using namespace std;

// Mutexes for thread safety
mutex solutions_mutex;
atomic<int> total_solutions(0);

// Function to solve the puzzle for a given range of T values
void solve_range(vector<int> T_values, vector<string>& solutions) {
    for (int T : T_values) {
        int H = (10 - (2 * T % 10)) % 10; // Calculate H based on T
        if (H == T)
            continue;

        int used_digits_TH = (1 << T) | (1 << H); // Bitmask for T and H

        for (int N = 0; N <= 9; ++N) {
            if (used_digits_TH & (1 << N))
                continue;
            int used_N = used_digits_TH | (1 << N);

            for (int E = 0; E <= 9; ++E) {
                if (used_N & (1 << E))
                    continue;
                int used_NE = used_N | (1 << E);

                for (int S = 0; S <= 9; ++S) {
                    if (used_NE & (1 << S))
                        continue;
                    int used_NES = used_NE | (1 << S);

                    for (int W = 0; W <= 9; ++W) {
                        if (used_NES & (1 << W))
                            continue;
                        int used_NESW = used_NES | (1 << W);

                        for (int O = 0; O <= 9; ++O) {
                            if (used_NESW & (1 << O))
                                continue;
                            int used_NESWO = used_NESW | (1 << O);

                            for (int R = 0; R <= 9; ++R) {
                                if (used_NESWO & (1 << R))
                                    continue;
                                int used_NESWOR = used_NESWO | (1 << R);

                                for (int A = 0; A <= 9; ++A) {
                                    if (used_NESWOR & (1 << A))
                                        continue;
                                    int used_NESWORA = used_NESWOR | (1 << A);

                                    for (int U = 0; U <= 9; ++U) {
                                        if (used_NESWORA & (1 << U))
                                            continue;

                                        // Compute the numerical values
                                        int NORTH = N * 10000 + O * 1000 + R * 100 + T * 10 + H;
                                        int EAST = E * 1000 + A * 100 + S * 10 + T;
                                        int SOUTH = S * 10000 + O * 1000 + U * 100 + T * 10 + H;
                                        int WEST = W * 1000 + E * 100 + S * 10 + T;
                                        int EARTH = E * 10000 + A * 1000 + R * 100 + T * 10 + H;

                                        // Check the equation
                                        if (NORTH + EAST + SOUTH + WEST == EARTH) {
                                            // Thread-safely update the total solution count
                                            total_solutions++;

                                            // Collect the solution
                                            stringstream ss;
                                            ss << "Solution " << total_solutions << ":\n";
                                            ss << "NORTH (" << N << O << R << T << H << ") + EAST (" << E << A << S << T << ") + SOUTH (" << S << O << U << T << H << ") + WEST (" << W << E << S << T << ") = EARTH (" << E << A << R << T << H << ")\n";
                                            ss << NORTH << " + " << EAST << " + " << SOUTH << " + " << WEST << " = " << EARTH << "\n\n";

                                            // Lock mutex before modifying shared solutions vector
                                            lock_guard<mutex> guard(solutions_mutex);
                                            solutions.push_back(ss.str());
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
}

// Function to run the puzzle solver multiple times and measure performance
void runMultipleTimes(int iterations) {
    vector<double> times; // Store execution times
    vector<int> solutions_counts; // Store solution counts
    times.reserve(iterations);
    solutions_counts.reserve(iterations);

    for (int i = 0; i < iterations; ++i) {
        auto start_time = chrono::high_resolution_clock::now(); // Start timer

        vector<string> solutions;
        total_solutions = 0;  // Reset total solutions for each run

        // Determine the number of hardware threads supported
        unsigned int num_threads = thread::hardware_concurrency();
        if (num_threads == 0)
            num_threads = 4; // Default to 4 threads if hardware_concurrency() can't determine

        // Collect all valid T values based on the constraint
        vector<int> valid_T_values;
        for (int T = 0; T <= 9; ++T) {
            int H = (10 - (2 * T % 10)) % 10;
            if (H != T)
                valid_T_values.push_back(T);
        }

        // Divide T values among threads
        vector<vector<int>> T_values_per_thread(num_threads);

        for (size_t i = 0; i < valid_T_values.size(); ++i) {
            T_values_per_thread[i % num_threads].push_back(valid_T_values[i]);
        }

        // Create threads
        vector<thread> threads;
        for (unsigned int i = 0; i < num_threads; ++i) {
            threads.emplace_back(solve_range, T_values_per_thread[i], ref(solutions));
        }

        // Join threads
        for (auto& th : threads) {
            th.join();
        }

        auto end_time = chrono::high_resolution_clock::now(); // End timer
        auto elapsed = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() / 1000.0;

        times.push_back(elapsed);
        solutions_counts.push_back(total_solutions.load());
    }

    // Calculate and display statistics
    double fastest = *min_element(times.begin(), times.end());
    double slowest = *max_element(times.begin(), times.end());
    double average = accumulate(times.begin(), times.end(), 0.0) / times.size();

    for (int i = 0; i < iterations; ++i) {
        cout << "Run " << i + 1 << ": " << times[i] << " milliseconds, " << solutions_counts[i] << " solutions\n";
    }

    cout << "Total number of executions: " << iterations << "\n";
    cout << "Fastest time: " << fastest << " milliseconds\n";
    cout << "Slowest time: " << slowest << " milliseconds\n";
    cout << "Average time: " << average << " milliseconds\n";
}

int main() {
    runMultipleTimes(1000); // Run the solver 1000 times
    return 0;
}
# Optimizing the Cryptarithmetic Puzzle Solver: Techniques and Heuristics

## Introduction

This report details the optimization techniques and heuristics employed to enhance the efficiency of a C++ program designed to solve the cryptarithmetic puzzle:

```
NORTH + EAST + SOUTH + WEST = EARTH
```

Each letter represents a unique digit from 0 to 9. The objective was to find all valid digit assignments that satisfy the equation while optimizing the code for maximum efficiency. The code incorporates multiple optimization strategies, including algorithmic improvements, mathematical constraints, efficient data structures, multithreading, and compiler optimizations.

```
On my machine I was able to achieve the following results:
Total number of executions: 1000
Fastest time: 0.48 milliseconds
Slowest time: 1.622 milliseconds
Average time: 0.57268 milliseconds
```

---

## Overview of Optimization Strategies

1. **Mathematical Constraints**: Reducing the search space by applying arithmetic properties derived from the puzzle.
2. **Efficient Data Structures**: Utilizing bitmasking for fast digit usage tracking.
3. **Algorithmic Optimizations**: Avoiding unnecessary computations and leveraging early pruning.
4. **Multithreading**: Parallelizing computations to utilize multiple CPU cores effectively.
5. **Minimizing I/O Overhead**: Collecting solutions during computation and outputting them after execution.
6. **Compiler Optimizations**: Compiling with aggressive optimization flags to enhance performance.

---

## Detailed Explanation of Optimizations

### 1. Mathematical Constraints

**Objective**: Reduce the number of possible digit combinations by applying mathematical relationships derived from the puzzle.

#### a. Constraint on `T` and `H`
```
  NORTH 
+  EAST
+ SOUTH
+  WEST
= EARTH
```
From the last digits of the sum:

\[ 
H + T + H + T  is equivalent to H mod 10 
\]

Simplifies to:

\[ 
(H + 2T) mod 10 = 0 
\]

This equation allows us to precompute valid pairs of `T` and `H` where this condition holds and `T` is not equal to `H`. By filtering out invalid pairs early, the search space is significantly reduced.

#### b. Additional Analysis (Considered but Not Implemented)

While further column-wise addition analysis was conducted to derive more constraints, implementing them would introduce complexity without substantial performance gains. The overhead of solving simultaneous equations and adding conditional logic could negate the benefits.

### 2. Efficient Data Structures

**Objective**: Fast and efficient tracking of digit usage to ensure uniqueness without incurring significant overhead.

#### Bitmasking for Digit Tracking

- **Implementation**: An integer (e.g., `int used_digits`) is used as a bitmask to represent used digits.
- **Operation**:
  - Each bit corresponds to a digit from 0 to 9.
  - Setting a bit indicates that the digit is in use.
  - Checking if a digit is used involves a simple bitwise AND operation.
- **Advantages**:
  - Faster than using arrays or sets for tracking.
  - Minimizes memory footprint and maximizes cache efficiency.

**Example**:

```cpp
int used_digits = (1 << T) | (1 << H); // Marks digits T and H as used
if (used_digits & (1 << N))            // Checks if digit N is already used
    continue;
```

### 3. Algorithmic Optimizations

**Objective**: Reduce computation time by eliminating unnecessary calculations and leveraging early exits when possible.

#### a. Nested Loops for Digit Assignment

- **Approach**: Sequentially assign digits to letters using nested loops.
- **Optimization**:
  - If a digit is already used (checked via bitmask), the loop skips to the next iteration.
  - This ensures that no invalid combinations are processed further.

#### b. Early Pruning

- **Technique**: Immediately discard any combinations that violate the uniqueness constraint.
- **Benefit**: Prevents wasted computational effort on invalid paths.

### 4. Multithreading

**Objective**: Parallelize the computation to utilize multiple CPU cores and reduce overall execution time.

#### a. Workload Distribution

- **Strategy**: Distribute valid `T` values among available threads.
- **Implementation**:
  - Determine the number of hardware threads using `std::thread::hardware_concurrency()`.
  - Evenly divide the valid `T` values among the threads.
  - Each thread processes its assigned subset of `T` values independently.

#### b. Thread Safety

- **Shared Resources**: `total_solutions` counter and `solutions` vector.
- **Synchronization Mechanisms**:
  - **Atomic Variables**: `std::atomic<int>` for `total_solutions` allows for thread-safe increments without locks.
  - **Mutexes**: `std::mutex` to protect access to the `solutions` vector when appending new solutions.

#### c. Efficiency Considerations

- **Minimizing Lock Contention**:
  - The use of atomic operations and minimal locking reduces the overhead associated with thread synchronization.
- **Thread Overhead**:
  - The number of threads is limited to the number of hardware threads to prevent excessive context switching and scheduling overhead.

### 5. Minimizing I/O Overhead

**Objective**: Ensure that input/output operations do not negatively impact computation performance.

#### Solution Collection and Deferred Output

- **Approach**:
  - Collect solutions in a `vector` during computation.
  - Output all solutions after the computation and timing are complete.
- **Benefit**:
  - Eliminates the performance cost associated with frequent console output during computation.
  - Ensures that execution time measurements reflect only computation time.

### 6. Compiler Optimizations

**Objective**: Utilize compiler features to generate highly optimized machine code.

#### Compilation Flags

- **Flags Used**:
  - `-Ofast`: Enables aggressive optimizations, disregarding strict standards compliance for potential speed gains.
  - `-pthread`: Enables multi-threading support.

**Compilation Command**:

```bash
g++ -std=c++11 -pthread -O2 -march=native -mtune=native -flto -Ofast
```

#### Effects of Compiler Optimizations

- **Inlining Functions**: Reduces function call overhead by embedding function code directly.
- **Loop Unrolling**: Optimizes loops by reducing iteration overhead.
- **Vectorization**: Utilizes SIMD instructions where possible.

---

## Code Structure

### Main Components

1. **Main Function (`int main()`)**:
   - Initializes timing and data structures.
   - Determines the number of threads and divides the workload.
   - Creates and joins threads.
   - Outputs execution time and total solutions.
   - Prints all collected solutions.

2. **Worker Function (`void solve_range(...)`)**:
   - Processes a subset of `T` values assigned to a thread.
   - Performs the nested loops for digit assignment and equation checking.
   - Uses thread-safe mechanisms to update shared variables.

### Pseudocode Outline

```cpp
main() {
    start timing;
    determine number of threads;
    divide valid T values among threads;
    create threads to execute solve_range();
    join threads;
    stop timing;
    output execution time and total solutions;
    output all solutions;
}

solve_range(T_values) {
    for each T in T_values {
        calculate H;
        initialize used_digits bitmask;
        for N from 0 to 9 {
            if N is used, continue;
            update used_digits;
            // Continue for other letters: E, S, W, O, R, A, U
            if equation holds {
                increment total_solutions atomically;
                lock mutex;
                append solution to solutions vector;
                unlock mutex;
            }
        }
    }
}
```

---

## Conclusion

By integrating the aforementioned optimization techniques, the cryptarithmetic puzzle solver achieves high efficiency and reduced execution time while ensuring correctness. The combination of mathematical constraints, efficient data structures, algorithmic improvements, multithreading, and compiler optimizations results in a program that effectively leverages system resources to solve the puzzle.

---
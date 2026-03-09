# Linear System Solver

A C program that solves systems of linear equations using Gaussian elimination with partial pivoting. Originally implemented with Cramer's rule, but switched to Gaussian elimination for numerical stability and efficiency.

## Features

- Solves Ax = B for square matrices
- Uses Gaussian elimination with back-substitution
- Handles random coefficient generation
- Logs execution details to a file
- Command-line interface

## Requirements

- GCC compiler
- POSIX-compliant system (for timing functions)

## Compilation

```bash
gcc main.c -o solver
```

## Usage

Run the program:

```bash
./solver
```

Enter the number of equations (N) when prompted. The program will:
1. Generate a random N x N matrix A and vector B
2. Solve for x in Ax = B
3. Display execution time and first 10 solution values
4. Log all details to `cramer_execution_log.txt`

## Example

```
Enter number of equations (N) [e.g., 1000]: 3
```

Output shows solutions like:
```
x[0] = -0.108794
x[1] = -0.585415
x[2] = 0.438594
```

## Files

- `main.c`: Main program
- `matrix_math.h`: Matrix operations and Gaussian elimination
- `logger.h`: Logging utilities
- `sys_utils.h`: System utilities (CPU detection)
- `thread_solver.h`: Legacy threading code (not used)
- `cramer_execution_log.txt`: Execution log

## Notes

- For large N (>1000), computation may be slow due to O(N³) complexity
- Random coefficients are in range [-10, 10]
- Program assumes the matrix is non-singular
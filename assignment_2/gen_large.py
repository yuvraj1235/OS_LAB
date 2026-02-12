import random

N = 1000
with open("input.txt", "w") as f:
    for i in range(N):
        # Generate 1000 coefficients + 1 constant for each row
        row = [round(random.uniform(1.0, 10.0), 2) for _ in range(N + 1)]
        f.write(" ".join(map(str, row)) + "\n")

print(f"Generated 1000x1000 system in input.txt")
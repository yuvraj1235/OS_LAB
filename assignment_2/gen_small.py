import random

# Generate a small well-conditioned 10x10 system
N = 10
random.seed(42)
with open("small_input.txt", "w") as f:
    for i in range(N):
        for j in range(N):
            # Add diagonal dominance to ensure non-singular
            if i == j:
                f.write(f"{random.uniform(50, 100):.2f} ")
            else:
                f.write(f"{random.uniform(1, 5):.2f} ")
        f.write(f"{random.uniform(10, 50):.2f}\n")

print(f"Generated {N}x{N} well-conditioned system in small_input.txt")

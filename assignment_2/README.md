**AI-Generated System Solver — Project README**

**Overview:**
- **What:** A small C project that demonstrates solving a system of N linear equations using Cramer's rule with numerically stable log-arithmetic determinant computation.
- **Why:** Direct determinant multiplication can overflow/underflow for larger matrices. This project computes determinants in log-space (returning log(abs(det)) and sign) and parallelizes the per-variable numerator determinants to speed up the solution.

**Files:**
- `aiGen.c`: Main program that generates a random system, computes determinants, parallelizes worker processes, and prints solutions and runtime.
- `manMade.c`: (If present) an alternative or helper file — check your workspace.
- `README.md`: This file.

**Build & Run (macOS / zsh)**
- **Compile:** `gcc -O2 -o aiGen aiGen.c -lm`
- **Run:** `./aiGen`

When you run the program it will prompt for `N` (number of equations). The program generates random integer matrix entries and a random RHS vector (values 1..10), computes the solution using Cramer's rule (log-determinant arithmetic), and prints each `x[i]` and the total time.

**Implementation notes**
- **Numerical approach:** Determinants are computed with Gaussian elimination while accumulating the log of absolute diagonal entries; the sign of the determinant is tracked separately. This avoids overflow when multiplying many values.
- **Parallelization:** The program forks 8 worker processes. Each worker computes Di for a subset of indices, writes results to `res_<p>.txt`, and exits. The parent waits, aggregates results, and prints the final solution.
- **Memory:** Allocations use `malloc` and are freed after use — the program aims for explicit memory management.

**Original prompt (reconstructed)**
```
Write a C program that solves a system of N linear equations using Cramer's rule. Use log-arithmetic to compute determinants (return log(abs(det)) and sign) to avoid overflow/underflow. Generate a random matrix A and vector B with entries 1..10. Parallelize computation of numerator determinants Di across 8 worker processes using fork(); each worker writes `res_<p>.txt`. Parent collects results, computes solutions x[i], prints them and total runtime. Ensure memory safety and free allocations.
```

**Performance Comparison**
- **AI-generated code (`aiGen.c`):** 118.497647 seconds (measured on a random test case)
- **Hand-written code (`manMade.c`):** 116.529 seconds (measured on the same test case)
- **Difference:** ~1.97 seconds (AI-generated code ~1.7% slower)

Both implementations follow similar approaches (Cramer's rule with log-arithmetic and 8-way parallelization). The small difference suggests both are comparable in terms of optimization and efficiency for this workload.

**Token usage and notes**
- **User-reported (Gemini):** ~1480 tokens for the request/context.
- **OpenAI-style quick estimate:** the short prompt above is ~494 characters which heuristically maps to ~120–130 tokens (1 token ≈ 4 chars). This is only an approximation.

Why the counts differ
- Models use different tokenizers (Gemini may use a different encoding than `tiktoken`), and reported usage may include full conversation history, system messages, or code payloads, which increases total tokens.

How to get an exact token count
- For OpenAI-style models use the `tiktoken` Python package to encode your exact payload (system + user messages + code) and count tokens. Example:

```bash
pip install tiktoken
python - <<'PY'
import tiktoken
enc = tiktoken.encoding_for_model("gpt-3.5-turbo")
payload = """<paste the exact payload you sent to the model, including system and user messages>"""
print('tokens:', len(enc.encode(payload)))
PY
```

If you want me to estimate with `tiktoken`, I can run that here (for an OpenAI-style tokenizer) and give a breakdown.

**Tips to reduce tokens for submission to any model**
- Keep the prompt concise — focus on functional requirements only.
- Avoid sending full source files unless necessary; send a brief summary or the specific function(s) you want help with.
- Remove long explanatory comments, examples, or repeated code blocks from the request.

**Limitations & safety**
- This program demonstrates numeric techniques for small to moderate N. Cramer's rule is O(N^4) with naive Di recomputation; for large N prefer Gaussian elimination or LU decomposition directly for performance.


NAME            - MAIL ID

Yubraj Sau      - ys.24u10345@nitdgp.ac.in
Jyothi Kiran    - gjk.24u10352@nitdgp.ac.in
Yaswanth Kumar  - syk.24u10357@nitdgp.ac.in
Sudipto Maity   - sm.24u10318@nitdgp.ac.in
Aryan Kumar     - ak.24u10320@nitdgp.ac.in
Seepani Roshini - sr.24u10321@nitdgp.ac.in
Dinesh Baratan  - bd.24u10340@nitdgp.ac.in
Partha Rana     - pr.24u10323@nitdgp.ac.in

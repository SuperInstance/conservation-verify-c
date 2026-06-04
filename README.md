# conservation-verify-c

C implementation of conservation law verification for ternary agents.

## Components

- **ScaleSweep** — Run simulation at [10, 50, 100, 500, 1000, 5000] agents
- **InvariantChecker** — Verify avoidance ratio std < threshold across all scales
- **RegressionTest** — Check against 5 known conservation laws
- **StatisticalReport** — Formatted output with PASS/FAIL per scale

## Build & Test

```bash
gcc -o test_verify tests/test_verify.c src/conservation_verify.c -lm -Wall -O2
./test_verify
```

## Conservation Laws Verified

1. Avoidance ratio > 0.5 at all scales
2. Dominance ratio ~3:1 (simulation analog of 294:1 theoretical)
3. Avoidance ratio std-dev < 0.05 across scales
4. Interference rate > 0 at all scales
5. Scale values [10, 50, 100, 500, 1000, 5000] conserved

## License

MIT

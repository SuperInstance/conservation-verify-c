# Future Integration: conservation-verify-c

## Current State
C implementation of conservation law verification for ternary agents. Checks that energy, surprise, and population are conserved across ternary transformations, with violation reporting.

## Integration Opportunities

### With conservation-matrix-c
The matrix implements conservation; verification checks it. Together on ESP32: the matrix runs in the hot path (every cell tick), the verifier runs periodically (every N ticks). If the verifier finds a drift, it triggers a recalibration of the conservation matrix.

### With ternary-energy (Rust)
Rust provides the theoretical conservation framework; C provides the on-device verification. Cross-language consistency: the same conservation test in Rust and C must produce identical results. Any discrepancy is a bug.

### With ternary-failure
Conservation violations are failure events. When the verifier detects a violation, it feeds into the failure analysis framework. `FailureMode::Critical` for energy conservation violations (should never happen), `Negligible` for floating-point rounding drift.

## Potential in Mature Systems
In room-as-codespace, conservation verification is the integrity check. Every room runs the verifier. If a room violates conservation laws, it is flagged for investigation. PLATO aggregates verification results across the fleet, identifying systemic conservation issues before they cascade.

## Cross-Pollination Ideas
- Verification as a continuous integrity check — rooms self-audit their conservation compliance
- Cross-device verification: compare conservation matrices between devices running the same room
- Verification frequency tuning: verify often on volatile rooms, rarely on stable ones

## Dependencies for Next Steps
- Integration with conservation-matrix-c for hot-path + background verification
- Cross-validation test suite with Rust ternary-energy
- Violation reporting protocol for PLATO aggregation

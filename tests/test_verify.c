#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../src/conservation_verify.h"

/* ── Simple test framework ─────────────────────────────────────────── */

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg)                                                      \
    do {                                                                       \
        tests_run++;                                                           \
        if (cond) {                                                            \
            tests_passed++;                                                    \
            printf("  ✓ PASS: %s\n", msg);                                     \
        } else {                                                               \
            tests_failed++;                                                    \
            printf("  ✗ FAIL: %s\n", msg);                                     \
        }                                                                      \
    } while (0)

#define ASSERT_F(cond, fmt, ...)                                               \
    do {                                                                       \
        tests_run++;                                                           \
        if (cond) {                                                            \
            tests_passed++;                                                    \
            printf("  ✓ PASS: " fmt "\n", __VA_ARGS__);                        \
        } else {                                                               \
            tests_failed++;                                                    \
            printf("  ✗ FAIL: " fmt "\n", __VA_ARGS__);                        \
        }                                                                      \
    } while (0)

/* ── Test 1: scale_sweep_run returns success ───────────────────────── */

static void test_sweep_run_success(void) {
    printf("── Test 1: scale_sweep_run returns success ──\n");
    double a, d, i;
    int ok = scale_sweep_run(100, &a, &d, &i);
    ASSERT(ok == 1, "scale_sweep_run(100) returns success");
    ASSERT(a >= 0.0 && a <= 1.0, "avoidance ratio in [0, 1]");
    ASSERT(d >= 0.0, "dominance ratio non-negative");
    ASSERT(i >= 0.0 && i <= 1.0, "interference rate in [0, 1]");
}

/* ── Test 2: scale_sweep_run rejects invalid input ─────────────────── */

static void test_sweep_run_invalid(void) {
    printf("── Test 2: scale_sweep_run rejects invalid input ──\n");
    int ok = scale_sweep_run(0, NULL, NULL, NULL);
    ASSERT(ok == 0, "scale_sweep_run(0) returns failure");
}

/* ── Test 3: scale_sweep_all populates all scales ──────────────────── */

static void test_sweep_all(void) {
    printf("── Test 3: scale_sweep_all populates all scales ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);
    for (int i = 0; i < SCALE_COUNT; i++) {
        ASSERT_F(a[i] >= 0.0 && a[i] <= 1.0, "scale %d: avoidance in [0,1] = %.6f",
                 SCALE_VALUES[i], a[i]);
    }
}

/* ── Test 4: avoidance ratio > 0.5 at all scales ───────────────────── */

static void test_avoidance_above_half(void) {
    printf("── Test 4: avoidance ratio > 0.5 at all scales ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);
    ASSERT(invariant_check_avoidance_above_half(a) == 1,
           "avoidance ratio > 0.5 at every scale");
}

/* ── Test 5: avoidance std < threshold ─────────────────────────────── */

static void test_avoidance_std(void) {
    printf("── Test 5: avoidance std-dev < 0.05 ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);
    double std = 0;
    int ok = invariant_check_avoidance_std(a, 0.05, &std);
    ASSERT_F(ok == 1, "avoidance std = %.6f (< 0.05)", std);
}

/* ── Test 6: dominance ratio in expected range ─────────────────────── */

static void test_dominance_ratio(void) {
    printf("── Test 6: dominance ratio ~3:1 (±2 tolerance) ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);
    int ok = invariant_check_dominance_ratio(d, 3.0, 2.0);
    ASSERT(ok == 1, "dominance ratio within [1, 5] at all scales");
}

/* ── Test 7: interference rate is positive ──────────────────────────── */

static void test_interference_positive(void) {
    printf("── Test 7: interference rate > 0 at all scales ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);
    int ok = 1;
    for (int i = 0; i < SCALE_COUNT; i++) {
        if (inter[i] <= 0.0) {
            ok = 0;
            break;
        }
    }
    ASSERT(ok == 1, "interference rate > 0 at all scales");
}

/* ── Test 8: scale values are correct ──────────────────────────────── */

static void test_scale_values(void) {
    printf("── Test 8: scale values are [10, 50, 100, 500, 1000, 5000] ──\n");
    ASSERT(SCALE_COUNT == 6, "SCALE_COUNT == 6");
    ASSERT(SCALE_VALUES[0] == 10, "scale[0] == 10");
    ASSERT(SCALE_VALUES[1] == 50, "scale[1] == 50");
    ASSERT(SCALE_VALUES[2] == 100, "scale[2] == 100");
    ASSERT(SCALE_VALUES[3] == 500, "scale[3] == 500");
    ASSERT(SCALE_VALUES[4] == 1000, "scale[4] == 1000");
    ASSERT(SCALE_VALUES[5] == 5000, "scale[5] == 5000");
}

/* ── Test 9: regression tests all pass ─────────────────────────────── */

static void test_regression(void) {
    printf("── Test 9: all 5 regression tests pass ──\n");
    int passed = 0, failed = 0;
    int ok = regression_tests_run(&passed, &failed);
    ASSERT_F(passed == 5, "regression: %d passed, %d failed", passed, failed);
    ASSERT(ok == 1, "regression_tests_run returns 1 (all pass)");
}

/* ── Test 10: statistical report generation ─────────────────────────── */

static void test_report_generation(void) {
    printf("── Test 10: statistical report generation ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);

    statistical_report_t report;
    memset(&report, 0, sizeof(report));
    statistical_report_generate(&report, a, d, inter);

    ASSERT(report.scales[0].scale == 10, "report scales[0].scale == 10");
    ASSERT(report.scales[5].scale == 5000, "report scales[5].scale == 5000");
    ASSERT(report.avoidance_std >= 0.0, "report avoidance_std >= 0");
}

/* ── Test 11: statistical report printing ───────────────────────────── */

static void test_report_print(void) {
    printf("── Test 11: statistical report printing (visual check) ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);

    statistical_report_t report;
    statistical_report_generate(&report, a, d, inter);
    statistical_report_print(&report);
    ASSERT(1, "report printed without crash");
}

/* ── Test 12: deterministic results ────────────────────────────────── */

static void test_determinism(void) {
    printf("── Test 12: results are deterministic ──\n");
    double a1, d1, i1, a2, d2, i2;
    scale_sweep_run(100, &a1, &d1, &i1);
    scale_sweep_run(100, &a2, &d2, &i2);
    ASSERT(fabs(a1 - a2) < 1e-12, "avoidance ratio is deterministic");
    ASSERT(fabs(d1 - d2) < 1e-12, "dominance ratio is deterministic");
    ASSERT(fabs(i1 - i2) < 1e-12, "interference rate is deterministic");
}

/* ── Test 13: larger scale gives tighter stats ──────────────────────── */

static void test_scale_convergence(void) {
    printf("── Test 13: larger scale gives tighter stats ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);
    /* The interference rate at scale 5000 should be closer to theoretical 0.1
       than at scale 10 */
    double err_small = fabs(inter[0] - 0.1);
    double err_large = fabs(inter[5] - 0.1);
    ASSERT_F(err_large <= err_small,
             "interf. rate convergence: err(5000)=%.4f <= err(10)=%.4f",
             err_large, err_small);
}

/* ── Test 14: NULL output pointers don't crash ─────────────────────── */

static void test_null_outputs(void) {
    printf("── Test 14: NULL output pointers don't crash ──\n");
    int ok = scale_sweep_run(50, NULL, NULL, NULL);
    ASSERT(ok == 1, "scale_sweep_run with NULL outputs succeeds");
}

/* ── Test 15: invariant_check_avoidance_std with high threshold ────── */

static void test_std_high_threshold(void) {
    printf("── Test 15: invariant_check with high threshold always passes ──\n");
    double a[SCALE_COUNT], d[SCALE_COUNT], inter[SCALE_COUNT];
    scale_sweep_all(a, d, inter);
    double std = 0;
    int ok = invariant_check_avoidance_std(a, 100.0, &std);
    ASSERT(ok == 1, "std check passes with threshold=100");
}

/* ── Main ──────────────────────────────────────────────────────────── */

int main(void) {
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("  conservation-verify-c — Test Suite\n");
    printf("══════════════════════════════════════════════════════════════\n\n");

    test_sweep_run_success();
    test_sweep_run_invalid();
    test_sweep_all();
    test_avoidance_above_half();
    test_avoidance_std();
    test_dominance_ratio();
    test_interference_positive();
    test_scale_values();
    test_regression();
    test_report_generation();
    test_report_print();
    test_determinism();
    test_scale_convergence();
    test_null_outputs();
    test_std_high_threshold();

    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("  Results: %d/%d passed, %d failed\n", tests_passed, tests_run,
           tests_failed);
    printf("══════════════════════════════════════════════════════════════\n\n");

    return tests_failed > 0 ? 1 : 0;
}

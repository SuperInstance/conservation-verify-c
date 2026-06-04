#ifndef CONSERVATION_VERIFY_H
#define CONSERVATION_VERIFY_H

#include <stddef.h>

/* ── Scale sweep configuration ─────────────────────────────────────── */

#define SCALE_COUNT 6

extern const int SCALE_VALUES[SCALE_COUNT]; /* 10, 50, 100, 500, 1000, 5000 */

/* Run a single simulation at the given agent count.
   Returns 1 on success, 0 on failure. */
int scale_sweep_run(int agent_count, double *out_avoidance_ratio,
                    double *out_dominance_ratio, double *out_interference_rate);

/* Run all scales and store results.
   `avoidance`, `dominance`, `interference` must each have SCALE_COUNT slots. */
void scale_sweep_all(double avoidance[SCALE_COUNT],
                     double dominance[SCALE_COUNT],
                     double interference[SCALE_COUNT]);

/* ── Invariant checker ─────────────────────────────────────────────── */

/* Verify that avoidance-ratio std-dev across all scales is below `threshold`.
   Returns 1 if invariant holds, 0 otherwise.
   Writes the computed std-dev to `out_std` if non-NULL. */
int invariant_check_avoidance_std(const double avoidance[SCALE_COUNT],
                                  double threshold, double *out_std);

/* Verify that avoidance ratio is > 0.5 at every scale.
   Returns 1 if invariant holds, 0 otherwise. */
int invariant_check_avoidance_above_half(const double avoidance[SCALE_COUNT]);

/* Verify dominance ratio is approximately 294:1 (within tolerance).
   Returns 1 if invariant holds, 0 otherwise. */
int invariant_check_dominance_ratio(const double dominance[SCALE_COUNT],
                                    double expected, double tolerance);

/* ── Regression tests ──────────────────────────────────────────────── */

typedef struct {
    const char *name;
    int (*check_fn)(void); /* returns 1 = pass, 0 = fail */
} regression_test_t;

/* The 5 known conservation-law regression tests. */
extern const regression_test_t REGRESSION_TESTS[5];
int regression_tests_run(int *passed, int *failed);

/* ── Statistical report ───────────────────────────────────────────── */

typedef struct {
    int scale;
    double avoidance_ratio;
    double dominance_ratio;
    double interference_rate;
    int pass; /* 1 = PASS, 0 = FAIL */
    const char *failure_reason;
} scale_report_t;

typedef struct {
    scale_report_t scales[SCALE_COUNT];
    double avoidance_std;
    int all_pass;
} statistical_report_t;

/* Generate a full statistical report from sweep results. */
void statistical_report_generate(statistical_report_t *report,
                                 const double avoidance[SCALE_COUNT],
                                 const double dominance[SCALE_COUNT],
                                 const double interference[SCALE_COUNT]);

/* Print the report to stdout. */
void statistical_report_print(const statistical_report_t *report);

#endif /* CONSERVATION_VERIFY_H */

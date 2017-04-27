#include <stdio.h>

double compute_pi_baseline(size_t N);
double compute_pi_openmp(size_t N, int threads);
double compute_pi_avx(size_t N);
double compute_pi_avx_unroll(size_t N);

double compute_pi_leibniz(size_t N);
double compute_pi_leibniz_openmp(size_t N, int threads);
double compute_pi_leibniz_avx(size_t N);
double compute_pi_leibniz_avx_unroll(size_t N);

double compute_pi_euler(size_t N);
double compute_pi_euler_openmp(size_t N, int threads);
double compute_pi_euler_avx(size_t N);
double compute_pi_euler_avx_unroll(size_t N);

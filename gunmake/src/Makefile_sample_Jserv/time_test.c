#include <stdio.h>
#include "computepi.h"

int main(int argc, char const *argv[])
{
    __attribute__((unused)) int N = 400000000;
    double pi = 0.0;

#if defined(BASELINE)
    pi = compute_pi_baseline(N);
#endif

#if defined(OPENMP_2)
    pi = compute_pi_openmp(N, 2);
#endif

#if defined(OPENMP_4)
    pi = compute_pi_openmp(N, 4);
#endif

#if defined(AVX)
    pi = compute_pi_avx(N);
#endif

#if defined(AVXUNROLL)
    pi = compute_pi_avx_unroll(N);
#endif

#if defined(LEIBNIZ)
    pi = compute_pi_leibniz(N);
#endif

#if defined(LEIBNIZ_OPENMP_2)
    pi = compute_pi_leibniz_openmp(N, 2);
#endif

#if defined(LEIBNIZ_OPENMP_4)
    pi = compute_pi_leibniz_openmp(N, 4);
#endif

#if defined(LEIBNIZ_AVX)
    pi = compute_pi_leibniz_avx(N);
#endif

#if defined(LEIBNIZ_AVXUNROLL)
    pi = compute_pi_leibniz_avx_unroll(N);
#endif

#if defined(EULER)
    pi = compute_pi_leibniz(N);
#endif

#if defined(EULER_OPENMP_2)
    pi = compute_pi_leibniz_openmp(N, 2);
#endif

#if defined(EULER_OPENMP_4)
    pi = compute_pi_leibniz_openmp(N, 4);
#endif

#if defined(EULER_AVX)
    pi = compute_pi_leibniz_avx(N);
#endif

#if defined(EULER_AVXUNROLL)
    pi = compute_pi_leibniz_avx_unroll(N);
#endif
    printf("N = %d , pi = %lf\n", N, pi);

    return 0;
}

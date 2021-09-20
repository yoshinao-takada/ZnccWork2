#ifndef SUMKERNELC_H_
#define SUMKERNELC_H_
#include "ZnccC/ImageC.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    const float* base;
    int osRight, osBottom, osBottomRight;
} SumKernelC_t, *pSumKernelC_t;
typedef const SumKernelC_t *pcSumKernelC_t;

#define SumKernelC_SATSum(sk, index) ( \
        (sk)->base[index]  \
        - (sk)->base[index + (sk)->osRight] \
        - (sk)->base[index + (sk)->osBottom] \
        + (sk)->base[index + (sk)->osBottomRight])

#define MK_SUMKERNEL(image, sumRect) { \
    ImageC_BeginC(image) + sumRect[0] - 1 + (sumRect[1] - 1) * (image)->size[0], \
    sumRect[2], \
    sumRect[3] * (image)->size[0], \
    sumRect[2] + sumRect[3] * (image)->size[0] }

#ifdef __cplusplus
}
#endif
#endif /* SUMKERNELC_H_ */
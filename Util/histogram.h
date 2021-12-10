#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_
#include "ZnccC/Basic2DTypesC.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    float range[2];
    int counterCount;
    int *counters;
} Histogram_t, *pHistogram_t;
#define NULLHISTOGRAM_C { { 0.0f, 0.0f }, 0, (int*)NULL }

typedef const Histogram_t *pcHistogram_t;

int Histogram_New(pHistogram_t hist, float range0, float range1, int subrangeCount);

void Histogram_Delete(pHistogram_t hist);

/**
 * @brief Classify a number and count occurance frequency.
 * 
 * @param hist [in,out] histogram object
 * @param toDiscriminate [in] a number to classify.
 */
void Histogram_Classify(pHistogram_t hist, float toClassify);

/**
 * @brief get the center of a subrange of classification selected by index.
 * 
 * @param hist [in,out] histogram object
 * @param index [in] of counts
 * @return subrange center value
 */
float Histogram_SubrangeCenter(pcHistogram_t hist, int index);

/**
 * @brief get the width of a subrange
 * 
 * @param hist [in,out] histogram object
 */
float Histogram_SubrangeWidth(pcHistogram_t hist);
#ifdef __cplusplus
}
#endif
#endif /* HISTOGRAM_H_ */
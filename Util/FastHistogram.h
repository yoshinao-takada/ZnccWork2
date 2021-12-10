#ifndef FASTHISTOGRAM_H_
#define FASTHISTOGRAM_H_
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    float range[2];
    float scaling;
    int counterCount;
    int counterCountM1;
    int *counters;
} FastHistogram_t, *pFastHistogram_t;
#define NULLFASTHISTOGRAM_C { { 0.0f, 0.0f }, 0.0f, 0, 0, (int*)NULL }

typedef const FastHistogram_t *pcFastHistogram_t;

int FastHistogram_New(pFastHistogram_t hist, float range0, float range1, int subrangeCount);

void FastHistogram_Delete(pFastHistogram_t hist);

/**
 * @brief Classify a number and count occurance frequency.
 * 
 * @param hist [in,out] histogram object
 * @param toDiscriminate [in] a number to classify.
 */
void FastHistogram_Classify(pFastHistogram_t hist, float toClassify);

/**
 * @brief get the center of a subrange of classification selected by index.
 * 
 * @param hist [in,out] histogram object
 * @param index [in] of counts
 * @return subrange center value
 */
float FastHistogram_SubrangeCenter(pcFastHistogram_t hist, int index);

/**
 * @brief get the width of a subrange
 * 
 * @param hist [in,out] histogram object
 */
float FastHistogram_SubrangeWidth(pcFastHistogram_t hist);
#ifdef __cplusplus
}
#endif
#endif /* FASTHISTOGRAM_H_ */
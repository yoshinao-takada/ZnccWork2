#include "Util/FastHistogram.h"
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/param.h>

int FastHistogram_New(pFastHistogram_t hist, float range0, float range1, int subrangeCount)
{
    hist->range[0] = range0;
    hist->range[1] = range1;
    hist->counterCount = subrangeCount + 2;
    hist->counterCountM1 = hist->counterCount - 1;
    hist->counters = (int*)malloc(hist->counterCount);
    hist->scaling = subrangeCount / (range1 - range0);
    if (hist->counters == NULL)
    {
        FastHistogram_Delete(hist);
        return ENOMEM;
    }
    memset(hist->counters, 0, hist->counterCount * sizeof(*hist->counters));
    float subrangeWidth = FastHistogram_SubrangeWidth(hist);
    hist->range[0] -= subrangeWidth;
    hist->range[1] -= subrangeWidth;
    return EXIT_SUCCESS;
}

void FastHistogram_Delete(pFastHistogram_t hist)
{
    if (hist && hist->counters)
    {
        free(hist->counters);
        hist->counters = NULL;
    }
    hist->range[0] = hist->range[1] = hist->scaling = 0.0f;
    hist->counterCountM1 = hist->counterCount = 0;
}

void FastHistogram_Classify(pFastHistogram_t hist, float toClassify)
{
    float indexF = hist->scaling * (toClassify - hist->range[0]);
    int index = MAX(0, MIN(hist->counterCountM1, (int)indexF));
    hist->counters[index]++;
}

float FastHistogram_SubrangeCenter(pcFastHistogram_t hist, int index)
{
    float deltaRange = FastHistogram_SubrangeWidth(hist);
    return hist->range[0] + deltaRange * ((float)index + 1.5f);
}

float FastHistogram_SubrangeWidth(pcFastHistogram_t hist)
{
    return (hist->range[1] - hist->range[0]) / (float)(hist->counterCount - 2);
}
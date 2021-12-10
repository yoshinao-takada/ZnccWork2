#include "Util/histogram.h"
#include <errno.h>
#include <memory.h>
#include <sys/param.h>
#include <math.h>

int Histogram_New(pHistogram_t hist, float range0, float range1, int subrangeCount)
{
    hist->range[0] = range0;
    hist->range[1] = range1;
    hist->counterCount = subrangeCount + 2;
    hist->counters = (int*)malloc(hist->counterCount);
    if (hist->counters == NULL)
    {
        Histogram_Delete(hist);
        return ENOMEM;
    }
    memset(hist->counters, 0, hist->counterCount * sizeof(*hist->counters));
    return EXIT_SUCCESS;
}

void Histogram_Delete(pHistogram_t hist)
{
    if (hist->counters)
    {
        free(hist->counters);
        hist->counters = NULL;
    }
    hist->counterCount = 0;
    hist->range[0] = hist->range[1];
}

void Histogram_Classify(pHistogram_t hist, float toClassify)
{
    do {
        int i = 0;
        float deltaRange = Histogram_SubrangeWidth(hist);
        if (toClassify < hist->range[0])
        {
            hist->counters[i]++;
            break;
        }
        float subrange0 = -1.f;
        float subrange1 = hist->range[0];
        for (i = 1; i < hist->counterCount - 1; i++)
        {
            subrange0 = subrange1;
            subrange1 = subrange0 + deltaRange;
            if (subrange0 <= toClassify && toClassify < subrange1)
            {
                hist->counters[i]++;
                break;
            }
        }
        if (i == (hist->counterCount - 1))
        {
            hist->counters[i]++;
        }
    } while (0);
}

float Histogram_SubrangeCenter(pcHistogram_t hist, int index)
{
    float deltaRange = Histogram_SubrangeWidth(hist);
    return hist->range[0] + deltaRange * ((float)index + 0.5f);
}

float Histogram_SubrangeWidth(pcHistogram_t hist)
{
    return (hist->range[1] - hist->range[0]) / (float)(hist->counterCount - 2);
}

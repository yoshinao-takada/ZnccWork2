#include "Util/FastHistogram.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
static void ShowHistgramInfo(pcFastHistogram_t hist)
{
    fprintf(stderr, "subrange width = %f, subrange count = %d\n",
        FastHistogram_SubrangeWidth(hist), hist->counterCount);
    fprintf(stderr, "subrange center =");
    for (size_t i = 0; i != hist->counterCount - 2; i++)
    {
        fprintf(stderr, "%c%f", (i == 0) ? ' ':',', FastHistogram_SubrangeCenter(hist, (int)i));
    }
    fprintf(stderr, "\n\n");
}

static void ShowFastHistogramCounters(pcFastHistogram_t hist)
{
    fprintf(stderr, "histogram counters\n");
    for (int i = 0; i != hist->counterCount; i++)
    {
        fprintf(stderr, "%c%d", (i == 0)? ' ' : ',', hist->counters[i]);
    }
    fprintf(stderr, "\n\n");
}

static const float rangeExamples[][2] = 
{
    { 0.33f, 0.66f },
    { 4000.0f, 4500.0f },
    { -0.12f, 0.1f }
};
static const int subrangeCounts[] =
{
    3, 5, 11
};
static const size_t exampleCount = sizeof(subrangeCounts)/sizeof(subrangeCounts[0]);

static int UtilFastHistogramUT_InitDelete()
{
    int err = EXIT_SUCCESS;
    FastHistogram_t hist = NULLFASTHISTOGRAM_C;
    do {
        for (size_t i = 0; i != exampleCount; i++)
        {
            const float range0 = rangeExamples[i][0];
            const float range1 = rangeExamples[i][1];
            const int subrangeCount = subrangeCounts[i];
            if (EXIT_SUCCESS != (err = FastHistogram_New(&hist, range0, range1, subrangeCount)))
            {
                fprintf(stderr, "%s,%d,Fail in FastHistogram_New(),err= %d\n", __FUNCTION__, __LINE__, err);
                break;
            }
            ShowHistgramInfo(&hist);
            FastHistogram_Delete(&hist);
        }
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

static int UtilFastHistogramUT_Classify()
{
    int err = EXIT_SUCCESS;
    FastHistogram_t hist = NULLFASTHISTOGRAM_C;
    do {
        FastHistogram_New(&hist, rangeExamples[0][0], rangeExamples[0][1], subrangeCounts[0]);
        FastHistogram_Classify(&hist, 0.3f);
        FastHistogram_Classify(&hist, 0.33f);
        FastHistogram_Classify(&hist, (0.33f + 0.11f + 0.01f));
        FastHistogram_Classify(&hist, (0.33f + 0.11f*2.0f + 0.01f));
        FastHistogram_Classify(&hist, (0.33f + 0.11f*3.0f + 0.01f));
        for (int i = 0; i != hist.counterCount; i++)
        {
            if (hist.counters[i] != 1)
            {
                fprintf(stderr, "%s,%d,hist.counterCount mismatch, i = %d\n", __FUNCTION__, __LINE__, i);
                err = EXIT_FAILURE;
                break;
            }
        }
        if (err) break;
        FastHistogram_Classify(&hist, -2e10);
        FastHistogram_Classify(&hist, (0.33f + 0.11f - 0.1e-5f));
        FastHistogram_Classify(&hist, (0.33f + 0.11f + 0.001f));
        FastHistogram_Classify(&hist, (0.33f + 0.11f * 2 + 0.01f));
        FastHistogram_Classify(&hist, 100e5);
        for (int i = 0; i != hist.counterCount; i++)
        {
            if (hist.counters[i] != 2)
            {
                fprintf(stderr, "%s,%d,hist.counterCount mismatch, i = %d\n", __FUNCTION__, __LINE__, i);
                err = EXIT_FAILURE;
                break;
            }
        }
        if (err) break;
    } while (0);
    FastHistogram_Delete(&hist);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}


int UtilFastHistogramUT()
{
    int err = EXIT_SUCCESS;
    FastHistogram_t hist;
    do {
        if (EXIT_SUCCESS != (err = UtilFastHistogramUT_InitDelete()))
        {
            break;
        }
        if (EXIT_SUCCESS != (err = UtilFastHistogramUT_Classify()))
        {
            break;
        }
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}



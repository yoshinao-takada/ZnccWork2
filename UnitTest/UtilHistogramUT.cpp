#include "Util/histogram.h"
#include <stdio.h>
static void ShowHistgramInfo(pcHistogram_t hist)
{
    fprintf(stderr, "subrange width = %f, subrange count = %d\n",
        Histogram_SubrangeWidth(hist), hist->counterCount);
    fprintf(stderr, "subrange center =");
    for (size_t i = 0; i != hist->counterCount - 2; i++)
    {
        fprintf(stderr, "%c%f", (i == 0) ? ' ':',', Histogram_SubrangeCenter(hist, (int)i));
    }
    fprintf(stderr, "\n\n");
}

static void ShowHistogramCounters(pcHistogram_t hist)
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

static int UtilHistogramUT_InitDelete()
{
    int err = EXIT_SUCCESS;
    Histogram_t hist = NULLHISTOGRAM_C;
    do {
        for (size_t i = 0; i != exampleCount; i++)
        {
            const float range0 = rangeExamples[i][0];
            const float range1 = rangeExamples[i][1];
            const int subrangeCount = subrangeCounts[i];
            if (EXIT_SUCCESS != (err = Histogram_New(&hist, range0, range1, subrangeCount)))
            {
                fprintf(stderr, "%s,%d,Fail in Histogram_New(),err= %d\n", __FUNCTION__, __LINE__, err);
                break;
            }
            ShowHistgramInfo(&hist);
            Histogram_Delete(&hist);
        }
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

static int UtilHistogramUT_Classify()
{
    int err = EXIT_SUCCESS;
    Histogram_t hist = NULLHISTOGRAM_C;
    do {
        Histogram_New(&hist, rangeExamples[0][0], rangeExamples[0][1], subrangeCounts[0]);
        Histogram_Classify(&hist, 0.3f);
        Histogram_Classify(&hist, 0.33f);
        Histogram_Classify(&hist, (0.33f + 0.11f + 0.01f));
        Histogram_Classify(&hist, (0.33f + 0.11f*2.0f + 0.01f));
        Histogram_Classify(&hist, (0.33f + 0.11f*3.0f + 0.01f));
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
        Histogram_Classify(&hist, -2e10);
        Histogram_Classify(&hist, (0.33f + 0.11f - 0.1e-5f));
        Histogram_Classify(&hist, (0.33f + 0.11f + 0.001f));
        Histogram_Classify(&hist, (0.33f + 0.11f * 2 + 0.01f));
        Histogram_Classify(&hist, 100e5);
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
    Histogram_Delete(&hist);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}


int UtilHistogramUT()
{
    int err = EXIT_SUCCESS;
    Histogram_t hist;
    do {
        if (EXIT_SUCCESS != (err = UtilHistogramUT_InitDelete()))
        {
            break;
        }
        if (EXIT_SUCCESS != (err = UtilHistogramUT_Classify()))
        {
            break;
        }
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}



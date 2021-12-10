#include <cstdlib>
#include <cstdio>
int BasicsUT();
int UtilImageSourceUT();
int MeanScatteringUT();
int CostMapUT();
int BasicsCUT(); // Tests foor Basic2DTypesC.h
int MeanScatteringCUT(); // tests for ImageC.h and ZnccHalfC.h
int CostMapCUT(); // test for CostMapC.h and CostMapGenC.h
int UtilHistogramUT(); // test for Util/Histogram.h
int UtilFastHistogramUT(); // test for Util/FastHistogram.h

int main()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = BasicsUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
        if (EXIT_SUCCESS != (err = UtilImageSourceUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
        // if (EXIT_SUCCESS != (err = MeanScatteringUT()))
        // {
        //     fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        // }
        // if (EXIT_SUCCESS != (err = CostMapUT()))
        // {
        //     fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        // }
        // if (EXIT_SUCCESS != (err = BasicsCUT()))
        // {
        //     fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        // }
        // if (EXIT_SUCCESS != (err = MeanScatteringCUT()))
        // {
        //     fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        // }
        // if (EXIT_SUCCESS != (err = CostMapCUT()))
        // {
        //     fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        // }
        if (EXIT_SUCCESS != (err = UtilHistogramUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
        if (EXIT_SUCCESS != (err = UtilFastHistogramUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
    } while (0);
    fprintf(stderr, "%s,%d,err=%d\n", __FUNCTION__, __LINE__, err);
    return err;
}
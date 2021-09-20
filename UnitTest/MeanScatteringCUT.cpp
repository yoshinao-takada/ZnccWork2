#include "ZnccC/ZnccHalfC.h"
#include "Util/imagesource.h"
#include "Util/imagelog.h"
#include "Util/compare.h"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cerrno>
#define RELTOL  (1.0e-4f)
#define RELTOL2 (1.0e-3f)
#define WORKDIR             "../WorkData/"
#define WORKDIR_IMAGEC      WORKDIR "ImageC/"
static const RectC_t sumRect = { -15, -15, 31, 31 };
static const RectC_t searchRect = { -64, -2, 80, 5 };
static ZnccHalfC_t znccHalf = NULLZNCCHALF_C;
static SpecialImagesC_t si = NULLSPECIALIMAGE_C;
static Size2iC_t packedSize;
static RectC_t marginRect, ROI;
static Size2iC_t extendedSize;
static ImageC_t packedSource = NULLIMAGE_C;
static ImageC_t extendedSource = NULLIMAGE_C;
static ZnccMatchingSizesC_t sizes =
{
    {0,0},
    {0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0},
    {0,0,0,0}
};

static void SetSizes()
{
    COPY2(sizes.packedSize, packedSize);
    COPY4(sizes.searchRect, searchRect);
    COPY4(sizes.sumRect, sumRect);
    COPY4(sizes.marginRect, marginRect);
    COPY2(sizes.imageBufferSize, extendedSize);
    COPY4(sizes.imageRoi, ROI);
}

static void MeanScattering_CreateSource()
{
    int err = EXIT_SUCCESS;
    {
        ImageSource_MonotalSample(&packedSource);
        packedSize[0] = packedSource.roi[2];
        packedSize[1] = packedSource.roi[3];
        RectC_t marginRect_ = { -1 + sumRect[0] + searchRect[0], -1 + sumRect[1] + searchRect[1], 1 + sumRect[2] + searchRect[2], 1 + sumRect[3] + searchRect[3] };
        COPY4(marginRect, marginRect_);
        RectC_t roi_ = { -marginRect[0], -marginRect[1], packedSize[0], packedSize[1] };
        COPY4(ROI, roi_);
        extendedSize[0] = packedSize[0] + marginRect[2];
        extendedSize[1] = packedSize[1] + marginRect[3];
    }
    SetSizes();
}

static int MeanScattering_CreateZnccHalf()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = SpecialImagesC_Create(&si, extendedSize, ROI)))
        {
            fprintf(stderr, "%s,%d,fail in SpecialImagesC_Create()\n", __FUNCTION__, __LINE__);
            break;
        }
        
        if (EXIT_SUCCESS != (err = ImageC_SetPackedImageF(&extendedSource, packedSource.elements, extendedSize, ROI)))
        {
            fprintf(stderr, "%s,%d,fail in ImageC_SetPackedImageF()\n", __FUNCTION__, __LINE__);
            break;
        }

        if (EXIT_SUCCESS != (err = ZnccHalfC_SetImage(&znccHalf, &extendedSource, &sizes, &si)))
        {
            fprintf(stderr, "%s,%d,fail in ZnccHalfC_SetImage()\n", __FUNCTION__, __LINE__);
            break;
        }
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return 0;
}

static int MeanScattering_TestMean()
{
    int err = EXIT_SUCCESS;
    ImageC_t mean = NULLIMAGE_C;
    const Point2iC_t quaterPoint = { sizes.packedSize[0]/4, sizes.packedSize[1]/4 };
    const Point2iC_t watchPoints[] = // points where mean calculation tested at.
    {
        { quaterPoint[0], quaterPoint[1] },
        { 3 * quaterPoint[0], quaterPoint[1] },
        { quaterPoint[0], 3 * quaterPoint[1] },
        { 3 * quaterPoint[0], 3 * quaterPoint[1] }
    };
    const int watchPointsCount = sizeof(watchPoints)/sizeof(watchPoints[0]);
    do {
        if (EXIT_SUCCESS != (err = ImageC_Mean(&mean, &extendedSource, sizes.sumRect)))
        {
            fprintf(stderr, "%s,%d,fail in ImageC_Mean()\n", __FUNCTION__, __LINE__);
            break;
        }
        for (int iWatch = 0; iWatch != watchPointsCount; iWatch++)
        {
            const Point2iC_t& watchPoint = watchPoints[iWatch];
            float sum = 0.0f;
            for (int iSumRow = 0; iSumRow != sizes.sumRect[3]; ++iSumRow)
            {
                for (int iSumCol = 0; iSumCol != sizes.sumRect[2]; ++iSumCol)
                {
                    Point2iC_t sumOffset = { iSumCol + sizes.sumRect[0], iSumRow + sizes.sumRect[1] };
                    Point2iC_t sumPoint = { watchPoint[0] + sumOffset[0], watchPoint[1] + sumOffset[1] };
                    Point2iC_t extendedSumPoint = { extendedSource.roi[0] + sumPoint[0], extendedSource.roi[1] + sumPoint[1] };
                    int linearIndex = extendedSumPoint[0] + extendedSumPoint[1] * extendedSource.size[0];
                    sum += extendedSource.elements[linearIndex];
                }
            }
            float meanRef = sum / (float)AREA_RECT(sizes.sumRect);
            const Point2iC_t extendedWatchPoint = { watchPoint[0] + mean.roi[0], watchPoint[1] + mean.roi[1] };
            int linearIndexMean = extendedWatchPoint[0] + extendedWatchPoint[1] * mean.size[0];
            float meanZncc = znccHalf.mean.elements[linearIndexMean];
            float meanValue = mean.elements[linearIndexMean];
            bool b = AreEqualF(meanRef, meanValue, RELTOL) && AreEqualF(meanRef, meanZncc, RELTOL);
            if (!b)
            {
                fprintf(stderr, "%s,%d,(%d,%d):meanRef=%f, meanValue=%f, meanZncc=%f\n", __FUNCTION__, __LINE__, watchPoint[0], watchPoint[1], meanRef, meanValue, meanZncc);
            }
        }
    } while (0);
    ImageC_Delete(&mean);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

static void MeanScattering_Cleanup()
{
    ZnccHalfC_Delete(&znccHalf);
    SpecialImagesC_Delete(&si);
    ImageC_Delete(&packedSource);
    ImageC_Delete(&extendedSource);
}

static int MeanScattering_TestImage()
{
    int err = EXIT_SUCCESS;
    ImageC_t image0 = NULLIMAGE_C, image0Mean = NULLIMAGE_C;
    RectC_t roi = { 3, 3, 5, 5 };
    Size2iC_t size = { roi[2] + 6, roi[3] + 6 };
    do {
        ImageC_New(&image0, size, roi);
        float* image0Ptr = ImageC_Begin(&image0);
        for (int iRow = 0; iRow != roi[3]; iRow++)
        {
            for (int iCol = 0; iCol != roi[2]; iCol++)
            {
                image0Ptr[iCol] = (float)iRow + 0.1f * (float)iCol;
            }
            image0Ptr += size[0];
        }
        ImageLog_SaveMatrixImageCSVC(WORKDIR_IMAGEC, "image0.csv", &image0);
        ImageC_Integrate(&image0);
        ImageLog_SaveMatrixImageCSVC(WORKDIR_IMAGEC, "image0A.csv", &image0);
    } while (0);
    ImageC_Delete(&image0);
    ImageC_Delete(&image0Mean);
    return err;
}

static int MeanScattering_TestStddev()
{
    int err = EXIT_SUCCESS;
    const Point2iC_t quaterPoint = { sizes.packedSize[0]/4, sizes.packedSize[1]/4 };
    const Point2iC_t watchPoints[] = // points where mean calculation tested at.
    {
        { quaterPoint[0], quaterPoint[1] },
        { 3 * quaterPoint[0], quaterPoint[1] },
        { quaterPoint[0], 3 * quaterPoint[1] },
        { 3 * quaterPoint[0], 3 * quaterPoint[1] }
    };
    const int watchPointsCount = sizeof(watchPoints)/sizeof(watchPoints[0]);
    do {
        const float rcpSumArea1 = 1.0f / (AREA_RECT(sizes.sumRect) - 1);
        for (int i = 0; i != watchPointsCount; ++i)
        {
            const Point2iC_t& watchPoint = watchPoints[i];
            float sum = 0.0f;
            for (int iSumRectV = 0; iSumRectV != sizes.sumRect[3]; iSumRectV++)
            {
                for (int iSumRectH = 0; iSumRectH != sizes.sumRect[2]; iSumRectH++)
                {
                    const Point2iC_t sumRectPoint = { iSumRectH + sizes.sumRect[0], iSumRectV + sizes.sumRect[1] };
                    const Point2iC_t extendedPixelPoint = 
                    {
                        sumRectPoint[0] + watchPoint[0] + znccHalf.lumdev.roi[0], 
                        sumRectPoint[1] + watchPoint[1] + znccHalf.lumdev.roi[1]
                    };
                    const int linearIndex = extendedPixelPoint[0] + extendedPixelPoint[1] * znccHalf.lumdev.size[0];
                    const float deviation = znccHalf.lumdev.elements[linearIndex];
                    sum += (deviation * deviation);
                }
            }
            const float refStddev = sqrtf(rcpSumArea1 * sum);
            const Point2iC_t pointZncc = { znccHalf.lumdev.roi[0] + watchPoint[0], znccHalf.lumdev.roi[1] + watchPoint[1] };
            const int linearIndexZncc = pointZncc[0] + pointZncc[1] * znccHalf.lumdev.size[0];
            const float stddev = 1.0f / znccHalf.rcpstddev.elements[linearIndexZncc];
            const bool b = AreEqualF(refStddev, stddev, RELTOL2);
            if (!b)
            {
                fprintf(stderr, "%s,%d,stddev mismatch at (%d,%d), refStddev=%f, stddev=%f\n",
                    __FUNCTION__, __LINE__, watchPoint[0], watchPoint[1], refStddev, stddev);
            }
        }
    } while (0);
    return err;
}

int MeanScatteringCUT()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = MeanScattering_TestImage()))
        {
            fprintf(stderr, "%s,%d,Fail in MeanScattering_TestImage()\n", __FUNCTION__, __LINE__);
            break;
        }
        MeanScattering_CreateSource();
        if (EXIT_SUCCESS != (err = MeanScattering_CreateZnccHalf()))
        {
            fprintf(stderr, "%s,%d,Fail in MeanScattering_CreateZnccHalf()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = MeanScattering_TestMean()))
        {
            fprintf(stderr, "%s,%d,Fail in MeanScattering_TestMean()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = MeanScattering_TestStddev()))
        {
            fprintf(stderr, "%s,%d,Fail in MeanScattering_TestStddev()\n", __FUNCTION__, __LINE__);
            break;
        }
    } while (0);
    MeanScattering_Cleanup();
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}
#include <ZnccISPC/ZnccHalf.hpp>
#include <Util/imagesource.h>
#include <Util/imagelog.h>
#include <Util/compare.h>
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#define WORKDIR         "../WorkData/"
#define SRCIMAGE        "source/"
#define MEAN            "mean/"
#define STDDEV          "stddev/"
#define LUM             "lum/"
#define NULLIMAGE       { {{0,0}}, {{0,0,0,0}}, nullptr }
#define NULLZNCCHALF    { NULLIMAGE, NULLIMAGE, NULLIMAGE }
#define NULLSPECIALIMAGES { NULLIMAGE, NULLIMAGE, NULLIMAGE, NULLIMAGE, NULLIMAGE }
static ispc::Image imageSource = NULLIMAGE;
static ispc::Image imageMargined = NULLIMAGE;
static ispc::ZnccMatchingSizes sizes;
static ispc::ZnccHalf znccHalf = NULLZNCCHALF;
static ispc::SpecialImages si = NULLSPECIALIMAGES;

static uint8_t OnlyTypeConversion(float pixelValue)
{
    return (uint8_t)std::max<int32_t>(0, std::min<int32_t>(255, (int32_t)pixelValue));
}

static int Create()
{
    int err = EXIT_SUCCESS;
    ispc::ZnccError zerr = ispc::ZnccError::errSuccess;
    do {
        ImageSource_MonoralSample(imageSource);
        sizes.searchRect = {{-64, -3, 80, 7}};
        sizes.gridSize = sizes.packedSize = imageSource.size;
        sizes.sumRect = {{-15,-15,31,31}};
        sizes.marginRect = {{
            sizes.sumRect.v[0]-1 + sizes.searchRect.v[0],
            sizes.sumRect.v[1]-1 + sizes.searchRect.v[1],
            sizes.sumRect.v[2] + 1 + sizes.searchRect.v[2],
            sizes.sumRect.v[3] + 1 + sizes.searchRect.v[3]
        }};
        ispc::int32_t4 roi = {{
            -sizes.marginRect.v[0],
            -sizes.marginRect.v[1],
            sizes.packedSize.v[0] + sizes.marginRect.v[2],
            sizes.packedSize.v[1] + sizes.marginRect.v[3]
        }};
        zerr = ispc::Image_SetPackedImageF(imageMargined, imageSource.elements, imageSource.size, sizes.marginRect);
        if (zerr != ispc::ZnccError::errSuccess)
        {
            fprintf(stderr, "%s,%d,Fail in ispc::Image_SetPackedImageF()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        ImageLog_SaveImage(WORKDIR, SRCIMAGE, &imageMargined, OnlyTypeConversion);
        ispc::SpecialImages_Create(si, imageMargined.size, imageMargined.roi);
        
        zerr = ispc::ZnccHalf_SetImage(znccHalf, imageMargined, sizes, si);
        if (zerr != ispc::ZnccError::errSuccess)
        {
            fprintf(stderr, "%s,%d,Fail in ispc::Image_SetPackedImageF()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }

        // visualize znccHalf
        {
            ispc::Image imageVis = NULLIMAGE; // visualization work area
            ispc::Image_New(imageVis, znccHalf.lumdev.size, znccHalf.lumdev.roi);
            const size_t elementCount = imageVis.size.v[0] * imageVis.size.v[1];
            const size_t copySize = sizeof(float) * elementCount;
            memcpy(imageVis.elements, znccHalf.lumdev.elements, copySize);
            for (size_t i = 0; i != elementCount; i++)
            {
                imageVis.elements[i] += znccHalf.mean.elements[i];
            }
            ImageLog_SaveImage(WORKDIR, LUM, &imageVis, OnlyTypeConversion);
            ImageLog_SaveImage(WORKDIR, MEAN, &znccHalf.mean, OnlyTypeConversion);
            for (size_t i = 0; i != elementCount; i++)
            {
                imageVis.elements[i] = 1.0f / znccHalf.rcpsd.elements[i];
            }
            ImageLog_SaveImage(WORKDIR, STDDEV, &imageVis, OnlyTypeConversion);
            ispc::Image_Delete(imageVis);
        }
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

static int CheckNumeric()
{
    const ispc::int32_t4& roi = znccHalf.mean.roi;
    const ispc::int32_t2 watchPoints[] =
    {
        {{ roi.v[2]/4, roi.v[3]/4 }},
        {{ roi.v[2]/2, roi.v[3]/2 }},
        {{ 3 * roi.v[2]/4, 3 * roi.v[3]/4 }}
    };
    const size_t watchPointCount = sizeof(watchPoints) / sizeof(watchPoints[0]);
    int err = EXIT_SUCCESS;
    ispc::ZnccError zerr = ispc::ZnccError::errSuccess;
    const int stride = znccHalf.mean.size.v[0];
    const float rcpSumArea = 1.0f / (float)(sizes.sumRect.v[2] * sizes.sumRect.v[3]);
    const float rcpSumArea1 = 1.0f / (float)(sizes.sumRect.v[2] * sizes.sumRect.v[3] - 1);
    const float meanRelTol = 1.0e-3f;
    const float stddevRelTol = 1.0e-3f;
    do {
        // check mean
        for (int i = 0; i != watchPointCount; i++)
        {
            const ispc::int32_t2& watchPoint = watchPoints[i];
            const ispc::int32_t2 sumBegin = {{ 
                watchPoint.v[0] + sizes.sumRect.v[0] + imageSource.roi.v[0],
                watchPoint.v[1] + sizes.sumRect.v[1] + imageSource.roi.v[1]
            }};
            float sum = 0.0f;
            for (int iRow = 0; iRow != sizes.sumRect.v[3]; iRow++)
            {
                for (int iCol = 0; iCol != sizes.sumRect.v[2]; iCol++)
                {
                    const ispc::int32_t2 rawPoint = {{ watchPoint.v[0] + imageMargined.roi.v[0] + iCol + sizes.sumRect.v[0], watchPoint.v[1] + imageMargined.roi.v[1] + iRow + sizes.sumRect.v[1]}};
                    const int linearIndex = rawPoint.v[0] + rawPoint.v[1] * imageMargined.size.v[0];
                    sum += imageMargined.elements[linearIndex];
                }
            }
            const float watchPointMean = rcpSumArea * sum;
            const size_t watchPointLinearIndex = znccHalf.mean.roi.v[0] + watchPoint.v[0] + (znccHalf.mean.roi.v[1] + watchPoint.v[1]) * znccHalf.mean.size.v[0];
            if (AreNotEqualF(watchPointMean, znccHalf.mean.elements[watchPointLinearIndex], meanRelTol))
            {
                fprintf(stderr, "%s,%d,Point(%d,%d) mean mismatch(%f,%f)\n",
                    __FUNCTION__, __LINE__, watchPoint.v[0], watchPoint.v[1], watchPointMean, znccHalf.mean.elements[watchPointLinearIndex]);
                err = EXIT_FAILURE;
                break;
            }
        }
        if (err) break;
        fprintf(stderr, "%s,%d,znccHalf.mean matching test pass.\n", __FUNCTION__, __LINE__);

        // check stddev
        for (int i = 0; i != watchPointCount; i++)
        {
            const ispc::int32_t2& watchPoint = watchPoints[i];
            float sum = 0.0f;
            for (int iRow = 0; iRow != sizes.sumRect.v[3]; iRow++)
            {
                for (int iCol = 0; iCol != sizes.sumRect.v[2]; iCol++)
                {
                    const ispc::int32_t2 rawPoint = {{ watchPoint.v[0] + imageMargined.roi.v[0] + iCol + sizes.sumRect.v[0], watchPoint.v[1] + imageMargined.roi.v[1] + iRow + sizes.sumRect.v[1]}};
                    const int linearIndex = rawPoint.v[0] + rawPoint.v[1] * imageMargined.size.v[0];
                    sum += (znccHalf.lumdev.elements[linearIndex] * znccHalf.lumdev.elements[linearIndex]);
                }
            }
            const float watchPointStddev = sqrtf(sum * rcpSumArea1);
            const size_t watchPointLinearIndex = znccHalf.mean.roi.v[0] + watchPoint.v[0] + (znccHalf.mean.roi.v[1] + watchPoint.v[1]) * znccHalf.mean.size.v[0];
            const float rcpStddev = znccHalf.rcpsd.elements[watchPointLinearIndex];
            const float znccStddev = 1.0f / rcpStddev;
            if (AreNotEqualF(watchPointStddev, znccStddev, stddevRelTol))
            {
                fprintf(stderr, "%s,%d,Point(%d,%d) stddev mismatch(%f,%f)\n",
                    __FUNCTION__, __LINE__, watchPoint.v[0], watchPoint.v[1], watchPointStddev, znccStddev);
                err = EXIT_FAILURE;
                break;
            }
        }
        if (err) break;
        fprintf(stderr, "%s,%d,znccHalf.rcpsd matching test pass.\n", __FUNCTION__, __LINE__);
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

void Cleanup()
{
    ispc::Image_Delete(imageSource);
    ispc::Image_Delete(imageMargined);
    ispc::ZnccHalf_Delete(znccHalf);
    ispc::SpecialImages_Delete(si);
}

int MeanScatteringUT()
{
    int err = EXIT_SUCCESS;
    ispc::ZnccError zerr = ispc::ZnccError::errSuccess;
    do {
        if (EXIT_SUCCESS != (err = Create()))
        {
            fprintf(stderr, "%s,%d,Fail in Create(), err = %d\n", __FUNCTION__, __LINE__, err);
            break;
        }
        if (EXIT_SUCCESS != (err = CheckNumeric()))
        {
            fprintf(stderr, "%s,%d,Fail in CheckNumeric(), err = %d\n", __FUNCTION__, __LINE__, err);
            break;
        }
    } while (0);
    Cleanup();
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}
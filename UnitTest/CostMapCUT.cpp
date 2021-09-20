#include <ZnccC/CostMapGenC.h>
#include <Util/costtablelog.h>
#include <Util/imagelog.h>
#include <Util/imagesource.h>
#include <Util/compare.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <memory.h>
#define WORKDIR         "../WorkData/"

static ImageC_t imageBase = NULLIMAGE_C, imageShifted = NULLIMAGE_C;
static ImageC_t dispMapH = NULLIMAGE_C, dispMapV = NULLIMAGE_C;
static ZnccHalfC_t base = NULLZNCCHALF_C, shifted = NULLZNCCHALF_C;
static ZnccMatchingSizesC_t sizes =
{
    { 0, 0 }, // packedSize
    { 0, 0 }, // gridSize
    { -64, -2, 80, 5 }, // searchRect
    { -15, -15, 31, 31 }, // sumRect
    { 0, 0, 0, 0 }, // marginRect
    { 0, 0 }, // imageBufferSize
    { 0, 0, 0, 0 } // imageRoi
};
static SpecialImagesC_t siBase = NULLSPECIALIMAGE_C, siShifted = NULLSPECIALIMAGE_C;
static CostMapC_t costmap = NULLCOSTMAP_C;
static Point2iC_t watchPoints[4];
static const int watchPointCount = sizeof(watchPoints) / sizeof(watchPoints[0]);

static uint8_t OnlyTypeConversion(float pixelValue)
{
    return (uint8_t)std::max<int32_t>(0, std::min<int32_t>(255, (int32_t)pixelValue));
}

static uint8_t RangeAndTypeConversion(float pixelValue, float range0, float range1)
{
    int16_t normalized = (int16_t)(256.0f * (pixelValue - range0) / (range1 - range0));
    return (uint8_t)std::min<int16_t>(255, std::max<int16_t>(0, normalized));
}

static void Cleanup()
{
    ImageC_Delete(&imageBase);
    ImageC_Delete(&imageShifted);
    ImageC_Delete(&dispMapH);
    ImageC_Delete(&dispMapV);
    ZnccHalfC_Delete(&base);
    ZnccHalfC_Delete(&shifted);
    CostMapC_Delete(&costmap);
}

static int CreateStereo()
{
    int err = EXIT_SUCCESS;
    ImageC_t imageBaseSource = NULLIMAGE_C, imageShiftedSource = NULLIMAGE_C;
    ImageSource_StereoSamples(&imageShiftedSource, &imageBaseSource);
    COPY2(sizes.packedSize, imageBaseSource.size);
    {
        RectC_t marginRect =
        {
            sizes.searchRect[0] + sizes.sumRect[0] - 1,
            sizes.searchRect[1] + sizes.sumRect[1] - 1,
            sizes.searchRect[2] + sizes.sumRect[2] + 1,
            sizes.searchRect[3] + sizes.sumRect[3] + 1
        };
        COPY4(sizes.marginRect, marginRect);
        RectC_t imageRoi = { -marginRect[0], -marginRect[1], sizes.packedSize[0], sizes.packedSize[1] };
        COPY4(sizes.imageRoi, imageRoi);
        Size2iC_t imageBufferSize = { sizes.packedSize[0] + marginRect[2], sizes.packedSize[1] + marginRect[3] };
        COPY2(sizes.imageBufferSize, imageBufferSize);
    }
    do {
        if ((EXIT_SUCCESS != (err = ImageC_SetPackedImageF(&imageBase, imageBaseSource.elements, sizes.imageBufferSize, sizes.imageRoi))) ||
            (EXIT_SUCCESS != (err = ImageC_SetPackedImageF(&imageShifted, imageShiftedSource.elements, sizes.imageBufferSize, sizes.imageRoi))))
        {
            fprintf(stderr, "%s,%d,fail in ImageC_SetPackedImageF()\n", __FUNCTION__, __LINE__);
            break;
        }
        if ((EXIT_SUCCESS != (err = SpecialImagesC_Create(&siBase, sizes.imageBufferSize, sizes.imageRoi))) ||
            (EXIT_SUCCESS != (err = SpecialImagesC_Create(&siShifted, sizes.imageBufferSize, sizes.imageRoi))))
        {
            fprintf(stderr, "%s,%d,fail in SpecialImagesC_Create()\n", __FUNCTION__, __LINE__);
            break;
        }
        if ((EXIT_SUCCESS != (err = ZnccHalfC_SetImage(&base, &imageBase, &sizes, &siBase))) ||
            (EXIT_SUCCESS != (err = ZnccHalfC_SetImage(&shifted, &imageShifted, &sizes, &siShifted))))
        {
            fprintf(stderr, "%s,%d,fail in ZnccHalfC_SetImage()\n", __FUNCTION__, __LINE__);
            break;
        }
        ImageLog_SaveImageC(WORKDIR, "imageBaseC/", &imageBase, OnlyTypeConversion);
        ImageLog_SaveImageC(WORKDIR, "imageShiftedC/", &imageShifted, OnlyTypeConversion);
        ImageLog_SaveImageC(WORKDIR, "baseLumDevC/", &base.lumdev, OnlyTypeConversion);
        ImageLog_SaveImageC(WORKDIR, "shiftedLumDevC/", &shifted.lumdev, OnlyTypeConversion);
        ImageLog_SaveImageC(WORKDIR, "baseMeanC/", &base.mean, OnlyTypeConversion);
        ImageLog_SaveImageC(WORKDIR, "shiftedMeanC/", &shifted.mean, OnlyTypeConversion);
        if (EXIT_SUCCESS != (err = CostMapGenC_FillCostMap(&base, &shifted, &sizes, &costmap)))
        {
            fprintf(stderr, "%s,%d,fail in CostMapGen_FillCostMap()\n", __FUNCTION__, __LINE__);
            break;
        }
    } while (0);
    return err;
}

static void CreateWatchPoints()
{
    const int quaterPointH = sizes.packedSize[0] / 4;
    const int quaterPointV = sizes.packedSize[1] / 4;
    watchPoints[0][0] = quaterPointH;
    watchPoints[0][1] = quaterPointV;
    watchPoints[1][0] = 3 * quaterPointH;
    watchPoints[1][1] = quaterPointV;
    watchPoints[2][0] = quaterPointH;
    watchPoints[2][1] = 3 * quaterPointV;
    watchPoints[3][0] = 3 * quaterPointH;
    watchPoints[3][1] = 3 * quaterPointV;
}

static int SaveCostSearchTable(const Point2iC_t watchPoint)
{
    int err = EXIT_SUCCESS;
    CostSearchTableC_t cst = NULLCOSTSEARCHTABLE_C;
    do {
        if (EXIT_SUCCESS != (err = CostSearchTableC_New(&cst, &costmap)))
        {
            fprintf(stderr, "%s,%d,fail in CostSearchTableC_New()\n", __FUNCTION__, __LINE__);
        }
        CostSearchTableC_Gather(&cst, &costmap, watchPoint);
        CostTableLog_SaveCSVC(&cst, WORKDIR, "costtableC", watchPoint);
        Point2iC_t maxPoint;
        CostSearchTableC_FindMaximumPoint(&cst, maxPoint);
        fprintf(stderr, "%s,%d,maxPonit = (%d,%d)\n", __FUNCTION__, __LINE__, maxPoint[0], maxPoint[1]);
    } while (0);
    CostSearchTableC_Delete(&cst);
    return err;
}

static int GetDispMap()
{
    int err = EXIT_SUCCESS;
    CostSearchTableC_t cst = NULLCOSTSEARCHTABLE_C;
    do {
        if ((EXIT_SUCCESS != (err = ImageC_New(&dispMapH, sizes.imageBufferSize, sizes.imageRoi))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&dispMapV, sizes.imageBufferSize, sizes.imageRoi))))
        {
            fprintf(stderr, "%s,%d,fail in ImageC_New()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = CostSearchTableC_New(&cst, &costmap)))
        {
            fprintf(stderr, "%s,%d,fail in CostSearchTableC_New()\n", __FUNCTION__, __LINE__);
            break;
        }
        float* dispHPtr = ImageC_Begin(&dispMapH);
        float* dispVPtr = ImageC_Begin(&dispMapV);
        const int stride = sizes.imageBufferSize[0];
        for (int iRow = 0; iRow != sizes.imageRoi[3]; iRow++)
        {
            for (int iCol = 0; iCol != sizes.imageRoi[2]; iCol++)
            {
                Point2iC_t packedPixelPoint = { iCol, iRow };
                CostSearchTableC_Gather(&cst, &costmap, packedPixelPoint);
                Point2iC_t pointOfMaximumCost;
                CostSearchTableC_FindMaximumPoint(&cst, pointOfMaximumCost);
                dispHPtr[iCol] = pointOfMaximumCost[0];
                dispVPtr[iCol] = pointOfMaximumCost[1];
            }
            dispHPtr += stride;
            dispVPtr += stride;
        }
    } while(0);
    CostSearchTableC_Delete(&cst);
    return err;
}

int CostMapCUT()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = CreateStereo()))
        {
            fprintf(stderr, "%s,%d,fail in CreateStereo()\n", __FUNCTION__, __LINE__);
            break;
        }
        CreateWatchPoints();
        for (int i = 0; i != watchPointCount; i++)
        {
            SaveCostSearchTable(watchPoints[i]);
        }

        if (EXIT_SUCCESS != (err = GetDispMap()))
        {
            fprintf(stderr, "%s,%d,fail in GetDispMap()\n", __FUNCTION__, __LINE__);
            break;
        }
        ImageLog_SaveColorMapC(WORKDIR, "DispH_C", &dispMapH, RangeAndTypeConversion, -10.0, 4.0);
        ImageLog_SaveColorMapC(WORKDIR, "DispV_C", &dispMapV, RangeAndTypeConversion, -3.0, 3.0);
    } while (0);
    Cleanup();
    return err;
}
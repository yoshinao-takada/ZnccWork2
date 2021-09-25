#include <ZnccC/CostMapGenC.h>
#include <ZnccC/ImageC.h>
#include <Util/stopwatch.h>
#include <Util/imagelog.h>
#include <Util/imagesource.h>
#define WORKDIR         "../WorkData/"
// call back function for grayscale image logging
static uint8_t OnlyTypeConversion(float pixelValue)
{
    return (uint8_t)std::max<int32_t>(0, std::min<int32_t>(255, (int32_t)pixelValue));
}
// call back function for colormap image logging
static uint8_t RangeAndTypeConversion(float pixelValue, float range0, float range1)
{
    int16_t normalized = (int16_t)(256.0f * (pixelValue - range0) / (range1 - range0));
    return (uint8_t)std::min<int16_t>(255, std::max<int16_t>(0, normalized));
}

static ImageC_t baseImage = NULLIMAGE_C, shiftedImage = NULLIMAGE_C;
static ImageC_t dispmapH = NULLIMAGE_C, dispmapV = NULLIMAGE_C;
static SpecialImagesC_t baseSI = NULLSPECIALIMAGE_C, shiftedSI = NULLSPECIALIMAGE_C;
static ZnccHalfC_t base = NULLZNCCHALF_C, shifted = NULLZNCCHALF_C;
static CostMapC_t costmap = NULLCOSTMAP_C;
static CostSearchTableC_t cst = NULLCOSTSEARCHTABLE_C;
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

static void Cleanup()
{
    ImageC_Delete(&baseImage);
    ImageC_Delete(&shiftedImage);
    ImageC_Delete(&dispmapH);
    ImageC_Delete(&dispmapV);
    SpecialImagesC_Delete(&baseSI);
    SpecialImagesC_Delete(&shiftedSI);
    ZnccHalfC_Delete(&base);
    ZnccHalfC_Delete(&shifted);
    CostMapC_Delete(&costmap);
    CostSearchTableC_Delete(&cst);
}

static void CreateStereoImages()
{
    ImageC_t packedBase = NULLIMAGE_C, packedShifted = NULLIMAGE_C;
    ImageSource_StereoSamples(&packedShifted, &packedBase);
    COPY2(sizes.packedSize, packedBase.size);
    {
        const RectC_t marginRect = 
        {
            -1 + sizes.searchRect[0] + sizes.sumRect[0], -1 + sizes.searchRect[1] + sizes.sumRect[1],
            1 + sizes.searchRect[2] + sizes.sumRect[2], 1 + sizes.searchRect[3] + sizes.sumRect[3]
        };
        COPY4(sizes.marginRect, marginRect);
    }
    {
        const Size2iC_t imageBufferSize = 
        {
            sizes.marginRect[2] + sizes.packedSize[0], sizes.marginRect[3] + sizes.packedSize[1]
        };
        COPY2(sizes.imageBufferSize, imageBufferSize);
    }
    {
        const RectC_t imageRoi =
        {
            -sizes.marginRect[0], -sizes.marginRect[1], sizes.packedSize[0], sizes.packedSize[1]
        };
        COPY4(sizes.imageRoi, imageRoi);
    }
    int err = ImageC_SetPackedImageF(&baseImage, packedBase.elements, sizes.imageBufferSize, sizes.imageRoi);
    assert(err == EXIT_SUCCESS);
    err = ImageC_SetPackedImageF(&shiftedImage, packedShifted.elements, sizes.imageBufferSize, sizes.imageRoi);
    assert(err == EXIT_SUCCESS);
    ImageC_Delete(&packedBase);
    ImageC_Delete(&packedShifted);
    err = SpecialImagesC_Create(&baseSI, sizes.imageBufferSize, sizes.imageRoi);
    assert(err == EXIT_SUCCESS);
    err = SpecialImagesC_Create(&shiftedSI, sizes.imageBufferSize, sizes.imageRoi);
    assert(err == EXIT_SUCCESS);
}

static void CalcZnccHalf()
{
    int err = ZnccHalfC_SetImage(&base, &baseImage, &sizes, &baseSI);
    assert(EXIT_SUCCESS == err);
    err = ZnccHalfC_SetImage(&shifted, &shiftedImage, &sizes, &shiftedSI);
    assert(EXIT_SUCCESS == err);
}

static void CalcCostMap()
{
    int err = CostMapGenC_FillCostMap(&base, &shifted, &sizes, &costmap);
    assert(EXIT_SUCCESS == err);
}

static void CalcDispMap()
{
    const int stride = dispmapH.size[0];
    int err = CostSearchTableC_New(&cst, &costmap);
    assert(EXIT_SUCCESS == err);
    float* dispH = ImageC_Begin(&dispmapH);
    float* dispV = ImageC_Begin(&dispmapV);
    for (int iRow = 0; iRow != dispmapH.roi[3]; iRow++)
    {
        for (int iCol = 0; iCol != dispmapH.roi[2]; iCol++)
        {
            Point2iC_t pointOfMaximum = {0,0};
            Point2iC_t pixelPoint = { iCol, iRow };
            CostSearchTableC_Gather(&cst, &costmap, pixelPoint);
            CostSearchTableC_FindMaximumPoint(&cst, pointOfMaximum);
            dispH[iCol] = (float)pointOfMaximum[0];
            dispV[iCol] = (float)pointOfMaximum[1];
        }
        dispH += stride;
        dispV += stride;
    }
}
int PerfTestC()
{
    int err = EXIT_SUCCESS;
    struct timespec tRef = { 0, 0 };
    Stopwatch_Clear();
    do {
        // Step 1: Create stereo images
        CreateStereoImages();

        // Step 2: Calculate ZnccHalf
        timespec_get(&tRef, TIME_UTC);
        CalcZnccHalf();
        Stopwatch_Record("ZnccHalf:C", 0, tRef);

        // Step 3: Calculate Costmap
        timespec_get(&tRef, TIME_UTC);
        CalcCostMap();
        Stopwatch_Record("Costmap:C", 0, tRef);

        // Step 4: Calculate disparity map
        err = ImageC_New(&dispmapH, sizes.imageBufferSize, sizes.imageRoi);
        assert(EXIT_SUCCESS == err);
        err = ImageC_New(&dispmapV, sizes.imageBufferSize, sizes.imageRoi);
        assert(EXIT_SUCCESS == err);
        timespec_get(&tRef, TIME_UTC);
        CalcDispMap();
        Stopwatch_Record("Dispmap:C", 0, tRef);
        Stopwatch_Save(stderr);
        ImageLog_SaveColorMapC(WORKDIR, "PerfTestDispH_C", &dispmapH, RangeAndTypeConversion, -10.0, 4.0);
        ImageLog_SaveColorMapC(WORKDIR, "PerfTestDispV_C", &dispmapV, RangeAndTypeConversion, -3.0, 3.0);
    } while (0);
    Cleanup();
    return err;
}
#include <ZnccC/CostMapGenC.h>
#include <ZnccC/ImageC.h>
#include <Util/stopwatch.h>
#include <Util/imagelog.h>
#include <Util/imagesource.h>

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
    int err = ZnccHalfC_SetImage(&shifted, &shiftedImage, &sizes, &shiftedSI);
    assert(EXIT_SUCCESS == err);
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
        Stopwatch_Record("Costmap:C", 0, tRef);

        // Step 4: Calculate disparity map
        timespec_get(&tRef, TIME_UTC);
        Stopwatch_Record("Dispmap:C", 0, tRef);
    } while (0);
    Cleanup();
    return err;
}
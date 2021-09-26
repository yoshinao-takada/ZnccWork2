#include "ZnccISPC/ZnccHalf.hpp"
#include "ZnccISPC/CostMapGen.hpp"
#include "ZnccC/Basic2DTypesC.h"
#include "Util/stopwatch.h"
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
#define NULL_SIZE {{0,0}}
#define NULL_RECT {{0,0,0,0}}
#define NULL_IMAGE { NULL_SIZE, NULL_RECT, nullptr }
#define NULL_SPECIALIMAGES { NULL_IMAGE, NULL_IMAGE, NULL_IMAGE, NULL_IMAGE, NULL_IMAGE }
#define NULL_ZNCCHALF { NULL_IMAGE, NULL_IMAGE, NULL_IMAGE, NULL_IMAGE }
#define NULL_COSTMAP { NULL_SIZE, NULL_SIZE, nullptr }
#define NULL_COSTSEARCHTABLE { NULL_RECT, nullptr }

static ispc::Image baseImage = NULL_IMAGE, shiftedImage = NULL_IMAGE;
static ispc::Image dispmapH = NULL_IMAGE, dispmapV = NULL_IMAGE;
static ispc::SpecialImages baseSI = NULL_SPECIALIMAGES, shiftedSI = NULL_SPECIALIMAGES;
static ispc::ZnccHalf base = NULL_ZNCCHALF, shifted = NULL_ZNCCHALF;
static ispc::CostMap costmap = NULL_COSTMAP;
static ispc::CostSearchTable cst = NULL_COSTSEARCHTABLE;
static ispc::ZnccMatchingSizes sizes = 
{
    NULL_SIZE, // packedSize
    NULL_SIZE, // gridSize
    {{ -64, -2, 80, 5 }}, // searchRect
    {{ -15, -15, 31, 31 }}, // sumRect
    NULL_RECT, // marginRect
    NULL_SIZE, // imageBufferSize
    NULL_RECT // imageRoi
};

static void Cleanup()
{
    ispc::Image_Delete(baseImage);
    ispc::Image_Delete(shiftedImage);
    ispc::Image_Delete(dispmapH);
    ispc::Image_Delete(dispmapV);
    ispc::SpecialImages_Delete(baseSI);
    ispc::SpecialImages_Delete(shiftedSI);
    ispc::ZnccHalf_Delete(base);
    ispc::ZnccHalf_Delete(shifted);
    ispc::CostMap_Delete(costmap);
    ispc::CostSearchTable_Delete(cst);
}

static void CreateStereoImages()
{
    ispc::Image packedBase = NULL_IMAGE, packedShifted = NULL_IMAGE;
    ImageSource_StereoSamples(packedShifted, packedBase);
    COPY2(sizes.packedSize.v, packedBase.size.v);
    {
        const ispc::int32_t4 marginRect =
        {{
            -1 + sizes.searchRect.v[0] + sizes.sumRect.v[0], -1 + sizes.searchRect.v[1] + sizes.sumRect.v[1],
            1 + sizes.searchRect.v[2] + sizes.sumRect.v[2], 1 + sizes.searchRect.v[3] + sizes.sumRect.v[3]
        }};
        COPY4(sizes.marginRect.v, marginRect.v);
    }
    sizes.imageBufferSize.v[0] = sizes.marginRect.v[2] + sizes.packedSize.v[0];
    sizes.imageBufferSize.v[1] = sizes.marginRect.v[3] + sizes.packedSize.v[1];
    sizes.imageRoi.v[0] = -sizes.marginRect.v[0];
    sizes.imageRoi.v[1] = -sizes.marginRect.v[1];
    sizes.imageRoi.v[2] = sizes.packedSize.v[0];
    sizes.imageRoi.v[3] = sizes.packedSize.v[1];

    ispc::ZnccError zerr = ispc::Image_SetPackedImageF(baseImage, packedBase.elements, sizes.packedSize, sizes.marginRect);
    assert(zerr == ispc::ZnccError::errSuccess);
    zerr = ispc::Image_SetPackedImageF(shiftedImage, packedShifted.elements, sizes.packedSize, sizes.marginRect);
    assert(zerr == ispc::ZnccError::errSuccess);
    ispc::Image_Delete(packedBase);
    ispc::Image_Delete(packedShifted);
    ispc::SpecialImages_Create(baseSI, sizes.imageBufferSize, sizes.imageRoi);
    ispc::SpecialImages_Create(shiftedSI, sizes.imageBufferSize, sizes.imageRoi);
}

static void CalcZnccHalf()
{
    ispc::ZnccError zerr = ispc::ZnccHalf_SetImage(base, baseImage, sizes, baseSI);
    assert(zerr == ispc::ZnccError::errSuccess);
    zerr = ispc::ZnccHalf_SetImage(shifted, shiftedImage, sizes, shiftedSI);
    assert(zerr == ispc::ZnccError::errSuccess);
}

static void CalcCostMap()
{
    ispc::ZnccError zerr = ispc::CostMapGen_Fill(base, shifted, sizes, costmap);
    assert(zerr == ispc::ZnccError::errSuccess);
}

static void CalcDispMap()
{
    const int stride = dispmapH.size.v[0];
    ispc::ZnccError zerr = ispc::CostSearchTable_New(cst, sizes.searchRect);
    assert(zerr == ispc::ZnccError::errSuccess);
    float* dispH = dispmapH.elements + dispmapH.roi.v[0] + dispmapH.roi.v[1] * stride;
    float* dispV = dispmapV.elements + dispmapV.roi.v[0] + dispmapV.roi.v[1] * stride;
    for (int iRow = 0; iRow != dispmapH.roi.v[3]; iRow++)
    {
        for (int iCol = 0; iCol != dispmapH.roi.v[2]; iCol++)
        {
            ispc::int32_t2 pointOfMaximum = NULL_SIZE;
            ispc::int32_t2 pixelPoint = {{ iCol, iRow }};
            ispc::CostMap_Gather(costmap, cst, pixelPoint);
            ispc::CostSearchTable_FindMaximum(cst, 0.0f, pointOfMaximum);
            dispH[iCol] = (float)pointOfMaximum.v[0];
            dispV[iCol] = (float)pointOfMaximum.v[1];
        }
        dispH += stride;
        dispV += stride;
    }
}

int PerfTestISPC()
{
    int err = EXIT_SUCCESS;
    struct timespec tRef = { 0, 0 };
    do {
        // Step 1: Create stereo images
        CreateStereoImages();

        // Step 2: Calculate ZnccHalf
        timespec_get(&tRef, TIME_UTC);
        CalcZnccHalf();
        Stopwatch_Record("ZnccHalf:ISPC", 0, tRef);

        // Step 3: Calculate Costmap
        timespec_get(&tRef, TIME_UTC);
        CalcCostMap();
        Stopwatch_Record("Costmap:ISPC", 0, tRef);

        // Step 4: Calculate disparity map
        ispc::ZnccError zerr = ispc::Image_New(dispmapH, sizes.imageBufferSize, sizes.imageRoi);
        assert(ispc::ZnccError::errSuccess == zerr);
        err = ispc::Image_New(dispmapV, sizes.imageBufferSize, sizes.imageRoi);
        assert(ispc::ZnccError::errSuccess == zerr);
        timespec_get(&tRef, TIME_UTC);
        CalcDispMap();
        Stopwatch_Record("Dispmap:ISPC", 0, tRef);
        Stopwatch_Save(stderr);
        ImageLog_SaveColorMap(WORKDIR, "PerfTestDispH_ISPC", &dispmapH, RangeAndTypeConversion, -10.0, 4.0);
        ImageLog_SaveColorMap(WORKDIR, "PerfTestDispV_ISPC", &dispmapV, RangeAndTypeConversion, -3.0, 3.0);
    } while (0);
    return err;
}
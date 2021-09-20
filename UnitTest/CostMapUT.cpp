#include <ZnccISPC/ZnccHalf.hpp>
#include <ZnccISPC/CostMapGen.hpp>
#include <Util/imagesource.h>
#include <Util/imagelog.h>
#include <Util/compare.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <memory.h>
#define ENABLE_TASK_PARALLEL
#define WORKDIR         "../WorkData/"
#define NULLIMAGE       { {{0,0}}, {{0,0,0,0}}, nullptr }
#define NULLZNCCHALF    { NULLIMAGE, NULLIMAGE, NULLIMAGE }
#define NULLSPECIALIMAGES { NULLIMAGE, NULLIMAGE, NULLIMAGE, NULLIMAGE }
#define NULLCOSTSEARCHTABLE { {{ 0, 0, 0, 0 }}, nullptr }
typedef enum {
    BaseShifted_Base,
    BaseShifted_Shifted
} BaseShifted_t;

static ispc::Image imageBase = NULLIMAGE, imageShifted = NULLIMAGE;
static ispc::Image dispMapH = NULLIMAGE, dispMapV = NULLIMAGE;
static ispc::ZnccHalf base = NULLZNCCHALF, shifted = NULLZNCCHALF;
static ispc::SpecialImages si0 = NULLSPECIALIMAGES, si1 = NULLSPECIALIMAGES;
static ispc::ZnccMatchingSizes sizes =
{
    {{ 0, 0 }}, // packed size is retrieved from source images
    {{ 0, 0 }}, // grid size is not used now. It is intended to be used for sparse matching.
    {{ -64, -2, 80, 5 }}, // search rect
    {{ -15, -15, 31, 31 }}, // sum rect
    {{ 0, 0, 0, 0 }} // margin rect is derived  from search rect and sum rect
};
static std::vector<ispc::int32_t2> watchPoints = std::vector<ispc::int32_t2>(4);
static ispc::CostMap costmap = { {{0,0}}, {{0,0}}, nullptr };

static uint8_t OnlyTypeConversion(float pixelValue)
{
    return (uint8_t)std::max<int32_t>(0, std::min<int32_t>(255, (int32_t)pixelValue));
}

static uint8_t RangeAndTypeConversion(float pixelValue, float range0, float range1)
{
    int16_t normalized = (int16_t)(256.0f * (pixelValue - range0) / (range1 - range0));
    return (uint8_t)std::min<int16_t>(255, std::max<int16_t>(0, normalized));
}
static void SaveZnccHalf(const char* workdir, BaseShifted_t bs, const ispc::ZnccHalf& znccHalf)
{
    char subdir[64];
    snprintf(subdir, sizeof(subdir), "%s-mean/", (bs == BaseShifted_Base) ? "base" : "shifted");
    ImageLog_SaveImage(workdir, subdir, &znccHalf.mean, OnlyTypeConversion);
    snprintf(subdir, sizeof(subdir), "%s-lumdev/", (bs == BaseShifted_Base) ? "base" : "shifted");
    ImageLog_SaveImage(workdir, subdir, &znccHalf.lumdev, OnlyTypeConversion);
    snprintf(subdir, sizeof(subdir), "%s-random/", (bs == BaseShifted_Base) ? "base" : "shifted");
    ImageLog_SaveImage(workdir, subdir, &znccHalf.random, OnlyTypeConversion);
}

/**
 * @brief Create a Stero ispc::ZnccHalf objects 
 * @return int error code
 */
static int CreateStero()
{
    int err = EXIT_SUCCESS;
    ispc::Image imageBaseSource = NULLIMAGE, imageShiftedSource = NULLIMAGE;
    do {
        ImageSource_StereoSamples(imageShiftedSource, imageBaseSource);
        sizes.packedSize = imageBaseSource.size;
        ispc::ZnccMatchingSizes_Update(sizes);
        if ((ispc::ZnccError::errSuccess != ispc::Image_SetPackedImageF(imageBase, imageBaseSource.elements, sizes.packedSize, sizes.marginRect)) ||
            (ispc::ZnccError::errSuccess != ispc::Image_SetPackedImageF(imageShifted, imageShiftedSource.elements, sizes.packedSize, sizes.marginRect)))
        {
            fprintf(stderr, "%s,%d,fail in Image_SetPackedImageF().\n", __FUNCTION__, __LINE__);
            assert(false);
        }
        ispc::SpecialImages_Create(si0, imageBase.size, imageBase.roi);
        ispc::SpecialImages_Create(si1, imageBase.size, imageBase.roi);
        ispc::ZnccHalf_SetImage(base, imageBase, sizes, si0);
        ispc::ZnccHalf_SetImage(shifted, imageShifted, sizes, si1);
        SaveZnccHalf(WORKDIR, BaseShifted_Base, base);
        SaveZnccHalf(WORKDIR, BaseShifted_Shifted, shifted);
    } while (0);
    ispc::Image_Delete(imageBaseSource);
    ispc::Image_Delete(imageShiftedSource);
    return EXIT_SUCCESS;
}

static void Cleanup()
{
    ispc::Image_Delete(imageBase);
    ispc::Image_Delete(imageShifted);
    ispc::Image_Delete(dispMapH);
    ispc::Image_Delete(dispMapV);
    ispc::ZnccHalf_Delete(base);
    ispc::ZnccHalf_Delete(shifted);
    ispc::SpecialImages_Delete(si0);
    ispc::SpecialImages_Delete(si1);
}

static void CreateWatchPoints()
{
    watchPoints[0].v[0] = sizes.packedSize.v[0] / 4;
    watchPoints[0].v[1] = sizes.packedSize.v[1] / 4;

    watchPoints[1].v[0] = 3 * sizes.packedSize.v[0] / 4;
    watchPoints[1].v[1] = sizes.packedSize.v[1] / 4;

    watchPoints[2].v[0] = sizes.packedSize.v[0] / 4;
    watchPoints[2].v[1] = 3 * sizes.packedSize.v[1] / 4;

    watchPoints[3].v[0] = 3 * sizes.packedSize.v[0] / 4;
    watchPoints[3].v[1] = 3 * sizes.packedSize.v[1] / 4;
}


static int CreateCostMap()
{
    int err = EXIT_SUCCESS;
    ispc::ZnccError zerr = ispc::ZnccError::errSuccess;
    ispc::CostSearchTable cst = NULLCOSTSEARCHTABLE;
    do {
#ifdef ENABLE_TASK_PARALLEL
        zerr = CostMapGen_FillByTasks(base, shifted, sizes, costmap);
#else
        zerr = CostMapGen_Fill(base, shifted, sizes, costmap);
#endif
        if (ispc::ZnccError::errSuccess != zerr)
        {
            fprintf(stderr, "%s,%d,fail in CostMapGen_Fill()", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        CreateWatchPoints();
        zerr = ispc::CostSearchTable_New(cst, sizes.searchRect);
        for (const ispc::int32_t2& watchPoint : watchPoints)
        {
            char fname[64];
            snprintf(fname, sizeof(fname), "costs(%d,%d).csv", watchPoint.v[0], watchPoint.v[1]);
            ispc::CostMap_Gather(costmap, cst, watchPoint);
            ImageLog_SaveCostSearchTableCSV(WORKDIR "CostSearchTables/", fname, &cst);
        }
    } while (0);
    ispc::CostSearchTable_Delete(cst);
    return err;
}

static int MatchForward()
{
    ispc::CostSearchTable searchTable = NULLCOSTSEARCHTABLE;
    int err = EXIT_SUCCESS;
    ispc::ZnccError zerr = ispc::ZnccError::errSuccess;
    do {
        if ((ispc::ZnccError::errSuccess != (zerr = ispc::Image_New(dispMapH, imageBase.size, imageBase.roi))) ||
            (ispc::ZnccError::errSuccess != (zerr = ispc::Image_New(dispMapV, imageBase.size, imageBase.roi))) ||
            (ispc::ZnccError::errSuccess != (zerr = ispc::CostSearchTable_New(searchTable, sizes.searchRect))))
        {
            fprintf(stderr, "%s,%d,fail in ispc::Image_New() or ispc::CostSearchTable_New()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        for (int iRow = 0; iRow != dispMapH.roi.v[3]; iRow++)
        {
            for (int iCol = 0; iCol != dispMapH.roi.v[2]; iCol++)
            {
                ispc::int32_t2 packedPixelPoint = { iCol, iRow };
                ispc::int32_t2 disp = { INT32_MIN, INT32_MIN };
                ispc::CostMap_Gather(costmap, searchTable, packedPixelPoint);
                ispc::CostSearchTable_FindMaximum(searchTable, 0.5f, disp);
                ispc::int32_t2 extendedPixelPoint = { iCol + dispMapH.roi.v[0], iRow + dispMapH.roi.v[1] };
                const int32_t linearPixelIndex = extendedPixelPoint.v[0] + extendedPixelPoint.v[1] * dispMapH.size.v[0];
                dispMapH.elements[linearPixelIndex] = (float)disp.v[0];
                dispMapV.elements[linearPixelIndex] = (float)disp.v[1];
            }
        }
        ImageLog_SaveColorMap(WORKDIR, "DispMapH", &dispMapH, RangeAndTypeConversion, -10.0f, 4.0f);
        ImageLog_SaveColorMap(WORKDIR, "DispMapV", &dispMapV, RangeAndTypeConversion, -3.0f, 3.0f);
    } while (0);
    ispc::CostSearchTable_Delete(searchTable);
    return err;
}

int CostMapUT()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = CreateStero()))
        {
            fprintf(stderr, "%s,%d,fail in CreateStereo()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = CreateCostMap()))
        {
            fprintf(stderr, "%s,%d,fail in CreateCostMap()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = MatchForward()))
        {
            fprintf(stderr, "%s,%d,fail in MatchForward()\n", __FUNCTION__, __LINE__);
            break;
        }
    } while (0);
    Cleanup();
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return EXIT_SUCCESS;
}
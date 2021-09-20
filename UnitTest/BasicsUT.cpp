#include <ZnccISPC/Image.hpp>
#include <Util/imagelog.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#define ZERO_SIZE_DEF   {{0,0}}
#define ZERO_RECT_DEF   {{0,0,0,0}}
#define NULL_IMAGE { ZERO_SIZE_DEF, ZERO_RECT_DEF, nullptr }
#define WORKDIR     "../WorkData/"
#define IMAGE0DIR   "BasicImage0/"
#define IMAGE0INTEGRATEDIR  "BasicImage0Integrate/"
#define IMAGE1DIR   "BasicImage1/"
#define SUMRECT1    {{ 0, 0, 1, 1 }}
#define PACKED_SIZE {{ 5, 5 }}
#define MARGIN_RECT {{ -3, -3, 6, 6 }}

static ispc::Image image0 = NULL_IMAGE;
static ispc::Image image1 = NULL_IMAGE;
static const ispc::int32_t2 packedSize = PACKED_SIZE;
static const ispc::int32_t4 marginRect = MARGIN_RECT;
static const ispc::int32_t2 wholeSize = {{ packedSize.v[0] + marginRect.v[2], packedSize.v[1] + marginRect.v[3] }};
static const ispc::int32_t4 roi = {{ -marginRect.v[0], -marginRect.v[1], packedSize.v[0], packedSize.v[1] }};

int ImageIntegrateUT()
{
    int err = EXIT_SUCCESS;
    const int stride = wholeSize.v[0];
    do {
        ispc::Image_New(image0, wholeSize, roi);
        for (int iRow = 0; iRow != image0.roi.v[3]; iRow++)
        {
            for (int iCol = 0; iCol != image0.roi.v[2]; iCol++)
            {
                int linearIndex = iCol + image0.roi.v[0] + (iRow + image0.roi.v[1]) * stride;
                image0.elements[linearIndex] = (float)(iRow + iCol);
            }
        }
        ImageLog_SaveMatrixImageCSV(WORKDIR IMAGE0DIR, "matrix.csv", &image0);
        ispc::Image_Integrate(image0);
        ImageLog_SaveMatrixImageCSV(WORKDIR IMAGE0INTEGRATEDIR, "matrix.csv", &image0);
        image0.roi.v[0] -= 1; image0.roi.v[1] -= 1;
        image0.roi.v[2] += 3; image0.roi.v[3] += 3;
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

int ImageGetSumsUT()
{
    int err = EXIT_SUCCESS;
    do {
        ispc::Image_New(image1, image0.size, image0.roi);
        ispc::int32_t4 sumRect = { -1, -1, 2, 2 };
        ispc::Image_Sum(image0, sumRect, image1);
        ImageLog_SaveMatrixImageCSV(WORKDIR IMAGE1DIR, "matrix.csv", &image1);
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

void FillWithZero(ispc::Image& image)
{
    size_t area = image.size.v[0] * image.size.v[1];
    for (size_t i = 0; i != area; i++)
    {
        image.elements[i] = 0.0f;
    }
}

int BasicsUT()
{
    int err = EXIT_SUCCESS;
    do {
        err = ImageIntegrateUT();
        if (EXIT_SUCCESS != err)
        {
            fprintf(stderr, "%s,%d,Fail in ImageIntegrateUT()\n", __FUNCTION__, __LINE__);
        }
        err = ImageGetSumsUT();
        if (EXIT_SUCCESS != err)
        {
            fprintf(stderr, "%s,%d,Fail in ImageGetSumsUT()\n", __FUNCTION__, __LINE__);
        }
    } while (0);
    ispc::Image_Delete(image0);
    ispc::Image_Delete(image1);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}
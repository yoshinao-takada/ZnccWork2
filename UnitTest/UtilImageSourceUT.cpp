#include <Util/imagesource.h>
#include <Util/imagelog.h>
#include <cstdlib>
#include <cstdio>
#define ZERO_SIZE   {{0,0}}
#define ZERO_RECT   {{0,0,0,0}}
#define NULL_IMAGE { ZERO_SIZE, ZERO_RECT, nullptr }
#define WORKDIR     "../WorkData/"
#define IMAGESOURCE_DIR "ImageSource/"

static uint8_t OnlyTypeConversion(float pixelValue)
{
    return (uint8_t)std::max<int32_t>(0, std::min<int32_t>(255, (int32_t)pixelValue));
}

int UtilImageSourceUT()
{
    const ispc::int32_t2 imageSize = {{640, 480}};
    const ispc::int32_t2 patternScale = {{20, 20}};
    const ispc::int32_t4 imageRect = {{0,0,imageSize.v[0], imageSize.v[1]}};
    const std::vector<ispc::int32_t2> shifts =
    {
        {{ -8, -2 }}, {{ -9, 1 }}, {{ 5, 0 }}, {{ 0, 0 }}
    };
    ispc::ZnccError zerr = ispc::ZnccError::errSuccess;
    int err = EXIT_SUCCESS;
    ispc::Image image0 = NULL_IMAGE, image1 = NULL_IMAGE;
    do {
        if ((ispc::ZnccError::errSuccess != (zerr = ispc::Image_New(image0, imageSize, imageRect))) ||
            (ispc::ZnccError::errSuccess != (zerr = ispc::Image_New(image1, imageSize, imageRect))))
        {
            fprintf(stderr, "%s,%d,Fail in ispc::Image_New()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        ImageSource_Create(image0, patternScale);
        ImageSource_Shift(image1, image0, shifts);
        ImageLog_SaveImage(WORKDIR, "Image0", &image0, OnlyTypeConversion);
        ImageLog_SaveImage(WORKDIR, "Image1", &image1, OnlyTypeConversion);
    } while (0);
    ispc::Image_Delete(image0);
    ispc::Image_Delete(image1);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}

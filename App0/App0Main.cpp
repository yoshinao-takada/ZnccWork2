#include <ZnccC/Basic2DTypesC.h>
#include <ZnccC/CostMapGenC.h>
#include <Util/imagelog.h>
#include <App0/AppOpt.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define WORKDIR "../WorkData/"
#define SUBDIR_DISPH "App0DispH"
#define SUBDIR_DISPV "App0DispV"
#define SUBDIR_DISPH_REVERSEMATCHING "App0DispHR"
#define SUBDIR_DISPV_REVERSEMATCHING "App0DispVR"

#define HOW_TO_USE \
    "How to use app:\n" \
    "\tapp imageL-filename imageR-filename [option-filename]\n"
// call back function for colormap image logging
static uint8_t RangeAndTypeConversion(float pixelValue, float range0, float range1)
{
    int16_t normalized = (int16_t)(256.0f * (pixelValue - range0) / (range1 - range0));
    return (uint8_t)std::min<int16_t>(255, std::max<int16_t>(0, normalized));
}

static int AreInvalidImages(const cv::Mat1b& img0, const cv::Mat1b& img1)
{
    int err = EXIT_SUCCESS;
    do {
        if (img0.cols != img1.cols || img0.rows != img1.rows)
        {
            fprintf(stderr, "image size mismatch, {img0.rows,img1.cols}={%d,%d}, {img1.rows,img.cols}={%d,%d}\n",
                img0.rows, img0.cols, img1.rows, img1.cols);
            err = EINVAL;
            break;
        }
        if (img0.cols < 100 || img1.cols < 100 || img0.rows < 100 || img1.rows < 100)
        {
            fprintf(stderr, "image too small, {img0.rows,img1.cols}={%d,%d}, {img1.rows,img.cols}={%d,%d}\n",
                img0.rows, img0.cols, img1.rows, img1.cols);
            err = EINVAL;
            break;
        }
    } while (0);
    return err;
}
int main(int argc, char* argv[])
{
    int err = EXIT_SUCCESS;
    cv::Mat1b cvimgL, cvimgR, cvimgL_noscale, cvimgR_noscale;
    AppOpt_t opt = {DEFAULT_SEARCHRECT, DEFAULT_SUMRECT, DEFAULT_SCALE};
    ImageC_t imgL = NULLIMAGE_C, imgR = NULLIMAGE_C;
    ImageC_t dispH = NULLIMAGE_C, dispV = NULLIMAGE_C;
    ZnccHalfC_t znccL = NULLZNCCHALF_C, znccR = NULLZNCCHALF_C;
    SpecialImagesC_t siL = NULLSPECIALIMAGE_C, siR = NULLSPECIALIMAGE_C;

    do {
        if (argc < 3)
        {
            fprintf(stderr, "%s", HOW_TO_USE);
            err = EINVAL;
            break;
        }
        // Initialize
        cvimgL_noscale = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
        cvimgR_noscale = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);
        if (argc >= 4)
        {
           if (EXIT_SUCCESS != (err = AppOpt_ReadFile(&opt, argv[3])))
           {
               break;
           }
           if (opt.scale[0] == opt.scale[1])
           {
               opt.scale[0] = opt.scale[1] = 1;
               cvimgL = cvimgL_noscale.clone();
               cvimgR = cvimgR_noscale.clone();
           }
           else
           {
               cv::Size2i newSize(
                   opt.scale[0] * cvimgL_noscale.cols / opt.scale[1],
                   opt.scale[0] * cvimgL_noscale.rows / opt.scale[1]);
               cv::resize(cvimgL_noscale, cvimgL, newSize, 0.0, 0.0, cv::INTER_LINEAR);
               cv::resize(cvimgR_noscale, cvimgR, newSize, 0.0, 0.0, cv::INTER_LINEAR);
           }
        }
        if (EXIT_SUCCESS != (err = AreInvalidImages(cvimgL, cvimgR)))
        {
            break;
        }
        ZnccMatchingSizesC_t sizes =
        {
            .packedSize = { cvimgL.cols, cvimgL.rows },
            .gridSize = { cvimgL.cols, cvimgL.rows },
            .searchRect = { opt.searchRect[0], opt.searchRect[1], opt.searchRect[2], opt.searchRect[3] },
            .sumRect = { opt.sumRect[0], opt.sumRect[1], opt.sumRect[2], opt.sumRect[3] },
            .marginRect = {
                opt.searchRect[0] + opt.sumRect[0] - 1, opt.searchRect[1] + opt.sumRect[1] - 1,
                opt.searchRect[2] + opt.sumRect[2] + 1, opt.searchRect[3] + opt.sumRect[3] + 1 },
            .imageBufferSize = {
                cvimgL.cols + opt.searchRect[2] + opt.sumRect[2] + 1,
                cvimgL.rows + opt.searchRect[3] + opt.sumRect[3] + 1 },
            .imageRoi = {
                -(opt.searchRect[0] + opt.sumRect[0] - 1), -(opt.searchRect[1] + opt.sumRect[1] - 1),
                cvimgL.cols, cvimgL.rows }
        };

        // Matching
        {
            if ((EXIT_SUCCESS != (err = ImageC_SetPackedImage(&imgL, cvimgL.data, sizes.imageBufferSize, sizes.imageRoi))) ||
                (EXIT_SUCCESS != (err = ImageC_SetPackedImage(&imgR, cvimgR.data, sizes.imageBufferSize, sizes.imageRoi))))
            {
                fprintf(stderr, "%s,%d,Error\n", __FUNCTION__, __LINE__);
                break;
            }
            srand(1);
            if ((EXIT_SUCCESS != (err = SpecialImagesC_Create(&siL, sizes.imageBufferSize, sizes.imageRoi))) ||
                (EXIT_SUCCESS != (err = SpecialImagesC_Create(&siR, sizes.imageBufferSize, sizes.imageRoi))) ||
                (EXIT_SUCCESS != (err = ZnccHalfC_SetImage(&znccL, &imgL, &sizes, &siL))) ||
                (EXIT_SUCCESS != (err = ZnccHalfC_SetImage(&znccR, &imgR, &sizes, &siR))))
            {
                fprintf(stderr, "%s,%d,Error\n", __FUNCTION__, __LINE__);
                break;
            }
            ImageC_Delete(&imgL);
            ImageC_Delete(&imgR);
            if (EXIT_SUCCESS != (err = CostMapGen_FindMax(&znccL, &znccR, &sizes, &dispH, &dispV)))
            {
                fprintf(stderr, "%s,%d,Error\n", __FUNCTION__, __LINE__);
                break;
            }
            SpecialImagesC_Delete(&siL);
            SpecialImagesC_Delete(&siR);
            ZnccHalfC_Delete(&znccL);
            ZnccHalfC_Delete(&znccR);

            // Save dispmaps
            ImageLog_SaveColorMapC(WORKDIR, SUBDIR_DISPH, &dispH, RangeAndTypeConversion, -40.0f, 10.0f);
            ImageLog_SaveColorMapC(WORKDIR, SUBDIR_DISPV, &dispV, RangeAndTypeConversion, -20.0f, 20.0f);

            ImageC_Delete(&dispH);
            ImageC_Delete(&dispV);
        }

        // Flip searchRect
        {
            const RectC_t flipSearchRect = FLIP_RECT(sizes.searchRect);
            COPY4(sizes.searchRect, flipSearchRect);
            const RectC_t flipMarginRect = {
                sizes.searchRect[0] + sizes.sumRect[0] - 1, sizes.searchRect[1] + sizes.sumRect[1] - 1,
                sizes.searchRect[2] + sizes.sumRect[2] + 1, sizes.searchRect[3] + sizes.sumRect[3] + 1 };
            COPY4(sizes.marginRect, flipMarginRect);
            const RectC_t flipImageRoi = {
                -(sizes.searchRect[0] + sizes.sumRect[0] - 1), -(sizes.searchRect[1] + sizes.sumRect[1] - 1),
                cvimgL.cols, cvimgL.rows };
            COPY4(sizes.imageRoi, flipImageRoi);
        }
        // Reverse matching
        {
            if ((EXIT_SUCCESS != (err = ImageC_SetPackedImage(&imgL, cvimgR.data, sizes.imageBufferSize, sizes.imageRoi))) ||
                (EXIT_SUCCESS != (err = ImageC_SetPackedImage(&imgR, cvimgL.data, sizes.imageBufferSize, sizes.imageRoi))))
            {
                fprintf(stderr, "%s,%d,Error\n", __FUNCTION__, __LINE__);
                break;
            }
            srand(1);
            if ((EXIT_SUCCESS != (err = SpecialImagesC_Create(&siL, sizes.imageBufferSize, sizes.imageRoi))) ||
                (EXIT_SUCCESS != (err = SpecialImagesC_Create(&siR, sizes.imageBufferSize, sizes.imageRoi))) ||
                (EXIT_SUCCESS != (err = ZnccHalfC_SetImage(&znccL, &imgL, &sizes, &siL))) ||
                (EXIT_SUCCESS != (err = ZnccHalfC_SetImage(&znccR, &imgR, &sizes, &siR))))
            {
                fprintf(stderr, "%s,%d,Error\n", __FUNCTION__, __LINE__);
                break;
            }
            ImageC_Delete(&imgL);
            ImageC_Delete(&imgR);
            if (EXIT_SUCCESS != (err = CostMapGen_FindMax(&znccL, &znccR, &sizes, &dispH, &dispV)))
            {
                fprintf(stderr, "%s,%d,Error\n", __FUNCTION__, __LINE__);
                break;
            }
            SpecialImagesC_Delete(&siL);
            SpecialImagesC_Delete(&siR);
            ZnccHalfC_Delete(&znccL);
            ZnccHalfC_Delete(&znccR);
            ImageC_Negate(&dispH);
            ImageC_Negate(&dispV);

            // Save dispmaps
            ImageLog_SaveColorMapC(WORKDIR, SUBDIR_DISPH_REVERSEMATCHING, &dispH, RangeAndTypeConversion, -40.0f, 10.0f);
            ImageLog_SaveColorMapC(WORKDIR, SUBDIR_DISPV_REVERSEMATCHING, &dispV, RangeAndTypeConversion, -20.0f, 20.0f);

            ImageC_Delete(&dispH);
            ImageC_Delete(&dispV);
        }
    } while (0);
    return err;
}
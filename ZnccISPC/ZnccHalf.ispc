#include "ZnccHalf.isph"
#include "SumKernel.isph"

void ZnccHalf_New(uniform ZnccHalf& znccHalf, uniform const Size2i& size, uniform const Rect& roi)
{
    if (AreNotEqual(znccHalf.lumdev.size, size) || AreNotEqual(znccHalf.lumdev.roi, roi))
    {
        Image_New(znccHalf.lumdev, size, roi);
        Image_New(znccHalf.mean, size, roi);
        Image_New(znccHalf.rcpsd, size, roi);
    }
}

export uniform ZnccError ZnccHalf_SetImage(
    uniform ZnccHalf& znccHalf, uniform const Image& image,
    uniform const ZnccMatchingSizes& sizes, uniform const SpecialImages& si
) {
    Image_Clone(znccHalf.lumdev, image);
    Image_Add(znccHalf.lumdev, si.RandBgZeroFg); // fill background with random
    Image_New(znccHalf.mean, image.size, image.roi);
    Image_New(znccHalf.rcpsd, image.size, image.roi);
    Image_Mean(znccHalf.mean, znccHalf.lumdev, sizes.sumRect); // calculate mean
    Image_Subtract(znccHalf.lumdev, znccHalf.mean); // convert raw luminances to deviations from means.
    uniform Image satForScattering = IMAGE_INIT;
    Image_Clone(satForScattering, znccHalf.lumdev);
    Image_Multiply(satForScattering, znccHalf.lumdev);
    Image_Integrate(satForScattering);
    uniform const Rect satRoi = CreateRoi(image.size, SumKernel_MarginRect(sizes.sumRect));
    satForScattering.roi = satRoi;
    uniform SumKernel sk = Image_GetSumKernel(satForScattering, sizes.sumRect);
    uniform const int stride = satForScattering.size[0];
    uniform const float rcpArea1 = 1.0f / (float)(Area(sizes.sumRect) - 1);
    for (uniform int iRow = 0; iRow != satRoi[3]; iRow++)
    {
        foreach (iCol = 0 ... satRoi[2])
        {
            const int linearIndex = iCol + satRoi[0] + (iRow + satRoi[1]) * stride;
            znccHalf.rcpsd.elements[linearIndex] = rsqrt_fast(rcpArea1 * SumKernel_SATSum(sk, iCol));
        }
        sk.base += stride;
    }
    Image_Delete(satForScattering);
    Image_Clone(znccHalf.random, si.AllRand);
}

export void ZnccHalf_Delete(uniform ZnccHalf& znccHalf)
{
    Image_Delete(znccHalf.lumdev);
    Image_Delete(znccHalf.mean);
    Image_Delete(znccHalf.rcpsd);
}

export void ZnccMatchingSizes_Update(uniform ZnccMatchingSizes& sizes)
{
    uniform Rect sumKernelMargin = SumKernel_MarginRect(sizes.sumRect);
    sizes.marginRect = sumKernelMargin + sizes.searchRect;
    {
        const uniform Size2i imageBufferSize = 
            { sizes.packedSize[0] + sizes.marginRect[2], sizes.packedSize[1] + sizes.marginRect[3] };
        sizes.imageBufferSize = imageBufferSize;
    }
    {
        const uniform Rect imageRoi = 
            { -sizes.marginRect[0], -sizes.marginRect[1], sizes.packedSize[0], sizes.packedSize[1] };
        sizes.imageRoi = imageRoi;
    }
}
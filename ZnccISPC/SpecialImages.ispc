#include "SpecialImages.isph"

void SpecialImages_CreateRandSource(uniform float* uniform array, uniform const int seed, uniform const int size)
{
    uniform RNGState r;
    seed_rng(&r, seed);
    for (uniform int i = 0; i != size; i++)
    {
        array[i] = (uniform float)(random(&r) & 0xff);
    }
}

export void SpecialImages_Create(uniform SpecialImages& si, uniform const Size2i& size, uniform const Rect& roi)
{
    Image_New(si.ZeroBgOneFg, size, roi);
    Image_New(si.OneBgZeroFg, size, roi);
    Image_New(si.RandBgZeroFg, size, roi);
    Image_New(si.FltMaxBgZeroFg, size, roi);
    Image_New(si.AllRand, size, roi);
    uniform const int area = Area(size);
    uniform float* uniform randArray = si.AllRand.elements;
    uniform const int seed = (uniform const int)&si;
    SpecialImages_CreateRandSource(randArray, seed, area);

    foreach (iRow = 0 ... size[1], iCol = 0 ... size[0])
    {
        const Point2i point = { iCol, iRow };
        const bool inBg = IsNotIn(point, roi); // true: background, false: foreground
        const int linearIndex = iCol + iRow * size[0];
        si.ZeroBgOneFg.elements[linearIndex] = select(inBg, 0.0f, 1.0f);
        si.OneBgZeroFg.elements[linearIndex] = select(inBg, 1.0f, 0.0f);
        si.RandBgZeroFg.elements[linearIndex] = select(inBg, randArray[linearIndex], 0.0f);
        si.FltMaxBgZeroFg.elements[linearIndex] = select(inBg, __FLT_MAX__, 0.0f);
    }
}

export void SpecialImages_Delete(uniform SpecialImages& si)
{
    Image_Delete(si.ZeroBgOneFg);
    Image_Delete(si.OneBgZeroFg);
    Image_Delete(si.RandBgZeroFg);
    Image_Delete(si.FltMaxBgZeroFg);
    Image_Delete(si.AllRand);
}
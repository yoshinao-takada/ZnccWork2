#include <ZnccC/Basic2DTypesC.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>

int BasicsCUT()
{
    int err = EXIT_SUCCESS;
    do {
        Size2iC_t size0 = { 640, 480 };
        RectC_t rect0 = { -15, -5, 31, 11 };
        Size2iC_t size1;
        RectC_t rect1;
        COPY2(size1, size0);
        COPY4(rect1, rect0);
        if ((size1[0] != 640) || (size1[1] != 480))
        {
            fprintf(stderr, "%s,%d,fail in COPY2()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        if ((rect1[0] != -15) || (rect1[1] != -5) || (rect1[2] != 31) || (rect1[3] != 11))
        {
            fprintf(stderr, "%s,%d,fail in COPY4()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        int32_t area_size0 = AREA_SIZE(size0);
        if (area_size0 != (640*480))
        {
            fprintf(stderr, "%s,%d,fail in AREA_SIZE()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        int32_t area_rect0 = AREA_RECT(rect0);
        if (area_rect0 != (31 * 11))
        {
            fprintf(stderr, "%s,%d,fail in AREA_RECT()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        if (ARE_NE_SIZE(size0, size1) || ARE_NE_RECT(rect0, rect1))
        {
            fprintf(stderr, "%s,%d,fail in ARE_NE_SIZE() or ARE_NE_RECT()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        size0[0] += 1;
        rect0[2] -= 1;
        if (ARE_EQ_SIZE(size0, size1))
        {
            fprintf(stderr, "%s,%d,fail in ARE_EQ_SIZE()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        if (ARE_EQ_RECT(rect0, rect1))
        {
            fprintf(stderr, "%s,%d,fail in ARE_EQ_RECT()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        Point2iC_t point0 = { -15, -5 }, point1 = { 15, 5 };
        if ((true != IS_IN_RECT(point0, rect1)) || (true != IS_IN_RECT(point1, rect1)))
        {
            fprintf(stderr, "%s,%d,fail in IS_IN_RECT()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
        point0[0] --;
        point1[1] ++;
        if ((true != IS_NIN_RECT(point0, rect1)) || (true != IS_NIN_RECT(point1, rect1)))
        {
            fprintf(stderr, "%s,%d,fail in IS_NIN_RECT()\n", __FUNCTION__, __LINE__);
            err = EXIT_FAILURE;
            break;
        }
    } while (0);
    fprintf(stderr, "%s,%d,err = %d\n", __FUNCTION__, __LINE__, err);
    return err;
}
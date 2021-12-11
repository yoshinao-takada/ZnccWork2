#ifndef BASIC2DTYPESC_H_
#define BASIC2DTYPESC_H_

#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#include <cassert>
extern "C" {
#else
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#endif

#define NULLRECT_C { 0, 0, 0, 0 }
#define NULLSIZE_C { 0, 0 }
#define NULLPOINT_C NULLSIZE_C

typedef int32_t Size2iC_t[2];
typedef int32_t Point2iC_t[2];
typedef int32_t Range1D_[2];
typedef int32_t Rational[2]; // numerator[0], denominator[1]
typedef int32_t RectC_t[4];
#define COPY2(dst, src) { dst[0] = src[0]; dst[1] = src[1]; }
#define COPY4(dst, src) { dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = src[3]; }

#define AREA_SIZE(s) (s[0] * s[1])

#define AREA_RECT(r) (r[2] * r[3])

#define ZERO_SIZE(s) { s[0] = s[1] = 0; }

#define ZERO_RECT(r) { r[0] = r[1] = r[2] = r[3] = 0; }

#define ARE_EQ_SIZE(s0, s1) ((s0[0] == s1[0]) && (s0[1] == s1[1]))

#define ARE_EQ_RECT(r0, r1) ((r0[0] == r1[0]) && (r0[1] == r1[1]) && (r0[2] == r1[2]) && (r0[3] == r1[3]))

#define ARE_NE_SIZE(s0, s1) ((s0[0] != s1[0]) || (s0[1] != s1[1]))

#define ARE_NE_RECT(r0, r1) ((r0[0] != r1[0]) || (r0[1] != r1[1]) || (r0[2] != r1[2]) || (r0[3] != r1[3]))

#define IS_IN_RECT(pnt, r) ( \
    (r[0] <= (pnt)[0]) && \
    (r[1] <= (pnt)[1]) && \
    ((pnt)[0] < (r[0] + r[2])) &&\
    ((pnt)[1] < (r[1] + r[3])))

#define IS_NIN_RECT(pnt, r) ( \
    ((pnt)[0] < r[0]) || \
    ((pnt)[1] < r[1]) || \
    ((r[0] + r[2]) <= (pnt)[0]) ||\
    ((r[1] + r[3]) <= (pnt)[1]))

#define ROI(wholeSize, marginRect) { \
    -marginRect[0], -marginRect[1], wholeSize[0] - marginRect[2], wholeSize[1] - marginRect[3] }

#define F2U8(f) (uint8_t)min(255, max(0, (intn32_t)f))

#define FLIP_RECT(r) { 1 - (r[0]+r[2]), 1 - (r[1]+r[3]), r[2], r[3] }

#ifdef __cplusplus
}
#endif
#endif /* BASIC2DTYPESC_H_ */
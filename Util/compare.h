#ifndef COMPARE_H_
#define COMPARE_H_
#ifdef __cplusplus
#include <cmath>
extern "C" {
#else
#include <math.h>
#endif
inline bool AreEqualD(double d0, double d1, double reltol)
{
    double sumAbs = fabs(d0) + fabs(d1);
    if (sumAbs < reltol)
    {
        return AreEqualD(d0 + reltol, d1 + reltol, reltol);
    }
    double absDiff = fabs(d0 - d1);
    return (absDiff/sumAbs) < reltol;
}

inline bool AreNotEqualD(double d0, double d1, double reltol)
{
    return !AreEqualD(d0,d1,reltol);
}

inline bool AreEqualF(float d0, float d1, float reltol)
{
    float sumAbs = fabsf(d0) + fabsf(d1);
    if (sumAbs < reltol)
    {
        return AreEqualF(d0 + reltol, d1 + reltol, reltol);
    }
    float absDiff = fabsf(d0 - d1);
    return (absDiff/sumAbs) < reltol;
}

inline bool AreNotEqualF(float d0, float d1, float reltol)
{
    return !AreEqualF(d0,d1,reltol);
}

inline bool AreEqualArrayD(const double* d0, const double* d1, size_t size, double reltol)
{
    bool result = true;
    for (size_t i = 0; i != size; i++)
    {
        result = result & AreEqualD(d0[i], d1[i], reltol);
    }
    return result;
}

inline bool AreEqualArrayF(const float* d0, const float* d1, size_t size, float reltol)
{
    bool result = true;
    for (size_t i = 0; i != size; i++)
    {
        result = result & AreEqualF(d0[i], d1[i], reltol);
    }
    return result;
}

inline bool AreNotEqualArrayD(const double* d0, const double* d1, size_t size, double reltol)
{
    return !AreEqualArrayD(d0, d1, size, reltol);
}

inline bool AreNotEqualArrayF(const float* d0, const float* d1, size_t size, float reltol)
{
    return !AreEqualArrayF(d0, d1, size, reltol);
}
#ifdef __cplusplus
}
#endif
#endif /* COMPARE_H_ */
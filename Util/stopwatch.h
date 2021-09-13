#ifndef STOPWATCH_H_
#define STOPWATCH_H_
#ifdef __cplusplus
#include <ctime>
extern "C" {
#else
#include <time.h>
#endif
/*!
\brief clear global record buffer
*/
void Stopwatch_Clear();

/*!
\brief add a time measurement recrod
*/
void Stopwatch_Record(const char* caption, int optNum, struct timespec refTime);

/*!
\brief save the records in a csv file
*/
void Stopwatch_SaveCSV(const char* dir, const char* filename);

#ifdef __cplusplus
}
#endif
#endif /* STOPWATCH_H_ */

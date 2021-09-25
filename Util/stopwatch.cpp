#include "Util/stopwatch.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#define MAX_TIMERECORDS 256

typedef struct {
    char caption[16];
    int optNum;
    struct timespec elapsed;
} StopwatchRecord_t, *pStopwatchRecord_t;
typedef const StopwatchRecord_t *pcStopwatchRecord_t;
#define MAX_CAPTION_LEN 15

static int StopwatchRecord_filled = 0;
static StopwatchRecord_t timeRecords[MAX_TIMERECORDS];

void Stopwatch_Clear() { StopwatchRecord_filled = 0; }

void Stopwatch_Record(const char* caption, int optNum, struct timespec refTime)
{
    struct timespec time_now;
    timespec_get(&time_now, TIME_UTC);
    StopwatchRecord_t& record = timeRecords[StopwatchRecord_filled];
    if (strlen(caption) > MAX_CAPTION_LEN)
    {
        strncpy(record.caption, caption, MAX_CAPTION_LEN);
    }
    else
    {
        strcpy(record.caption, caption);
    }
    record.optNum = optNum;
    record.elapsed.tv_sec = time_now.tv_sec - refTime.tv_sec;
    if (time_now.tv_nsec >= refTime.tv_nsec)
    {
        record.elapsed.tv_nsec = time_now.tv_nsec - refTime.tv_nsec;
    }
    else
    {
        record.elapsed.tv_sec--;
        record.elapsed.tv_nsec = 1000000000 - refTime.tv_nsec + time_now.tv_nsec;
    }
    if (++StopwatchRecord_filled == MAX_TIMERECORDS)
    {
        fprintf(stderr, "%s,%d,Buffer full\n", __FUNCTION__, __LINE__);
        Stopwatch_SaveCSV(".", "stopwatch.csv");
        abort();
    }
}

void Stopwatch_SaveCSV(const char* dir, const char* filename)
{
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    std::filesystem::path filepath = std::filesystem::path(dir) / std::filesystem::path(filename);
    const char* cfilepath = filepath.c_str();
    FILE* pf = fopen(cfilepath, "w");
    Stopwatch_Save(pf);
    if (pf) fclose(pf);
}

void Stopwatch_Save(FILE* pf)
{
    fprintf(pf, "caption,opt No.,elapsed time (microseconds)\n");
    for (int i = 0; i < StopwatchRecord_filled; i++)
    {
        int elapsedMicroseconds = 1000000 * timeRecords[i].elapsed.tv_sec + timeRecords[i].elapsed.tv_nsec / 1000;
        fprintf(pf, "%s,%d,%d [microseconds]\n",
            timeRecords[i].caption, timeRecords[i].optNum, elapsedMicroseconds);
    }
}
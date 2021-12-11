#include <cstdlib>
#include <cstdio>
int PerfTestCWithoutCostMap();
int PerfTestC();
int PerfTestISPC();
int PerfTestCGrid();

int main()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = PerfTestCGrid()))
        {
            fprintf(stderr, "%s,%d,fail in PerfTestCGrid()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = PerfTestCWithoutCostMap()))
        {
            fprintf(stderr, "%s,%d,fail in PerfTestCWithoutCostMap()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = PerfTestC()))
        {
            fprintf(stderr, "%s,%d,fail in PerfTestC()\n", __FUNCTION__, __LINE__);
            break;
        }
        if (EXIT_SUCCESS != (err = PerfTestISPC()))
        {
            fprintf(stderr, "%s,%d,fail in PerfTestISPC()\n", __FUNCTION__, __LINE__);
            break;
        }
    } while (0);
    return err;
}
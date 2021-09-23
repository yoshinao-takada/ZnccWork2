#include <cstdlib>
#include <cstdio>
int PerfTestC();
int PerfTestISPC();

int main()
{
    int err = EXIT_SUCCESS;
    do {
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
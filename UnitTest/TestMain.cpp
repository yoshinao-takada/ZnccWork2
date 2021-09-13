#include <cstdlib>
#include <cstdio>
int BasicsUT();
int UtilImageSourceUT();
int MeanScatteringUT();
int CostMapUT();

int main()
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = BasicsUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
        if (EXIT_SUCCESS != (err = UtilImageSourceUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
        if (EXIT_SUCCESS != (err = MeanScatteringUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
        if (EXIT_SUCCESS != (err = CostMapUT()))
        {
            fprintf(stderr, "%s,%d,fail,err=%d\n", __FUNCTION__, __LINE__, err);
        }
    } while (0);
    fprintf(stderr, "%s,%d,err=%d\n", __FUNCTION__, __LINE__, err);
    return err;
}
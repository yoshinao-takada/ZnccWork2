#include "App0/AppOpt.h"
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define APPOPT_KW_SEARCHRECT    "SEARCHRECT"
#define APPOPT_KW_SUMRECT       "SUMRECT"
#define APPOPT_KW_SCALE         "SCALE"

void ToUpperString(char* str)
{
    while (*str)
    {
        *str = (char)toupper((int)(*str));
        str++;
    }
}

int AppOpt_Read(pAppOpt_t opt, FILE* pf)
{
    int err = EXIT_SUCCESS;
    char buf[128];
    char key[64];
    do {
        {
            const Rational DefaultScale = DEFAULT_SCALE;
            const RectC_t DefaultSearchRect = DEFAULT_SEARCHRECT;
            const RectC_t DefaultSumRect = DEFAULT_SUMRECT;
            COPY2(opt->scale, DefaultScale);
            COPY4(opt->searchRect, DefaultSearchRect);
            COPY4(opt->sumRect, DefaultSumRect);
        }
        while (buf == fgets(buf, sizeof(buf), pf))
        {
            sscanf(buf, "%s", key);
            ToUpperString(key);
            if (0 == strncmp(key, APPOPT_KW_SEARCHRECT, strlen(APPOPT_KW_SEARCHRECT)))
            {
                sscanf(buf + strlen(key), "%d %d %d %d", 
                    &opt->searchRect[0], &opt->searchRect[1], &opt->searchRect[2], &opt->searchRect[3]);
            }
            else if (0 == strncmp(key, APPOPT_KW_SCALE, strlen(APPOPT_KW_SCALE)))
            {
                sscanf(buf + strlen(key), "%d %d", &opt->scale[0], &opt->scale[1]);
            }
            else if (0 == strncmp(key, APPOPT_KW_SUMRECT, strlen(APPOPT_KW_SUMRECT)))
            {
                sscanf(buf + strlen(key), "%d %d %d %d", 
                    &opt->sumRect[0], &opt->sumRect[1], &opt->sumRect[2], &opt->sumRect[3]);
            }
            else
            {
                fprintf(stderr, "Invalid keyword [%s] was detected.\n", key);
                err = EINVAL;
                break;
            }
        }
    } while (0);
    return err;
}

int AppOpt_ReadFile(pAppOpt_t opt, const char* filepath)
{
    int err = EXIT_SUCCESS;
    FILE* pf = NULL;
    do {
        if (NULL == (pf = fopen(filepath, "r")))
        {
            err = errno;
            break;
        }
        err = AppOpt_Read(opt, pf);
    } while (0);
    if (pf)
    {
        fclose(pf);
    }
    return err;
}

void AppOpt_Write(pAppOpt_t opt, FILE* pf)
{
    fprintf(pf, "%s %d %d %d %d\n", APPOPT_KW_SEARCHRECT,
        opt->searchRect[0], opt->searchRect[1], opt->searchRect[2], opt->searchRect[3]);
    fprintf(pf, "%s %d %d %d %d\n", APPOPT_KW_SUMRECT,
        opt->sumRect[0], opt->sumRect[1], opt->sumRect[2], opt->sumRect[3]);
    fprintf(pf, "%s %d %d\n", APPOPT_KW_SCALE, opt->scale[0], opt->scale[1]);
}

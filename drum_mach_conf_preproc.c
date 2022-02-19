#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

#define LOG_ID "PREPROC"

#define NUM_CONFIG_VARS 128

char config_variables[NUM_CONFIG_VARS][2][16];
int init = 0;

void preprocess_line(char *preproc_buf, int max_sz, char *buf, const char *sample_data_path)
{
    if (!init)
    {
        init = 1;
        memset(config_variables, 0, sizeof(config_variables));
    }
    int preproc_buf_i = 0;
    preproc_buf[0] = 0;
    for (int i = 0; buf[i] && preproc_buf_i < max_sz - 1; i++)
    {
        if (buf[i] == '{')
        {
            int s = i + 1;
            for (; buf[i] && buf[i] != '}'; i++) {}
            buf[i] = 0;
            int found = 0;
            for (int j = 0; j < NUM_CONFIG_VARS; j++)
            {
                if (!strcmp(config_variables[j][0], &buf[s]))
                {
                    // Todo check length also here
                    strcat(preproc_buf, config_variables[j][1]);
                    found = 1;
                    break;
                }
            }
            if (!found)
            {
                log_warn("Variable '%s' not found!\n", &buf[s]);
            }
            buf[i] = '}';
            preproc_buf_i = strlen(preproc_buf);
            continue;
        }
        preproc_buf[preproc_buf_i++] = buf[i];
    }
    preproc_buf[preproc_buf_i] = 0;

    char cmd = preproc_buf[0];
    char *cmd_params = preproc_buf + 1;
    if (cmd == ':')
    {
        preproc_buf[0] = 0;
        char varname[256];
        char varval[256];
        sscanf(cmd_params, "%s %s", varname, varval);
        varname[15] = 0;
        varval[15] = 0;
        for (int i = 0; i < NUM_CONFIG_VARS; i++)
        {
            if (!config_variables[i][0][0])
            {
                log_info("Set variable %s = %s\n", varname, varval);
                strcpy(config_variables[i][0], varname);
                strcpy(config_variables[i][1], varval);
                break;
            }
        }
    }
    else if (cmd == 'I') // include
    {
        char file_name[256];
        int current_dir = 1;
        sscanf(cmd_params, "%s %d", file_name, &current_dir);
        FILE *f;
        if (!current_dir)
            f = fopen(file_name, "r");
        else
        {
            char full_path[1024];
            sprintf(full_path, "%s%s", sample_data_path, file_name);
            f = fopen(full_path, "r");
        }
        if (f)
        {
            cmd = 0;
            while (!feof(f) && cmd != 'E')
            {
                char readbuf[256];
                fgets(readbuf, 256, f);
                preprocess_line(preproc_buf, max_sz, readbuf, sample_data_path);
            }
            fclose(f);
        }
        preproc_buf[0] = 0;
    }
}

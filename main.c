/**<FVR configuration file generator
    John.Wang @ 20160909
*/
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "fvr_gen.h"

char g_buff[10240] = {0};

/**/
static int fvr_gen_pre_check(char* path)
{
    FILE *fp = NULL;
    int  file_size = 0;
    printf("\nfvr_gen_pre_check:%s\n", path);

    if (strlen(path) > FVR_CK_MAX_PATH_LEN)
    {
        printf("fvr_gen_pre_check ret=%d\n", FVR_CK_PATH_ERR);
        return FVR_CK_PATH_ERR;
    }

    if ((fp = fopen(path, "rb")) == NULL)
    {
        printf("fvr_gen_pre_check ret=%d\n", FVR_CK_FILE_OPEN_ERR);
        return FVR_CK_FILE_OPEN_ERR;
    }

    /* check file size, configuration file must smaller than 10k */
    fseek(fp, 0L, SEEK_END);
    if ((file_size = ftell(fp)) > FVR_CK_MAX_FILE_SIZE)
    {
        fclose(fp);
        printf("1-fvr_gen_pre_check file_size=%d\n", file_size);
        return FVR_CK_FILE_OVER_LEN;
    }
    else
    {
        printf("2-fvr_gen_pre_check file_size=%d\n", file_size);
        fclose(fp);
    }

    return FVR_CK_SUCCESS;
}

static void fvr_gen_output_err_info(int idx)
{
    printf("\nfvr_gen_output_err_info idx=%d\n", idx);
}

static void fvr_gen_parse_cmd_line(char *cmd_line, int cmd_len)
{
    printf("parse_cmd_line:%s, cmd_len:%d\n", cmd_line, cmd_len);
}

static int fvr_gen_pick_out_cmd_line(char *read_buffer, int *write_pos)
{
    char cmd_line[FVR_GEN_MAX_CMD_LEN + 1];
    int  step = 0;
    int  tail_exist = 0;
    int  cmd_len = 0;
    char *end_ptr = NULL;
    int  err_code = FVR_CK_SUCCESS;
    char *read_ptr = read_buffer;
    int i;

    memset(cmd_line, 0, sizeof(cmd_line));

    //printf("pick_out_cmd_line read_buffer=\n%s\n", read_ptr);
    #if 0
    for(i = 0; i < FVR_GEN_MAX_BUFF_LEN; i++)
    {
        if (*(read_ptr + i) == 0x0d || *(read_ptr + i) == 0x0a)
        {
            printf("=====> %d , 0x%x\n", i, *(read_ptr + i));
            break;
        }
    }
    #else
    do
    {
        end_ptr = strstr(read_ptr, "\x0d\x0a");
        if (NULL == end_ptr)
        {
            printf("pick_out_cmd_line -1 \n");
            end_ptr = strstr(read_ptr, "\x0d\x0d\x0a");
            if (NULL == end_ptr)
            {
                printf("pick_out_cmd_line -2 \n");
                break;
            }
            else
            {
                step = 3;
                tail_exist = 1;
            }
        }
        else
        {
            step = 2;
            tail_exist = 1;
        }

        if (tail_exist)
        {
            cmd_len = end_ptr - read_ptr + step;

            if (cmd_len > FVR_GEN_MAX_CMD_LEN)
            {
                err_code = FVR_CK_CMD_OVER_LEN;
                break;
            }
            memcpy(cmd_line, read_ptr, cmd_len - step);
            fvr_gen_parse_cmd_line(cmd_line, cmd_len);

            read_ptr += cmd_len;
            memset(cmd_line, 0, sizeof(cmd_line));
            cmd_len = 0;
        }
    }
    while(1);

    if (err_code == FVR_CK_SUCCESS)
    {
        *write_pos = FVR_GEN_MAX_BUFF_LEN - (int)(read_ptr - read_buffer);
        memmove(read_buffer, read_ptr, *write_pos);
        memset(&read_buffer[*write_pos], 0, (int)(read_ptr - read_buffer));
        printf("4- read_buffer *write_pos=%d\n", *write_pos);
    }
    #endif // 1

    printf("4- read_buffer err_code=%d\n", err_code);

    return err_code;
}

int fvr_gen_parse_conf_file(char* path)
{
    FILE *fp = NULL;
    char read_buffer[FVR_GEN_MAX_BUFF_LEN + 1];
    int  nmemb = 0;
    int  write_pos = 0;
    int  err_code = FVR_CK_SUCCESS;

    memset(read_buffer, 0, sizeof(read_buffer));

    printf("fvr_gen_parse_conf_file path=%s\n", path);

    if ((fp = fopen(path, "rb")) == NULL)
    {
        return FVR_CK_FILE_OPEN_ERR;
    }
    fseek(fp, 0L, SEEK_SET);

    do
    {
        nmemb = fread(&read_buffer[write_pos], (FVR_GEN_MAX_BUFF_LEN - write_pos), 1, fp);
        if (1 == nmemb)
        {
            err_code = fvr_gen_pick_out_cmd_line(read_buffer, &write_pos);
            if (err_code == FVR_CK_SUCCESS)
                continue;
            else
                break;
        }
        else
        {
            printf("fvr_gen_parse_conf_file last package\n");
            //printf("%s\n", read_buffer);
            err_code = fvr_gen_pick_out_cmd_line(read_buffer, &write_pos);
            if (err_code == FVR_CK_SUCCESS)
                fvr_gen_parse_cmd_line(read_buffer, write_pos);
            break;
        }
    }
    while(1);

    if (feof(fp))
    {
        err_code = FVR_CK_SUCCESS;
    }
    else
    {
        err_code = FVR_CK_FILE_END_ERR;
    }

    fclose(fp);

    printf("fvr_gen_parse_conf_file ret=%d\n", err_code);
    return err_code;
}

int main(int argc, char* argv[])
{
    int i;
    int err_code = 0;

    printf("argc:%d\nargv:", argc);
    for (i = 0; i < argc; i++)
        printf("%s ", argv[i]);

    err_code = fvr_gen_pre_check(argv[1]);

    if (FVR_CK_SUCCESS == err_code)
    {
        err_code = fvr_gen_parse_conf_file(argv[1]);
    }

    fvr_gen_output_err_info(err_code);

    return 0;
}

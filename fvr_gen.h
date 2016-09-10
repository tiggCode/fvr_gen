#ifndef __FVR_GEN_H__
#define __FVR_GEN_H__

#define FVR_CK_MAX_PATH_LEN  60
#define FVR_CK_MAX_FILE_SIZE 10240   /* 10K bytes */

#define FVR_GEN_STR_PASSWORD    "gl300vc"
#define FVR_GEN_STR_PRE_HEAD    "AT+GT"
#define FVR_GEN_STR_CMD_TYPE    "FVR"
#define FVR_GEN_STR_SERIAL_NUM  "1111"
#define FVR_GEN_TAIL_CHAR       '$'

#define FVR_GEN_MAX_CMD_LEN     600//355
#define FVR_GEN_MAX_BUFF_LEN    1024

typedef enum
{
    FVR_CK_SUCCESS = 0,
    FVR_CK_PATH_ERR,
    FVR_CK_FILE_OPEN_ERR,
    FVR_CK_FILE_OVER_LEN,
    FVR_CK_FILE_END_ERR,
    FVR_CK_FILE_READ_ERR,
    FVR_CK_CMD_OVER_LEN,
    FVR_CK_NUM
} fvr_gen_error_code_enum;

#endif /* __FVR_GEN_H__ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"

#define HEAD_OUT "./head.out"
#define BODY_OUT "./body.out"
#define RET_EQUAL 0
#define RET_NOT_EQUAL 1
#define RET_NO_UPGRADE_FILE 2
#define RET_FIND_UPGRADE_FILE 3
#define RET_CHECK_OK 4
#define RET_ERROR 5
#define RET_SD_VERSION_ERROR 6
#define RET_OK 7
#define RET_CUR_VERSION_ERROR 8
#define RET_XOR_ERROR 9
#define RET_UPGRADE_OK 10
#define RET_UPGRADE_ERROR 11
#define RET_ERROR_FILE_NAME 12
#define RET_DELAY_CMD_NOT_RUN 13
#define RET_DEALY_CMD_ERROR 14
#define RET_GET_HASH_OK 15
#define RET_GET_HASH_ERROR 16
#define RET_AREADY_CP_VERSION 17
#define RET_NOT_CP_VERSION 18
#define RET_XOR_VERSION 19
#define RET_OPEN_VERSION_LIST_PATH_FAIL 20
#define OPEN_HEAD_FILE_ERROR 21
#define OPEN_BODY_FILE_ERROR 22
#define RET_GET_VERSION_PATH 23
#define RET_NOT_GET_VERSION_PATH 24
#define TIME_OUT 25
#define DOWNLOAD_OK 26
#define LOG fprintf
#define LOG_NOTICE stderr

size_t write_data(void * ptr, size_t size, size_t nmemb, void* stream)
{
    int written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int detect_version(char * url)
{
 
    CURL * curl_handle;
 
    FILE * headerfile;
 
    FILE * bodyfile;
 
    static const char * headerfilename = HEAD_OUT;
 
    static const char * bodyfilename = BODY_OUT;
 
    int res = 0;
 
    curl_global_init(CURL_GLOBAL_ALL);
 
    curl_handle = curl_easy_init();
 
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
 
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
 
    headerfile = fopen(headerfilename, "wb");
 
    if(headerfilename == NULL) 
    {
        curl_easy_cleanup(curl_handle);
 
        return OPEN_HEAD_FILE_ERROR;
    }
 
    bodyfile = fopen(bodyfilename, "wb");
 
    if(bodyfile == NULL) 
    {
        curl_easy_cleanup(curl_handle);
 
        return OPEN_BODY_FILE_ERROR;
    }
 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, headerfile);
 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, bodyfile);
 
    res = curl_easy_perform(curl_handle);
 
    if(res != CURLE_OK)
 
    LOG(LOG_NOTICE, "curl_easy_perform() faild:%s\n",
 
    curl_easy_strerror(res));
 
    fclose(headerfile);
 
    fclose(bodyfile);
 
    curl_easy_cleanup(curl_handle);
 
    return res;
 
}

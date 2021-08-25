/*************************************************
Copyright:	KNY2021
Author: 	KNY2021
Date:2021-08-20
Description:获取基金信息发送到钉钉qun里
**************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "detect_version.h"
#include <time.h>
#include <unistd.h>
#include "cJSON.h"

#define url_path "./body.out"

static int fund_code[5] =
{
	4433,
	161725,
	160516，
	3096,
	260108
};

typedef struct {
	int fundcode;	//基金代码
	char name[64];	//基金名称
	char jzrq[64];	//净值日期
	double dwjz;	//当日净值
	double gsz;		//估算净值
	double gszzl;	//估算涨跌百分比
	char gztime[64];//估值时间
}fund;
#define FUND_LEN sizeof(fund)

/*************************************************
Function:       parse_str
Description:    cJSON解析基金信息字段
Input:          str 	: 	 解析字段
				fund_str:	 存放解析结果
Output:         NULL
Return:         NULL
*************************************************/
static void parse_str(char* str, fund* fund_str)
{
	cJSON* str_json = cJSON_Parse(str);

	cJSON* str_fundcode = cJSON_GetObjectItem(str_json, "fundcode");
	fund_str->fundcode = atoi(str_fundcode->valuestring);
	cJSON* str_name = cJSON_GetObjectItem(str_json, "name");
	strcpy(fund_str->name, str_name->valuestring);
	cJSON* str_jzrq = cJSON_GetObjectItem(str_json, "jzrq");
	strcpy(fund_str->jzrq, str_jzrq->valuestring);
	cJSON* str_dwjz = cJSON_GetObjectItem(str_json, "dwjz");
	fund_str->dwjz = atof(str_dwjz->valuestring);
	cJSON* str_gsz = cJSON_GetObjectItem(str_json, "gsz");
	fund_str->gsz = atof(str_gsz->valuestring);
	cJSON* str_gszzl = cJSON_GetObjectItem(str_json, "gszzl");
	fund_str->gszzl = atof(str_gszzl->valuestring);
	cJSON* str_gztime = cJSON_GetObjectItem(str_json, "gztime");
	strcpy(fund_str->gztime, str_gztime->valuestring);

	cJSON_Delete(str_json);
}

/*************************************************
Function:       send_dingding_qun
Description:    向钉钉qun发送基金信息
Input:          msg 	: 	 多支基金的涨跌信息
Output:         NULL
Return:         发送执行结果
*************************************************/
static int send_dingding_qun(char* msg)
{
	char cmd[5120] = { 0 };

	//sprintf钉钉的curl接口，修改成你的机器人的webhook的tooken
	sprintf(cmd, "curl 'https://oapi.dingtalk.com/robot/send?access_token=a31cd21bfa971dd4e05b2b1e818bd497e68edc12fd1fc5a00243f4be139c8acf' \
	  -H 'Content-Type: application/json' \
   	-d '{\"msgtype\": \"text\",\"text\": {\"content\": \"{%s\n}\"}}'", msg);
	printf("%s\n", cmd);
	//执行curl
	system(cmd);

	return 0;
}

/*************************************************
Function:       send_cmd
Description:    整合多支基金涨跌信息
Input:          scanf_num	: 输入的基金代码
				msg			: 获取要上传的字符串
Output:         基金信息字符串
Return:         执行结果
*************************************************/
static int send_cmd(int scanf_num, char* msg)
{
	char buff[2048] = { 0 };
	char str_fund[1024] = { 0 };

	char url_char[128] = { 0 };

	//整合获取基金信息的url
	sprintf(url_char, "http://fundgz.1234567.com.cn/js/%06d.js", scanf_num);
	//curl整合的url，保存接收的字符串到 body.out
	detect_version(url_char);

	fund* fun = malloc(FUND_LEN);

	FILE* fp = fopen(url_path, "a+");

	fgets(buff, 2048, fp);
	//预处理字符串
	sscanf(buff, "jsonpgz(%s", str_fund);
	int len = strlen(str_fund);
	sprintf(str_fund + len, "\"}");

	printf("%s\n", str_fund);
	//cJSON解析字符串
	parse_str(str_fund, fun);

	memset(buff, 0x00, 2048);
	//整合发送到qun里的输出信息
	sprintf(buff, "\n基金名称:%s\n估算涨跌百分比:%f", fun->name, fun->gszzl);
	strcat(msg, buff);

	fclose(fp);
	free(fun);
	return 0;
}

int main()
{
	char msg[2048] = { 0 };
	time_t now_time;

	struct tm* now_tm;

	while (1)
	{

		time(&now_time);

		now_tm = gmtime(&now_time);
		now_tm->tm_hour += 8;
		now_tm->tm_year += 1900;
		now_tm->tm_mon += 1;

		if (now_tm->tm_wday < 5 && now_tm->tm_hour >= 9 && now_tm->tm_hour <= 15)
		{
			if (now_tm->tm_min == 0 || now_tm->tm_min == 30)
			{
				for (int i = 0;i < 5;i++)
				{
					send_cmd(fund_code[i], msg);
				}
				send_dingding_qun(msg);
				memset(msg,0x00,2048);
			}
		}
		sleep(60);
	}

	return 0;
}

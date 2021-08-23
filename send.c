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
#include <sys/time.h>
#include "cJSON.h"

#define url_path "./body.out"

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

	cJSON* str_fundcode 	= cJSON_GetObjectItem(str_json, "fundcode");
	fund_str->fundcode 		= atoi(str_fundcode->valuestring);
	cJSON* str_name 		= cJSON_GetObjectItem(str_json, "name");
	strcpy(fund_str->name,str_name->valuestring);
	cJSON* str_jzrq 		= cJSON_GetObjectItem(str_json, "jzrq");
	strcpy(fund_str->jzrq,str_jzrq->valuestring);
	cJSON* str_dwjz 		= cJSON_GetObjectItem(str_json, "dwjz");
	fund_str->dwjz 			= atof(str_dwjz->valuestring);
	cJSON* str_gsz 			= cJSON_GetObjectItem(str_json, "gsz");
	fund_str->gsz			= atof(str_gsz->valuestring);
	cJSON* str_gszzl 		= cJSON_GetObjectItem(str_json, "gszzl");
	fund_str->gszzl 		= atof(str_gszzl->valuestring);
	cJSON* str_gztime 		= cJSON_GetObjectItem(str_json, "gztime");
	strcpy(fund_str->gztime,str_gztime->valuestring);

	cJSON_Delete(str_json);
}

/************************************************* 
Function:       send_cmd 
Description:    向钉钉qun发送基金信息 
Input:          NULL
Output:         基金信息字符串 
Return:         基金信息结构体
*************************************************/  
static fund* send_cmd()
{
	char cmd[5120] = { 0 };
	char buff[1500] = { 0 };
	char str_fund[1024] = {0};
	
	int scanf_num = 0;	
	char url_char[64]={0};
	//输入基金代码
	scanf("%d",&scanf_num);
	//整合获取基金信息的url
	sprintf(url_char,"http://fundgz.1234567.com.cn/js/%06d.js",scanf_num);
	//curl整合的url，保存接收的字符串到 body.out
	detect_version(url_char);

	fund* fun = malloc(FUND_LEN);

	FILE* fp = fopen(url_path, "a+");

	fgets(buff,1024,fp);
	//预处理字符串
	sscanf(buff,"jsonpgz(%s",str_fund);
	int len = strlen(str_fund);
	sprintf(str_fund+len,"\"}");

	printf("%s\n",str_fund);
	//cJSON解析字符串
	parse_str(str_fund,fun);

	memset(buff,0x00,1500);
	//整合发送到qun里的输出信息
	sprintf(buff, "基金代码:%d\n基金名称:%s\n净值日期:%s\n当日净值:%lf\n估算净值:%lf\n估算涨跌百分比:%f\n估值时间:%s", fun->fundcode, fun->name, fun->jzrq, fun->dwjz, fun->gsz, fun->gszzl, fun->gztime);
	//sprintf钉钉的curl接口，修改成你的机器人的webhook的tooken，****为机器人的webhook的tooken
	sprintf(cmd, "curl 'https://oapi.dingtalk.com/robot/send?access_token=*************' \
	  -H 'Content-Type: application/json' \
   	-d '{\"msgtype\": \"text\",\"text\": {\"content\": \"{%s\n}\"}}'",buff);
	printf("%s\n",cmd);
	//执行curl
	system(cmd);
	fclose(fp);
	return fun;
}

int main()
{

	fund* p= NULL;

	p = send_cmd();

	free(p);
	
	return 0;
}

#include <stdio.h>
#include <time.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#include <Wincon.h>
#include <io.h>
#include <fstream>
#include "student.h"
using namespace std;
#pragma warning (disable:4996)

// air_ppm.cpp : Defines the entry point for the console application.
//
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGREY 7
#define DARKGREY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15
#define BLINK 128
int ppm_colors[] = { FOREGROUND_BLUE,
LIGHTGREY,
CYAN,
MAGENTA,
FOREGROUND_GREEN,
LIGHTGREEN,
LIGHTCYAN,
YELLOW,
FOREGROUND_RED ,
LIGHTRED
};

/*
直升机采样栅格数据
*/
#define MAX_GRID_NUM 60
typedef struct _GRID_DATA_
{
	int size;//栅格大小1-10m
	float longitude;//地理位置信息，经度
	float latitude;//地理位置信息，纬度
	int ppm;//检测到的ppm值

}GRID_DATA, * PGRID_DATA;

#define GRID_DATA_SIZE 	(sizeof(GRID_DATA) * MAX_GRID_NUM * MAX_GRID_NUM)
/*
获取下一个采样文件名
文件命名规则为：grid-number.dat，其中number为采样的次数，自动检测
*/
void get_file_name(char* file_name)
{
	int number = 1;
	while (true)
	{
		sprintf(file_name, "%s-%d.dat", "grid", number++);
		//如果文件不存在，则返回
		if (0 != access(file_name, 0))
			break;
	}
}
/*
一次直升机采样
data_size:栅格大小
为了便于在console里面显示，假定一次采用的数据是有限的，
即直升机30分钟采用的数据
假设控制台（console）屏幕大小为MAX_GRID_NUM*MAX_GRID_NUM
所以生成MAX_GRID_NUM*MAX_GRID_NUM个栅格数据
起始经纬度为一个随机值，然后在以这个经纬度作为直升机的开始位置
假设直升机半个小时能飞行的面积为MAX_GRID_NUM*MAX_GRID_NUM单位
假设两个相邻栅格的经纬度变化为1个单位（实际并不是这样）
只在中国采样，中国经度范围：73°33′E至135°05′E；纬度范围：3°51′N至53°33′N
PPM等级10、20…100
采样完毕，自动保存文件，文件命名规则为：grid-number.dat，其中number为采样的次数，自动检测
*/
int global_val = 0;
void capture_data(int data_size, PGRID_DATA current_data, bool save_to_file)
{
	char file_name[256] = { 0 };
	get_file_name(file_name);
	FILE* file = NULL;
	if (save_to_file)
	{
		bool btest = save_to_file;

		file = fopen(file_name, "w+b");
		if (!file)
		{
			printf("不能打开文件%s\n", file_name);
			return;
		}
	}
	//写入采样时间
	time_t now = time(NULL);
	if (save_to_file)
	{
		fwrite(&now, sizeof(time_t), 1, file);
	}
	srand(time(NULL));
	//生成起始经纬度
	int longitude = rand() % (135 - 73) + 73;
	int latitude = rand() % (53 - 3) + 3;
	//PPM等级10-100
	for (int i = 0; i < MAX_GRID_NUM; i++)
	{
		for (int j = 0; j < MAX_GRID_NUM; j++)
		{
			current_data[i * MAX_GRID_NUM + j].size = data_size;
			current_data[i * MAX_GRID_NUM + j].longitude = (float)longitude + 0.11 * 5 * i;
			current_data[i * MAX_GRID_NUM + j].latitude = (float)latitude + 0.11 * 5 * j;
			current_data[i * MAX_GRID_NUM + j].ppm = rand() % 90 + 10;
			if (save_to_file)
			{
				fwrite(&current_data[i * MAX_GRID_NUM + j], sizeof(GRID_DATA), 1, file);
			}
		}
	}
	if (save_to_file)
	{
		fclose(file);
		printf("无人机采样完成，自动保存到文件，文件名为:%s\n", file_name);
	}
}
void draw_ppm(int ppm)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ppm / 10);
	printf("█");
}

class test_c
{
public:
	test_c() {}
	virtual ~test_c(){}
	int test_fun(int i,int j)
	{
		return i * j;
	}

};
void fun_test(int i)
{
	printf("enter fun_test with int %d\n",i);
}
void fun_test(float j)
{
	printf("enter fun_test with float j %f\n",j);
}
void draw_grid_data(PGRID_DATA data, time_t time)
{
	CONSOLE_SCREEN_BUFFER_INFO old_info = { 0 };
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &old_info);
	int max_ppm = 0;
	float longtitude = 0;
	float latitude = 0;
	for (int i = 0; i < MAX_GRID_NUM; i++)
	{
		for (int j = 0; j < MAX_GRID_NUM; j++)
		{
			int index = i * MAX_GRID_NUM + j;
			draw_ppm(data[index].ppm);
			if (data[index].ppm > max_ppm)
			{
				max_ppm = data[index].ppm;
				longtitude = data[index].longitude;
				latitude = data[index].latitude;
			}
		}
		printf("\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), old_info.wAttributes);
	tm* local = localtime(&time);
	char local_time_str[100] = { 0 };
	sprintf(local_time_str, "%d-%d-%d %d:%d:%d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	printf("采样时间为：%s, 污染源经纬度为：%.2f E % .2f N\n", local_time_str, longtitude, latitude);
}
/*
显示采样栅格图
*/
void draw_real_data()
{
	PGRID_DATA data = (PGRID_DATA)malloc(GRID_DATA_SIZE);
	capture_data(20, data, false);
	draw_grid_data(data, time(NULL));
	free(data);
}
void print_ppm_menu()
{
	printf("********大气环境监测显示与管理系统\n");
	printf("********请选择对应的菜单，然后按回车进行操作\n");
	printf("********1：无人机数据采样\n");
	printf("********2：查看实时采样数据\n");
	printf("********3：查看过往采样数据\n");
	printf("********q：退出\n");
}
/*
从文件中读取数据并显示
*/
void view_old_data()
{
	char file_name[256] = { 0 };
	printf("请输入采样数据文件名（已经保存的文件在当前目录下面，\n文件名格式为grid-xx.dat格式，xx为数字），然后按回车");
	scanf("%s", file_name);
	if (0 != access(file_name, 0))
	{
		printf("采样文件不存在，请重新操作\n");
		return;
	}
	FILE* file = fopen(file_name, "rb");
	if (!file)
	{
		printf("不能打开文件，请重新操作\n");
		return;
	}
	time_t now;
	PGRID_DATA data = (PGRID_DATA)malloc(GRID_DATA_SIZE);
	fread(&now, sizeof(time_t), 1, file);
	fread(data, GRID_DATA_SIZE, 1, file);
	draw_grid_data(data, now);
	free(data);

	fclose(file);
}
void add_new_data()
{
	int size = 1;
	printf("请输入采样栅格大小（1-10），然后按回车\n");
	scanf("%d", &size);
	if (size < 1)
	{
		printf("输入的栅格小于1，自动设定为1\n");
		size = 1;
	}
	if (size > 10)
	{
		printf("输入的栅格大于10，自动设定为10\n");
		size = 10;
	}
	PGRID_DATA data = (PGRID_DATA)malloc(GRID_DATA_SIZE);
	capture_data(size, data, true);
	free(data);
}
void clear_screen_show_menu()
{
	system("pause");
	system("cls");
	print_ppm_menu();
}

void intrinsic_functions_test()
{
	//strings
	const char* str1 = "test string";
	const char* str2 = "test string2";
	const wchar_t* wstr1 = L"test string";
	const wchar_t* wstr2 = L"test string2";
	printf("%s %s %S %S\n", str1, str2, wstr2, wstr2);
	//API

}
void get_last_error_message()
{
	fstream file;
	file.open("test.dat", std::ios_base::in);
	if (!file)
	{
		LPVOID  msg;
		DWORD err = GetLastError();
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&msg,
			0, NULL);
		printf("错误码是%d,错误信息是%s\n", err, (char*)msg);
		LocalFree(msg);
	}
	file.close();
}
std::string get_xml()
{
	std::string xml = "<students>	 \
		< student name = \"John\" >		 \
		<scores>		   \
		< Chinese>90 < / Chinese >	  \
		< Math>100 < / Math >		\
		< English>100 < / English >	  \
		< / scores>				 \
		< / student>			 \
		<student name = \"Mike\">	 \
		<scores>				   \
		< Chinese>99 < / Chinese >	  \
		< Math>98 < / Math >		  \
		< English>100 < / English >	  \
		< / scores>				   \
		< / student>			   \
		< / students>";
	return xml;
}
std::string get_json()
{
	std::string json = "[			  \
	{								 \
		\"name\": \"John\",	  \
			\"scores\" : {			\
			\"Chinese\": \"90\",			  \
				\"Math\" : \"100\",		   \
				\"English\" : \"100\"			\
		}								\
	},								   \
					{					\
					\"name\": \"Mike\",		\
					\"scores\" : {				  \
							  \"Chinese\": \"99\",		\
							  \"Math\" : \"98\",		   \
							  \"English\" : \"100\"			  \
					}						\
					}					   \
]";
return json;
}
void string_test()
{
	std::string json = get_json();
	std::string xml = get_xml();
}
void condition_test()
{
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			int k = i * j;
			
		}
	}
}
void data_breakpoint_test()
{
	char str1[11];
	char str2[5];
	strcpy(str1, "breakpoint");
	printf("str1 is %s\n", str1);
	strcpy(str2, "this is data breakpoint test");
	printf("str1 is %s\nstr2 is %s\n", str1, str2);
}
int main()
{
	char teststr[10];
	data_breakpoint_test();
	test_c test;
	test_c test2;
	test.test_fun(10, 20);
	test2.test_fun(20, 20);
	fun_test(10);
	fun_test(10.10f);
	condition_test();
	string_test();
	//char* name = NULL;
	//assert(name != NULL);
	//student stu2(name, 90);
	
	get_last_error_message();
	intrinsic_functions_test();
	print_ppm_menu();
	while (true)
	{
		int c = getchar();
		switch (c)
		{
		case '1':
			add_new_data();
			clear_screen_show_menu();
			break;
		case '2':
			draw_real_data();
			clear_screen_show_menu();
			break;
		case '3':
			view_old_data();
			clear_screen_show_menu();

			break;
		case 'q':
			return 0;
		default:
			break;
		}
	}

	return 0;
}


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
ֱ��������դ������
*/
#define MAX_GRID_NUM 60
typedef struct _GRID_DATA_
{
	int size;//դ���С1-10m
	float longitude;//����λ����Ϣ������
	float latitude;//����λ����Ϣ��γ��
	int ppm;//��⵽��ppmֵ

}GRID_DATA, * PGRID_DATA;

#define GRID_DATA_SIZE 	(sizeof(GRID_DATA) * MAX_GRID_NUM * MAX_GRID_NUM)
/*
��ȡ��һ�������ļ���
�ļ���������Ϊ��grid-number.dat������numberΪ�����Ĵ������Զ����
*/
void get_file_name(char* file_name)
{
	int number = 1;
	while (true)
	{
		sprintf(file_name, "%s-%d.dat", "grid", number++);
		//����ļ������ڣ��򷵻�
		if (0 != access(file_name, 0))
			break;
	}
}
/*
һ��ֱ��������
data_size:դ���С
Ϊ�˱�����console������ʾ���ٶ�һ�β��õ����������޵ģ�
��ֱ����30���Ӳ��õ�����
�������̨��console����Ļ��СΪMAX_GRID_NUM*MAX_GRID_NUM
��������MAX_GRID_NUM*MAX_GRID_NUM��դ������
��ʼ��γ��Ϊһ�����ֵ��Ȼ�����������γ����Ϊֱ�����Ŀ�ʼλ��
����ֱ�������Сʱ�ܷ��е����ΪMAX_GRID_NUM*MAX_GRID_NUM��λ
������������դ��ľ�γ�ȱ仯Ϊ1����λ��ʵ�ʲ�����������
ֻ���й��������й����ȷ�Χ��73��33��E��135��05��E��γ�ȷ�Χ��3��51��N��53��33��N
PPM�ȼ�10��20��100
������ϣ��Զ������ļ����ļ���������Ϊ��grid-number.dat������numberΪ�����Ĵ������Զ����
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
			printf("���ܴ��ļ�%s\n", file_name);
			return;
		}
	}
	//д�����ʱ��
	time_t now = time(NULL);
	if (save_to_file)
	{
		fwrite(&now, sizeof(time_t), 1, file);
	}
	srand(time(NULL));
	//������ʼ��γ��
	int longitude = rand() % (135 - 73) + 73;
	int latitude = rand() % (53 - 3) + 3;
	//PPM�ȼ�10-100
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
		printf("���˻�������ɣ��Զ����浽�ļ����ļ���Ϊ:%s\n", file_name);
	}
}
void draw_ppm(int ppm)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ppm / 10);
	printf("��");
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
	printf("����ʱ��Ϊ��%s, ��ȾԴ��γ��Ϊ��%.2f E % .2f N\n", local_time_str, longtitude, latitude);
}
/*
��ʾ����դ��ͼ
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
	printf("********�������������ʾ�����ϵͳ\n");
	printf("********��ѡ���Ӧ�Ĳ˵���Ȼ�󰴻س����в���\n");
	printf("********1�����˻����ݲ���\n");
	printf("********2���鿴ʵʱ��������\n");
	printf("********3���鿴������������\n");
	printf("********q���˳�\n");
}
/*
���ļ��ж�ȡ���ݲ���ʾ
*/
void view_old_data()
{
	char file_name[256] = { 0 };
	printf("��������������ļ������Ѿ�������ļ��ڵ�ǰĿ¼���棬\n�ļ�����ʽΪgrid-xx.dat��ʽ��xxΪ���֣���Ȼ�󰴻س�");
	scanf("%s", file_name);
	if (0 != access(file_name, 0))
	{
		printf("�����ļ������ڣ������²���\n");
		return;
	}
	FILE* file = fopen(file_name, "rb");
	if (!file)
	{
		printf("���ܴ��ļ��������²���\n");
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
	printf("���������դ���С��1-10����Ȼ�󰴻س�\n");
	scanf("%d", &size);
	if (size < 1)
	{
		printf("�����դ��С��1���Զ��趨Ϊ1\n");
		size = 1;
	}
	if (size > 10)
	{
		printf("�����դ�����10���Զ��趨Ϊ10\n");
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
		printf("��������%d,������Ϣ��%s\n", err, (char*)msg);
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


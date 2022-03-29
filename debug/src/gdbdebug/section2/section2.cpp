#include <stdio.h>
#include <malloc.h>
#include <string.h>
using namespace std;
#include <string>



typedef struct _DISTRICT_BUS_
{
	char name[100];
	int bus_number;
	_DISTRICT_BUS_* prev;
	_DISTRICT_BUS_* next;

}DISTRICT_BUS, * PDISTRICT_BUS;


typedef struct _BUS_STOP_
{
	char name[100];
	_BUS_STOP_* prev;
	_BUS_STOP_* next;

}BUS_STOP, * PBUS_STOP;

typedef struct _SCHOOL_BUS_
{
	char bus_no[16];
	int bus_seat_number;
	char driver_name[100];
	char driver_phone[20];
	bool ordered;
	struct _SCHOOL_BUS_* prev;
	struct _SCHOOL_BUS_* next;

}SCHOOL_BUS, * PSCHOOL_BUS;

PSCHOOL_BUS bus_queue_head = NULL;
PBUS_STOP bus_stop_head = NULL;
int ordered_bus_number = 0;
PDISTRICT_BUS dis_bus_head = NULL;
void add_bus_stop()
{
	PBUS_STOP new_bus_stop = NULL;
	new_bus_stop = (PBUS_STOP)malloc(sizeof(BUS_STOP));
	new_bus_stop->next = NULL;
	PBUS_STOP prev_bus = bus_stop_head->prev;
	if (prev_bus)
	{
		prev_bus->next = new_bus_stop;
		new_bus_stop->prev = prev_bus;
		bus_stop_head->prev = new_bus_stop;

	}
	else
	{
		bus_stop_head->next = new_bus_stop;
		new_bus_stop->prev = bus_stop_head;
		bus_stop_head->prev = new_bus_stop;
	}

	printf("请输入班车车站名,然后按回车\n");
	scanf("%s", new_bus_stop->name);
	printf("添加班车车站成功\n");
}
//删除站点信息
void delete_bus_stop()
{
	char bus_no[100] = { 0 };
	printf("请输入班车车站名字,然后按回车\n");
	scanf("%s", bus_no);
	PBUS_STOP bus_stop = bus_stop_head->next;

	bool found = false;
	while (bus_stop)
	{
		if (strcmp(bus_stop->name, bus_no) == 0)
		{
			printf("删除班车站点成功\n");
			bus_stop->prev->next = bus_stop->next;
			if (bus_stop->next)
				bus_stop->next->prev = bus_stop->prev;
			free(bus_stop);
			bus_stop = NULL;
			found = true;
			break;
		}

		bus_stop = bus_stop->next;
	}
	if (!found)
	{
		printf("没有找到班车站点 %s\n", bus_no);
	}

}
void view_bus_stop()
{
	PBUS_STOP bus_stop = bus_stop_head->next;
	int num = 1;
	while (bus_stop)
	{
		printf("#%d 班车车站:%s\n", num++, bus_stop->name);
		bus_stop = bus_stop->next;
	}
}
int add_bus()
{
	PSCHOOL_BUS new_bus = NULL;
	new_bus = (PSCHOOL_BUS)malloc(sizeof(SCHOOL_BUS));
	new_bus->next = NULL;
	PSCHOOL_BUS prev_bus = bus_queue_head->prev;
	if (prev_bus)
	{
		prev_bus->next = new_bus;
		new_bus->prev = prev_bus;
		bus_queue_head->prev = new_bus;

	}
	else
	{
		bus_queue_head->next = new_bus;
		new_bus->prev = bus_queue_head;
		bus_queue_head->prev = new_bus;
	}

	printf("请输入班车车牌（15个字符以内）,然后按回车\n");
	scanf("%s", new_bus->bus_no);
	printf("请输入班车座位数,然后按回车\n");
	scanf("%d", &new_bus->bus_seat_number);
	printf("请输入班车驾驶员姓名,然后按回车\n");
	scanf("%s", new_bus->driver_name);
	printf("请输入班车驾驶员电话,然后按回车\n");
	scanf("%s", new_bus->driver_phone);

	printf("添加班车成功\n");
	return 0;
}
int get_seat_num(PSCHOOL_BUS bus)
{
	return bus->bus_seat_number;
}
bool is_ordered(PSCHOOL_BUS bus)
{
	return bus->ordered;
}
int delete_bus()
{
	char bus_no[16] = { 0 };
	printf("请输入班车车牌（15个字符以内）,然后按回车\n");
	scanf("%s", bus_no);
	PSCHOOL_BUS bus = bus_queue_head->next;

	bool found = false;
	while (bus)
	{
		if (is_ordered(bus) && get_seat_num(bus) < 100)
		{
			continue;
		}
		if (strcmp(bus->bus_no, bus_no) == 0)
		{
			printf("删除班车成功\n");
			bus->prev->next = bus->next;
			if (bus->next)
				bus->next->prev = bus->prev;
			free(bus);
			bus = NULL;
			found = true;
			break;
		}

		bus = bus->next;
	}
	if (!found)
	{
		printf("没有找到班车 %s\n", bus_no);
	}
	return 0;
}
int view_buses()
{
	int num = 1;
	PSCHOOL_BUS bus = bus_queue_head->next;
	while (bus)
	{
		printf("#%d 车牌号:%s 座位数:%d 驾驶员姓名%s 驾驶员电话:%s\n", num++, bus->bus_no, bus->bus_seat_number, bus->driver_name, bus->driver_phone);
		bus = bus->next;
	}
	return 0;
}
PSCHOOL_BUS get_bus(const char* bus_no)
{
	PSCHOOL_BUS bus = bus_queue_head->next;
	while (bus)
	{
		if (strcmp(bus->bus_no, bus_no) == 0)
		{
			return bus;
		}
		bus = bus->next;
	}
	return NULL;
}
void view_one_bus()
{
	char bus_no[16] = { 0 };
	printf("请输入班车车牌（15个字符以内）,然后按回车\n");
	scanf("%s", bus_no);

	PSCHOOL_BUS bus = NULL;
	
	if (bus)
	{
		printf("车牌号:%s 座位数:%d 驾驶员姓名%s 驾驶员电话:%s\n", bus->bus_no, bus->bus_seat_number, bus->driver_name, bus->driver_phone);
	}
	else
	{
		printf("没有找到对应的班车信息.\n");
	}
}
int total_bus()
{
	int num = 0;
	PSCHOOL_BUS bus = bus_queue_head->next;
	while (bus)
	{
		num++;
		bus = bus->next;
	}
	return num;
}

void order_bus()
{
	printf("请输入需要的班车数量,然后按回车\n");
	int number = 0;
	scanf("%d", &number);

	if (number + ordered_bus_number > total_bus())
	{
		printf("空闲班车数量不足！预定班车失败\n");
		return;
	}
	else
	{
		ordered_bus_number += number;
	}
	printf("预定班车成功！\n");

}
void view_managed_bus()
{
	PDISTRICT_BUS bus = dis_bus_head->next;
	while (bus)
	{
		printf("校区名:%s,班车数:%d\n", bus->name, bus->bus_number);
		bus = bus->next;
	}
}
void manage_bus()
{
	PDISTRICT_BUS dis_bus = (PDISTRICT_BUS)malloc(sizeof(DISTRICT_BUS));
	printf("请输入校区名称，然后按回车\n");
	scanf("%s", dis_bus->name);
	printf("请输入需要的班车数量，然后按回车\n");
	scanf("%d", &dis_bus->bus_number);
	dis_bus->next = NULL;
	PDISTRICT_BUS prev_bus = dis_bus_head->prev;
	if (prev_bus)
	{
		prev_bus->next = dis_bus;
		dis_bus->prev = prev_bus;
		dis_bus_head->prev = dis_bus;

	}
	else
	{
		dis_bus_head->next = dis_bus;
		dis_bus->prev = dis_bus_head;
		dis_bus_head->prev = dis_bus;
	}

	printf("班车分配成功！\n");

}
/*
保存班车信息
文件名为运行目录下的bus.dat
*/
void save_bus_info_to_file()
{
	FILE* file = fopen("bus.dat", "w+b");
	if (!file)
	{
		return;
	}
	//写入班车信息
	PSCHOOL_BUS bus = bus_queue_head->next;
	while (bus)
	{
		fwrite(bus, sizeof(SCHOOL_BUS), 1, file);
		bus = bus->next;
	}
	fclose(file);

}

/*
从文件中读取班车信息
*/
void read_bus_info_from_file()
{
	FILE* file = fopen("bus.dat", "rb");
	if (!file)	
		return;

	while (true)
	{
		PSCHOOL_BUS new_bus = NULL;
		new_bus = (PSCHOOL_BUS)malloc(sizeof(SCHOOL_BUS));
		size_t read_size = fread(new_bus, 1, sizeof(SCHOOL_BUS), file);
		if (read_size < sizeof(SCHOOL_BUS))
			break;
		new_bus->next = NULL;
		PSCHOOL_BUS prev_bus = bus_queue_head->prev;
		if (prev_bus)
		{
			prev_bus->next = new_bus;
			new_bus->prev = prev_bus;
			bus_queue_head->prev = new_bus;

		}
		else
		{
			bus_queue_head->next = new_bus;
			new_bus->prev = bus_queue_head;
			bus_queue_head->prev = new_bus;
		}
	}
	fclose(file);
}
void print_menu()
{
	printf("********班车管理系统                        ********\n");
	printf("********输入对应的数字进行操作,然后按回车   ********\n");
	printf("********1.添加班车                          ********\n");
	printf("********2.删除班车                          ********\n");
	printf("********3.查看班车                          ********\n");
	printf("********5.预定班车                          ********\n");
	printf("********q.退出                              ********\n");


}

int main(int argc, char* argv[])
{
	bus_queue_head = (PSCHOOL_BUS)malloc(sizeof(SCHOOL_BUS));
	bus_queue_head->next = bus_queue_head->prev = NULL;
	bus_stop_head = (PBUS_STOP)malloc(sizeof(BUS_STOP));
	bus_stop_head->prev = bus_stop_head->next = NULL;

	dis_bus_head = (PDISTRICT_BUS)malloc(sizeof(DISTRICT_BUS));
	dis_bus_head->prev = dis_bus_head->next = NULL;

	read_bus_info_from_file();


	print_menu();

	while (true)
	{
		char c = getchar();
		switch (c)
		{
		case '1':
			add_bus();
			break;
		case '2':
			delete_bus();
			break;
		case '3':
			view_buses();
			break;
		case '4':
			view_one_bus();
			break;
		case '5':
			order_bus();
			break;
		case '6':
			manage_bus();
			break;
		case '7':
			view_managed_bus();
			break;
		case '8':
			add_bus_stop();
			break;
		case '9':
			delete_bus_stop();
			break;
		case '0':
			view_bus_stop();
			break;
		case 'q':
			save_bus_info_to_file();
			return 0;
		case 'm':
			print_menu();
			break;
		case '\n':
			break;
		default:
			printf("不支持的命令，请重新输入\n");
			break;
		}

	}
	return 0;
}



#include <stdio.h>
#include <linux/stddef.h>

#pragma pack(1)
struct test_t1 {
    int a;
    char b;
    short c;
    char d;
};
#pragma pack()

#pragma pack(2)
struct test_t2 {
    int a;
    char b;
    short c;
    char d;
};
#pragma pack()

#pragma pack(4)
struct test_t4 {
    int a;
    char b;
    short c;
    char d;
};
#pragma pack()

#pragma pack(8)
struct test_t8 {
    int a;
    char b;
    short c;
    char d;
};
#pragma pack()


struct test_align {
    int    a;   // 起始offset=0 0%2=0；存放位置区间[0,3]
    char   b;   // 起始offset=4 4%2=0；存放位置区间[4]
    short  c;   // 起始offset=6 6%2=0；存放位置区间[6,7]
    char   d;   // 起始offset=8 8%2=0；存放位置区间[8]
} __attribute((aligned));
struct test_align2 {
    int    a;   // 起始offset=0 0%2=0；存放位置区间[0,3]
    char   b;   // 起始offset=4 4%2=0；存放位置区间[4]
    short  c;   // 起始offset=6 6%2=0；存放位置区间[6,7]
    char   d __attribute((aligned(16)));    // 起始offset=8 8%2=0；存放位置区间[8]
} __attribute((aligned(2)));
struct test_align4 {
    int a;     // 起始offset=0 0%2=0；存放位置区间[0,3]
    char b;    // 起始offset=4 4%2=0；存放位置区间[4]
    short c;   // 起始offset=6 6%2=0；存放位置区间[6,7]
    char d;    // 起始offset=8 8%2=0；存放位置区间[8]
} __attribute((aligned(4)));

int main()
{
	printf("align1: %zd\nalign2: %zd\nalign4: %zd\nalign8: %zd\n",
			sizeof(struct test_t1), sizeof(struct test_t2),
			sizeof(struct test_t4), sizeof(struct test_t8));

	printf("__attribute((aligned))   : %zd\n"
		   "                        a: %p\n"
		   "                        b: %p\n"
		   "                        c: %p\n"
		   "                        d: %p\n\n",
			  sizeof(struct test_align),
			(&((struct test_align*)0)->a),
			(&((struct test_align*)0)->b),
			(&((struct test_align*)0)->c),
			(&((struct test_align*)0)->d));
	printf("__attribute((aligned(2))): %zd\n"
		   "                        a: %p\n"
		   "                        b: %p\n"
		   "                        c: %p\n"
		   "                        d: %p\n\n",
			  sizeof(struct test_align2),
			(&((struct test_align2*)0)->a),
			(&((struct test_align2*)0)->b),
			(&((struct test_align2*)0)->c),
			(&((struct test_align2*)0)->d));
	printf("__attribute((aligned(4))): %zd\n"
		   "                        a: %p\n"
		   "                        b: %p\n"
		   "                        c: %p\n"
		   "                        d: %p\n\n",
			sizeof(struct test_align4),
			(&((struct test_align4*)0)->a),
			(&((struct test_align4*)0)->b),
			(&((struct test_align4*)0)->c),
			(&((struct test_align4*)0)->d));

	printf("\nalign: %zd\nalign4: %zd\nalign4: %zd\n",
			sizeof(struct test_align), sizeof(struct test_align2),
            sizeof(struct test_align4));
    return 0;
}

/*包含头文件*/
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
#include<string.h>
/*图的邻接表类型定义*/
typedef char VertexType[4];
typedef char InfoPtr;
typedef int VRType;
#define MaxSize 50              /*最大顶点个数*/
typedef enum {DG, DN, UG, UN} GraphKind; /*图的类型：有向图、有向网、无向图和无向网*/
typedef struct ArcNode {        /*边结点的类型定义*/
    int adjvex;                 /*弧指向的顶点的位置*/
    InfoPtr* info;              /*与弧相关的信息*/
    struct ArcNode* nextarc;    /*指示下一个与该顶点相邻接的顶点*/
} ArcNode;
typedef struct VNode {          /*头结点的类型定义*/
    VertexType data;            /*用于存储顶点*/
    ArcNode* firstarc;          /*指示第一个与该顶点邻接的顶点*/
} VNode, AdjList[MaxSize];
typedef struct {                /*图的类型定义*/
    AdjList vertex;
    int vexnum, arcnum;         /*图的顶点数目与弧的数目*/
    GraphKind kind;             /*图的类型*/
} AdjGraph;
/*函数声明*/
int LocateVertex(AdjGraph G, VertexType v);
void CreateGraph(AdjGraph* G);
void DisplayGraph(AdjGraph G);
void DestroyGraph(AdjGraph* G);

int LocateVertex(AdjGraph G, VertexType v)
/*返回图中顶点对应的位置*/
{
    int i;

    for (i = 0; i < G.vexnum; i++)
        if (strcmp(G.vertex[i].data, v) == 0) {
            return i;
        }

    return -1;
}
void CreateGraph(AdjGraph* G)
/*采用邻接表存储结构，创建有向图G*/
{
    int i, j, k;
    VertexType v1, v2;              /*定义两个顶点v1和v2*/
    ArcNode* p;
    printf("请输入图的顶点数,边数(逗号分隔): ");
    scanf("%d,%d", &(*G).vexnum, &(*G).arcnum);
    printf("请输入%d个顶点的值:\n", G->vexnum);

    for (i = 0; i < G->vexnum; i++) {   /*将顶点存储在头结点中*/
        scanf("%s", G->vertex[i].data);
        G->vertex[i].firstarc = NULL; /*将相关联的顶点置为空*/
    }

    printf("请输入弧尾和弧头(以空格作为间隔):\n");

    for (k = 0; k < G->arcnum; k++) {   /*建立边链表*/
        scanf("%s%s", v1, v2);
        i = LocateVertex(*G, v1);
        j = LocateVertex(*G, v2);
        /*j为弧头i为弧尾创建邻接表*/
        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = j;
        p->info = NULL;
        p->nextarc = G->vertex[i].firstarc;
        G->vertex[i].firstarc = p;
    }

    (*G).kind = DG;
}

void DestroyGraph(AdjGraph* G)
/*销毁有向图G*/
{
    int i;
    ArcNode* p, *q;

    for (i = 0; i < (*G).vexnum; ++i) { /*释放图中的边表结点*/
        p = G->vertex[i].firstarc;      /*p指向边表的第一个结点*/

        if (p != NULL) {            /*如果边表不为空，则释放边表的结点*/
            q = p->nextarc;
            free(p);
            p = q;
        }
    }

    (*G).vexnum = 0;                /*将顶点数置为0*/
    (*G).arcnum = 0;                /*将边的数目置为0*/
}

void DisplayGraph(AdjGraph G)
/*输出图的邻接矩阵G*/
{
    int i;
    ArcNode* p;
    printf("%d个顶点：\n", G.vexnum);

    for (i = 0; i < G.vexnum; i++) {
        printf("%s ", G.vertex[i].data);
    }

    printf("\n%d条边:\n", G.arcnum);

    for (i = 0; i < G.vexnum; i++) {
        p = G.vertex[i].firstarc;

        while (p) {
            printf("%s→%s ", G.vertex[i].data, G.vertex[p->adjvex].data);
            p = p->nextarc;
        }

        printf("\n");
    }
}
int ExistCycle(AdjGraph G)
/*判断图G是否存在回路*/
{
    int indegree[MaxSize];
    int stack[MaxSize], top = -1, i, j, count = 0;
    ArcNode* p;

    for (i = 0; i < G.vexnum; i++) { /*初始时，将每个结点的入队都置为0*/
        indegree[i] = 0;
    }

    for (i = 0; i < G.vexnum; i++)  /*计算每个结点的入度*/
        for (p = G.vertex[i].firstarc; p != NULL; p = p->nextarc) {
            indegree[p->adjvex]++;
        }

    for (i = 0; i < G.vexnum; i++)  /*如果结点的入度为0，则入栈*/
        if (indegree[i] == 0) {
            top++;
            stack[top] = i;
        }

    while (top !=
           -1) {             /*如果栈不为空，则从栈中的结点出发，进行拓扑排序，如果出现入度为0的结点，则入栈*/
        i = stack[top];
        top--;
        count++;                    /*统计栈中结点的个数*/

        for (p = G.vertex[i].firstarc; p != NULL; p = p->nextarc) {
            j = p->adjvex;
            indegree[j]--;

            if (!indegree[j]) {
                top++;
                stack[top] = j;
            }
        }
    }

    if (count <
        G.vexnum) {         /*如果栈中结点个数小于顶点数，则说明图中存在回路，返回1，否则，返回0*/
        return 1;
    } else {
        return 0;
    }
}
void main()
{
    AdjGraph G;
    int flag;
    printf("采用邻接矩阵创建无向图G：\n");
    CreateGraph(&G);
    printf("输出无向图G：");
    DisplayGraph(G);
    flag = ExistCycle(G);

    if (flag == 1) {
        printf("图G存在回路！\n");
    } else {
        printf("图G不存在回路！\n");
    }

    DestroyGraph(&G);
}

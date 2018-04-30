/*包含头文件*/
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
#include<string.h>
typedef int DataType;           /*栈元素类型定义*/
#include"SeqStack.h"
/*图的邻接表类型定义*/
typedef char VertexType[4];
typedef int InfoPtr;            /*定义为整型，为了存放权值*/
typedef int VRType;
#define MaxSize 50              /*最大顶点个数*/
typedef enum {DG, DN, UG, UN} GraphKind; /*图的类型：有向图、有向网、无向图和无向网*/
typedef struct ArcNode {        /*边结点的类型定义*/
    int adjvex;                 /*弧指向的顶点的位置*/
    InfoPtr* info;              /*弧的权值*/
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
void BriefPath(AdjGraph G, int u, int v);
void PrintPath(int u, int v);
void DfsAllPath(AdjGraph* G, int u, int v);



void BriefPath(AdjGraph G, int u, int v)
/*求图G中从顶点u到顶点v的一条简单路径*/
{
    int k, i;
    SeqStack S;
    ArcNode* p;
    int visited[MaxSize];
    int parent[MaxSize];        /*存储已经访问顶点的前驱顶点*/
    InitStack(&S);

    for (k = 0; k < G.vexnum; k++) { /*访问标志初始化*/
        visited[k] = 0;
    }

    PushStack(&S, u);           /*开始顶点入栈*/
    visited[u] = 1;             /*访问标志置为1*/

    while (!StackEmpty(S)) {    /*广度优先遍历图，访问路径用parent存储*/
        PopStack(&S, &k);
        p = G.vertex[k].firstarc;

        while (p != NULL) {
            if (p->adjvex == v) { /*如果找到顶点v*/
                parent[p->adjvex] = k;  /*顶点v的前驱顶点序号是k*/
                printf("顶点%s到顶点%s的路径是：", G.vertex[u].data, G.vertex[v].data);
                i = v;

                do {                    /*从顶点v开始将路径中的顶点依次入栈*/
                    PushStack(&S, i);
                    i = parent[i];
                } while (i != u);

                PushStack(&S, u);

                while (!StackEmpty(S)) { /*从顶点u开始输出u到v中路径的顶点*/
                    PopStack(&S, &i);
                    printf("%s ", G.vertex[i].data);

                }

                printf("\n");
            } else if (visited[p->adjvex] ==
                       0) { /*如果未找到顶点v且邻接点未访问过，则继续寻找*/
                visited[p->adjvex] = 1;
                parent[p->adjvex] = k;
                PushStack(&S, p->adjvex);
            }

            p = p->nextarc;
        }
    }
}

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
/*采用邻接表存储结构，创建无向图N*/
{
    int i, j, k, w;
    VertexType v1, v2;                  /*定义两个顶点v1和v2*/
    ArcNode* p;
    printf("请输入图的顶点数,边数(以逗号分隔): ");
    scanf("%d,%d", &(*G).vexnum, &(*G).arcnum);
    printf("请输入%d个顶点的值:", G->vexnum);

    for (i = 0; i < G->vexnum; i++) {   /*将顶点存储在头结点中*/
        scanf("%s", G->vertex[i].data);
        G->vertex[i].firstarc = NULL;   /*将相关联的顶点置为空*/
    }

    printf("请输入边的两个顶点(以空格作为分隔):\n");

    for (k = 0; k < G->arcnum; k++) {   /*建立边链表*/
        scanf("%s%s", v1, v2);
        i = LocateVertex(*G, v1);
        j = LocateVertex(*G, v2);
        /*j为入边i为出边创建邻接表*/
        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = j;
        p->info = (InfoPtr*)malloc(sizeof(InfoPtr));
        /*将p指向的结点插入到边表中*/
        p->nextarc = G->vertex[i].firstarc;
        G->vertex[i].firstarc = p;
        /*i为入边j为出边创建邻接表*/
        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = i;
        p->info = NULL;
        p->nextarc = G->vertex[j].firstarc;
        G->vertex[j].firstarc = p;
    }

    (*G).kind = UG;
}
void DestroyGraph(AdjGraph* G)
/*销毁无向图G*/
{
    int i;
    ArcNode* p, *q;

    for (i = 0; i < G->vexnum; ++i) { /*释放图中的边表结点*/
        p = G->vertex[i].firstarc;  /*p指向边表的第一个结点*/

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
/*图G的邻接表的输出*/
{
    int i;
    ArcNode* p;
    printf("该图中有%d个顶点：", G.vexnum);

    for (i = 0; i < G.vexnum; i++) {
        printf("%s ", G.vertex[i].data);
    }

    printf("\n图中共有%d条边:\n", 2 * G.arcnum);

    for (i = 0; i < G.vexnum; i++) {
        p = G.vertex[i].firstarc;

        while (p) {
            printf("(%s,%s) ", G.vertex[i].data, G.vertex[p->adjvex].data);
            p = p->nextarc;
        }

        printf("\n");
    }
}
void main()
{
    AdjGraph G;
    CreateGraph(&G);        /*采用邻接表存储结构创建图G*/
    DisplayGraph(G);        /*输出无向图G*/
    BriefPath(G, 0, 4);     /*求图G中从顶点a到顶点e的简单路径*/
    DestroyGraph(&G);       /*销毁图G*/
}
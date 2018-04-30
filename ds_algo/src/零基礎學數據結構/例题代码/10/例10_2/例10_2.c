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
/*采用邻接表存储结构，创建无向图G*/
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
        /*i为弧头j为弧尾创建邻接表*/
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

    printf("\n%d条边:\n", 2 * G.arcnum);

    for (i = 0; i < G.vexnum; i++) {
        p = G.vertex[i].firstarc;

        while (p) {
            printf("%s→%s ", G.vertex[i].data, G.vertex[p->adjvex].data);
            p = p->nextarc;
        }

        printf("\n");
    }
}
void main()
{
    AdjGraph G;
    printf("采用邻接矩阵创建无向图G：\n");
    CreateGraph(&G);
    printf("输出无向图G：");
    DisplayGraph(G);
    DestroyGraph(&G);
}

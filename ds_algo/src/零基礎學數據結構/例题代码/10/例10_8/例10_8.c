/*包含头文件*/
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
#include<string.h>
typedef int DataType;           /*栈元素类型定义*/

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

void BsfLevel(AdjGraph G, int v0, int k)
/*在图G中，求距离顶点v0最短路径为k的所有顶点*/
{
    int visited[MaxSize];   /*一个顶点访问标志数组，0表示未访问，1表示已经访问*/
    int queue[MaxSize][2];  /*队列queue[][0]存储顶点的序号，queue[][1]存储当前顶点距离v0的路径长度*/
    int front = 0, rear = -1, v, i, level, yes = 0;
    ArcNode* p;

    for (i = 0; i < G.vexnum; i++) { /*初始化标志数组*/
        visited[i] = 0;
    }

    rear = (rear + 1) % MaxSize; /*顶点v0入队列*/
    queue[rear][0] = v0;
    queue[rear][1] = 1;
    visited[v0] = 1;            /*访问数组标志置为1*/
    level = 1;                  /*设置当前层次*/

    do {
        v = queue[front][0];    /*取出队列中顶点*/
        level = queue[front][1];
        front = (front + 1) % MaxSize;
        p = G.vertex[v].firstarc; /*p指向v的第一个邻接点*/

        while (p != NULL) {
            if (visited[p->adjvex] == 0) { /*如果该邻接点未被访问*/
                if (level == k) {       /*如果该邻接点距离v0的最短路径为k，则将其输出*/
                    if (yes == 0)
                        printf("距离%s的最短路径为%2d的顶点有：%s ",
                               G.vertex[v0].data, k, G.vertex[p->adjvex].data);
                    else {
                        printf(",%s", G.vertex[p->adjvex].data);
                    }

                    yes = 1;
                }

                visited[p->adjvex] = 1; /*访问标志置为1*/
                rear = (rear + 1) % MaxSize; /*并将该顶点入队*/
                queue[rear][0] = p->adjvex;
                queue[rear][1] = level + 1;
            }

            p = p->nextarc;             /*如果当前顶点已经被访问，则p移向下一个邻接点*/
        }
    } while (front != rear && level < k + 1);

    printf("\n");
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
    BsfLevel(G, 0, 2);      /*求图G中距离顶点v0最短路径为2的顶点*/
    DestroyGraph(&G);       /*销毁图G*/
}

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

int visited[MaxSize]; /* 访问标志数组*/

void DFSTraverse(AdjGraph G);
void DFS(AdjGraph G, int v);
int FirstAdjVertex(AdjGraph G, VertexType v);
int NextAdjVex(AdjGraph G, VertexType v, VertexType w);
int DFSTraverse2(AdjGraph G, int v);
void BFSTraverse(AdjGraph G);
void Visit(VertexType v);
int LocateVertex(AdjGraph G, VertexType v);
void CreateGraph(AdjGraph* G);
void DestroyGraph(AdjGraph* G);

int FirstAdjVertex(AdjGraph G, VertexType v)
/*返回顶点v的第一个邻接顶点的序号*/
{
    ArcNode* p;
    int v1;
    v1 = LocateVertex(G, v);    /*v1为顶点v在图G中的序号*/
    p = G.vertex[v1].firstarc;

    if (p) {                    /*如果顶点v的第一个邻接点存在，返回邻接点的序号，否则返回-1 */
        return p->adjvex;
    } else {
        return -1;
    }
}
int NextAdjVertex(AdjGraph G, VertexType v, VertexType w)
/*返回v的相对于w的下一个邻接顶点的序号*/
{
    ArcNode* p, *next;
    int v1, w1;
    v1 = LocateVertex(G, v);    /*v1为顶点v在图G中的序号*/
    w1 = LocateVertex(G, w);    /*w1为顶点w在图G中的序号*/

    for (next = G.vertex[v1].firstarc; next;)
        if (next->adjvex != w1) {
            next = next->nextarc;
        }

    p = next;                   /*p指向顶点v的邻接顶点w的结点*/

    if (!p || !p->nextarc) {    /*如果w不存在或w是最后一个邻接点，则返回-1*/
        return -1;
    } else {
        return p->nextarc->adjvex;    /*返回v的相对于w的下一个邻接点的序号*/
    }
}
int DFSTraverse2(AdjGraph G, int v)
/*图的非递归深度优先遍历*/
{
    int i, visited[MaxSize], top;
    ArcNode* stack[MaxSize], *p;

    for (i = 0; i < G.vexnum; i++) { /*将所有顶点都添加未访问标志*/
        visited[i] = 0;
    }

    Visit(G.vertex[v].data);        /*访问顶点v并将访问标志置为1，表示已经访问*/
    visited[v] = 1;
    top = -1;                       /*初始化栈*/
    p = G.vertex[v].firstarc;       /*p指向顶点v的第一个邻接点*/

    while (top > -1 || p != NULL) {
        while (p != NULL)
            if (visited[p->adjvex] ==
                1) { /*如果p指向的顶点已经访问过，则p指向下一个邻接点*/
                p = p->nextarc;
            } else {
                Visit(G.vertex[p->adjvex].data);    /*访问p指向的顶点*/
                visited[p->adjvex] = 1;
                stack[++top] = p;   /*保存p指向的顶点*/
                p = G.vertex[p->adjvex].firstarc; /*p指向当前顶点的第一个邻接点*/
            }

        if (top > -1) {
            p = stack[top--];       /*如果当前顶点都已经被访问，则退栈*/
            p = p->nextarc;         /*p指向下一个邻接点*/
        }
    }
}
void BFSTraverse(AdjGraph G)
/*从第1个顶点出发，按广度优先非递归遍历图G*/
{
    int v, u, w, front, rear;
    ArcNode* p;
    int queue[MaxSize];             /*定义一个队列Q*/
    front = rear = -1;              /*初始化队列Q*/

    for (v = 0; v < G.vexnum; v++) { /*初始化标志位*/
        visited[v] = 0;
    }

    v = 0;
    visited[v] = 1;                 /*设置访问标志为1，表示已经被访问过*/
    Visit(G.vertex[v].data);
    rear = (rear + 1) % MaxSize;
    queue[rear] = v;                /*v入队列*/

    while (front < rear) {          /*如果队列不空*/
        front = (front + 1) % MaxSize;
        v = queue[front];           /*队头元素出队赋值给v*/
        p = G.vertex[v].firstarc;

        while (p != NULL) {         /*遍历序号为v的所有邻接点*/
            if (visited[p->adjvex] == 0) { /*如果该顶点未被访问过*/
                visited[p->adjvex] = 1;
                Visit(G.vertex[p->adjvex].data);
                rear = (rear + 1) % MaxSize;
                queue[rear] = p->adjvex;
            }

            p = p->nextarc;         /*p指向下一个邻接点*/
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

void Visit(VertexType v)
{
    printf("%s ", v);
}
void main()
{
    AdjGraph G;
    printf("采用邻接矩阵创建无向图G：\n");
    CreateGraph(&G);
    printf("图G的深度优先遍历：");
    DFSTraverse2(G, 0);
    printf("\n");
    printf("图G的广度优先遍历：");
    BFSTraverse(G);
    printf("\n");
    DestroyGraph(&G);
}

void DFSTraverse(AdjGraph G)
/*从第1个顶点起，深度优先遍历图G*/
{
    int v;

    for (v = 0; v < G.vexnum; v++) {
        visited[v] = 0;    /*访问标志数组初始化为未被访问*/
    }

    for (v = 0; v < G.vexnum; v++)
        if (!visited[v]) {
            DFS(G, v);    /*对未访问的顶点v进行深度优先遍历*/
        }

    printf("\n");
}
void DFS(AdjGraph G, int v)
/*从顶点v出发递归深度优先遍历图G*/
{
    int w;
    visited[v] = 1;             /*访问标志设置为已访问*/
    Visit(G.vertex[v].data);    /*访问第v个顶点 */

    for (w = FirstAdjVertex(G, G.vertex[v].data); w >= 0;
         w = NextAdjVertex(G, G.vertex[v].data, G.vertex[w].data))
        if (!visited[w]) {
            DFS(G, w);    /*递归调用DFS对v的尚未访问的序号为w的邻接顶点*/
        }
}

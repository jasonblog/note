/*包含头文件及图的类型定义*/
#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdlib.h>
typedef char VertexType[4];
typedef char InfoPtr;
typedef int VRType;
#define INFINITY 100000         /*定义一个无限大的值*/
#define MaxSize 50              /*最大顶点个数*/
typedef int PathMatrix[MaxSize][MaxSize];   /*定义一个保存最短路径的二维数组*/
typedef int
ShortPathLength[MaxSize];       /*定义一个保存从顶点v0到顶点v的最短距离的数组*/
typedef enum {DG, DN, UG, UN} GraphKind; /*图的类型：有向图、有向网、无向图和无向网*/
typedef struct {
    VRType adj;             /*对于无权图，用1表示相邻，0表示不相邻；对于带权图，存储权值*/
    InfoPtr* info;              /*与弧或边的相关信息*/
} ArcNode, AdjMatrix[MaxSize][MaxSize];
typedef struct {                /*图的类型定义*/
    VertexType vex[MaxSize];    /*用于存储顶点*/
    AdjMatrix arc;              /*邻接矩阵，存储边或弧的信息*/
    int vexnum, arcnum;         /*顶点数和边（弧）的数目*/
    GraphKind kind;             /*图的类型*/
} MGraph;
typedef struct {                /*添加一个存储网的行、列和权值的类型定义*/
    int row;
    int col;
    int weight;
} GNode;
void CreateGraph(MGraph* N);
void DisplayGraph(MGraph N);
void Dijkstra(MGraph N, int v0, PathMatrix path, ShortPathLength dist);

void Dijkstra(MGraph N, int v0, PathMatrix path, ShortPathLength dist)
/*用Dijkstra算法求有向网N的v0顶点到其余各顶点v的最短路径P[v]及带权长度D[v]*/
/*final[v]为1表示v∈S，即已经求出从v0到v的最短路径*/
{
    int v, w, i, k, min;
    int final[MaxSize];         /*记录v0到该顶点的最短路径是否已求出*/

    for (v = 0; v < N.vexnum;
         v++) { /*数组dist存储v0到v的最短距离，初始化为v0到v的弧的距离*/
        final[v] = 0;
        dist[v] = N.arc[v0][v].adj;

        for (w = 0; w < N.vexnum; w++) {
            path[v][w] = 0;
        }

        if (dist[v] < INFINITY) { /*如果从v0到v有直接路径，则初始化路径数组*/
            path[v][v0] = 1;
            path[v][v] = 1;
        }
    }

    dist[v0] = 0;                   /*v0到v0的路径为0*/
    final[v0] = 1;                  /*v0顶点并入集合S*/

    /*从v0到其余G.vexnum-1个顶点的最短路径，并将该顶点并入集合S*/
    for (i = 1; i < N.vexnum; i++) {
        min = INFINITY;

        for (w = 0; w < N.vexnum; w++)
            if (!final[w] && dist[w] < min) { /*在不属于集合S的顶点中找到离v0最近的顶点*/
                v = w;                  /*将其离v0最近的顶点w赋给v，其距离赋给min*/
                min = dist[w];
            }

        final[v] = 1;               /*将v并入集合S*/

        for (w = 0; w < N.vexnum;
             w++) /*利用新并入集合S的顶点，更新v0到不属于集合S的顶点的最短路径长度和最短路径数组*/
            if (!final[w] && min < INFINITY && N.arc[v][w].adj < INFINITY &&
                (min + N.arc[v][w].adj < dist[w])) {
                dist[w] = min + N.arc[v][w].adj;

                for (k = 0; k < N.vexnum; k++) {
                    path[w][k] = path[v][k];
                }

                path[w][w] = 1;
            }
    }
}

void main()
{
    int i, j, vnum = 6, arcnum = 9;
    MGraph N;
    GNode value[] = {{0, 1, 30}, {0, 2, 60}, {0, 4, 150}, {0, 5, 40},
        {1, 2, 40}, {1, 3, 100}, {2, 3, 50}, {3, 4, 30}, {4, 5, 10}
    };
    VertexType ch[] = {"v0", "v1", "v2", "v3", "v4", "v5"};
    PathMatrix path;                     /*用二维数组存放最短路径所经过的顶点*/
    ShortPathLength disc;                /*用一维数组存放最短路径长度*/
    CreateGraph(&N, value, vnum, arcnum, ch); /*创建有向网N*/
    DisplayGraph(N);                     /*输出有向网N*/
    Dijkstra(N, 0, path, disc);
    printf("%s到各顶点的最短路径长度为：\n", N.vex[0]);

    for (i = 0; i < N.vexnum; ++i)
        if (i != 0) {
            printf("%s-%s:%d\n", N.vex[0], N.vex[i], disc[i]);
        }
}

void CreateGraph(MGraph* N, GNode* value, int vnum, int arcnum, VertexType* ch)
/*采用邻接矩阵表示法创建有向网N*/
{
    int i, j, k, w, InfoFlag, len;
    char s[MaxSize];
    VertexType v1, v2;
    N->vexnum = vnum;
    N->arcnum = arcnum;

    for (i = 0; i < vnum; i++) {
        strcpy(N->vex[i], ch[i]);
    }

    for (i = 0; i < N->vexnum; i++) /*初始化邻接矩阵*/
        for (j = 0; j < N->vexnum; j++) {
            N->arc[i][j].adj = INFINITY;
            N->arc[i][j].info = NULL; /*弧的信息初始化为空*/
        }

    for (k = 0; k < arcnum; k++) {
        i = value[k].row;
        j = value[k].col;
        N->arc[i][j].adj = value[k].weight;
    }

    N->kind = DN;                   /*图的类型为有向网*/
}
void DisplayGraph(MGraph N)
/*输出邻接矩阵存储表示的图N*/
{
    int i, j;
    printf("有向网具有%d个顶点%d条弧，顶点依次是: ", N.vexnum, N.arcnum);

    for (i = 0; i < N.vexnum; ++i) {    /*输出网的顶点*/
        printf("%s ", N.vex[i]);
    }

    printf("\n有向网N的:\n");           /*输出网N的弧*/
    printf("序号i=");

    for (i = 0; i < N.vexnum; i++) {
        printf("%8d", i);
    }

    printf("\n");

    for (i = 0; i < N.vexnum; i++) {
        printf("%8d", i);

        for (j = 0; j < N.vexnum; j++) {
            printf("%8d", N.arc[i][j].adj);
        }

        printf("\n");
    }
}
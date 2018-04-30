/*包含头文件及图的类型定义*/
#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include<stdlib.h>
typedef char VertexType[4];
typedef char InfoPtr;
typedef int VRType;
#define INFINITY 10000          /*定义一个无限大的值*/
#define MaxSize 50              /*最大顶点个数*/
typedef enum {DG, DN, UG, UN} GraphKind; /*图的类型：有向图、有向网、无向图和无向网*/
typedef struct {
    VRType adj;                 /*对于无权图，用1表示相邻，0表示不相邻；对于带权图，存储权值*/
    InfoPtr* info;              /*与弧或边的相关信息*/
} ArcNode, AdjMatrix[MaxSize][MaxSize];
typedef struct {                /*图的类型定义*/
    VertexType vex[MaxSize];    /*用于存储顶点*/
    AdjMatrix arc;              /*邻接矩阵，存储边或弧的信息*/
    int vexnum, arcnum;         /*顶点数和边（弧）的数目*/
    GraphKind kind;             /*图的类型*/
} MGraph;


/*记录从顶点集合U到V-U的代价最小的边的数组定义*/
typedef struct {
    VertexType adjvex;
    VRType lowcost;
} closeedge[MaxSize];
void CreateGraph(MGraph* N);
int LocateVertex(MGraph N, VertexType v);
void DestroyGraph(MGraph* N);
void DisplayGraph(MGraph N);

void Prim(MGraph G, VertexType u);
int MiniNum(closeedge SZ, MGraph G);

void main()
{

    MGraph N;
    printf("创建一个无向网：\n");
    CreateGraph(&N);
    DisplayGraph(N);
    Prim(N, "A");
    DestroyGraph(&N);
}
void Prim(MGraph G, VertexType u)
/*利用普里姆算法求从第u个顶点出发构造网G的最小生成树T*/
{
    int i, j, k;
    closeedge closedge;
    k = LocateVertex(G, u);     /*k为顶点u对应的序号*/

    for (j = 0; j < G.vexnum; ++j) { /*数组初始化*/
        strcpy(closedge[j].adjvex, u);
        closedge[j].lowcost = G.arc[k][j].adj;
    }

    closedge[k].lowcost = 0;    /*初始时集合U只包括顶点u*/
    printf("无向网的最小生成树的各条边分别是:\n");

    for (i = 1; i < G.vexnum; ++i) { /*选择剩下的G.vexnum-1个顶点*/
        k = MiniNum(closedge, G); /*k为与U中顶点相邻接的下一个顶点的序号*/
        printf("(%s-%s)\n", closedge[k].adjvex, G.vex[k]); /*输出生成树的边*/
        closedge[k].lowcost = 0; /*第k顶点并入U集*/

        for (j = 0; j < G.vexnum; ++j)
            if (G.arc[k][j].adj <
                closedge[j].lowcost) { /*新顶点加入U集后重新将最小边存入到数组*/
                strcpy(closedge[j].adjvex, G.vex[k]);
                closedge[j].lowcost = G.arc[k][j].adj;
            }
    }
}


int MiniNum(closeedge edge, MGraph G)
/*将lowcost的最小值的序号返回*/
{
    int i = 0, j, k, min;

    while (!edge[i].lowcost) {
        i++;
    }

    min = edge[i].lowcost;  /*第一个不为0的值*/
    k = i;

    for (j = i + 1; j < G.vexnum; j++)
        if (edge[j].lowcost > 0 &&
            edge[j].lowcost < min) { /*将最小值对应的序号赋值给k*/
            min = edge[j].lowcost;
            k = j;
        }

    return k;
}

void CreateGraph(MGraph* N)
/*采用邻接矩阵表示法创建有向网N*/
{
    int i, j, k, w, InfoFlag, len;
    char s[MaxSize];
    VertexType v1, v2;
    printf("请输入无向网N的顶点数,弧数,弧的信息(是:1,否:0): ");
    scanf("%d,%d,%d", &(*N).vexnum, &(*N).arcnum, &InfoFlag);
    printf("请输入%d个顶点的值(<%d个字符):\n", N->vexnum, MaxSize);

    for (i = 0; i < N->vexnum; ++i) { /*创建一个数组，用于保存网的各个顶点*/
        scanf("%s", N->vex[i]);
    }

    for (i = 0; i < N->vexnum; i++) /*初始化邻接矩阵*/
        for (j = 0; j < N->vexnum; j++) {
            N->arc[i][j].adj = INFINITY;
            N->arc[i][j].info = NULL; /*弧的信息初始化为空*/
        }

    printf("请输入%d条弧的弧尾 弧头 权值(以空格作为间隔): \n", N->arcnum);

    for (k = 0; k < N->arcnum; k++) {
        scanf("%s%s%d", v1, v2, &w); /*输入两个顶点和弧的权值*/
        i = LocateVertex(*N, v1);
        j = LocateVertex(*N, v2);
        N->arc[i][j].adj = N->arc[j][i].adj = w;

        if (InfoFlag) {             /*如果弧包含其它信息*/
            printf("请输入弧的相关信息: ");
            gets(s);
            len = strlen(s);

            if (len) {
                N->arc[i][j].info = (char*)malloc((len + 1) * sizeof(char)); /* 有向 */
                strcpy(N->arc[i][j].info, s);
            }
        }
    }

    N->kind = DN;                   /*图的类型为有向网*/
}
int LocateVertex(MGraph N, VertexType v)
/*在顶点向量中查找顶点v，找到返回在向量的序号，否则返回-1*/
{
    int i;

    for (i = 0; i < N.vexnum; ++i)
        if (strcmp(N.vex[i], v) == 0) {
            return i;
        }

    return -1;
}

void DestroyGraph(MGraph* N)
/*销毁网N*/
{
    int i, j;

    for (i = 0; i < N->vexnum; i++)         /*释放弧的相关信息*/
        for (j = 0; j < N->vexnum; j++)
            if (N->arc[i][j].adj != INFINITY) /*如果存在弧*/
                if (N->arc[i][j].info != NULL) { /*如果弧有相关信息，释放该信息所占用空间*/
                    free(N->arc[i][j].info);
                    N->arc[i][j].info = NULL;
                }

    N->vexnum = 0; /*将网的顶点数置为0*/
    N->arcnum = 0; /*将网的弧的数目置为0*/
}

void DisplayGraph(MGraph N)
/*输出邻接矩阵存储表示的图G*/
{
    int i, j;
    printf("无向网具有%d个顶点%d条弧，顶点依次是: ", N.vexnum, N.arcnum);

    for (i = 0; i < N.vexnum; ++i) {    /*输出网的顶点*/
        printf("%s ", N.vex[i]);
    }

    printf("\n序号i=");

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

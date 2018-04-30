

void InitMatrix(CrossList* M)
/*初始化稀疏矩阵*/
{
    M->rowhead = M->colhead = NULL;
    M->m = M->n = M->len = 0;
}


void DestroyMatrix(CrossList* M)
/*销毁稀疏矩阵*/
{
    int i;
    OLink p, q;

    for (i = 0; i < M->m; i++) { /*按行释放结点空间*/
        p = *(M->rowhead + i);

        while (p) {
            q = p;
            p = p->right;
            free(q);
        }
    }

    free(M->rowhead);
    free(M->colhead);
    InitMatrix(M);
}

void CreateMatrix(CrossList* M)
/*使用十字链表的存储方式创建稀疏矩阵*/
{
    int i, k;
    int m, n, num;
    OLNode* p, *q;

    if (M->rowhead) {       /*如果链表不空，则释放链表空间*/
        DestroyMatrix(M);
    }

    printf("请输入稀疏矩阵的行数,列数,非零元素的个数: ");
    scanf("%d,%d,%d", &m, &n, &num);
    M->m = m;
    M->n = n;
    M->len = num;
    M->rowhead = (OLink*)malloc(m * sizeof(OLink));

    if (!M->rowhead) {
        exit(-1);
    }

    M->colhead = (OLink*)malloc(n * sizeof(OLink));

    if (!M->colhead) {
        exit(-1);
    }

    for (k = 0; k < m; k++) { /*初始化十字链表，将链表的行指针置为空*/
        M->rowhead[k] = NULL;
    }

    for (k = 0; k < n; k++) { /*初始化十字链表，将链表的列指针置为空*/
        M->colhead[k] = NULL;
    }

    printf("请按任意次序输入%d个非零元的行号、列号及元素值:\n", M->len);

    for (k = 0; k < num; k++) {
        p = (OLink*)malloc(sizeof(OLNode));  /*动态生成结点*/

        if (!p) {
            exit(-1);
        }

        printf("请输入第%d个非零元的行号(0~%d)、列号(0~%d)及元素值:", k + 1, m, n);
        scanf("%d,%d,%d", &p->i, &p->j, &p->e); /*依次输入行号，列号和元素值*/

        /*-----------------------------行插入p结点-----------------------------------*/
        if (M->rowhead[p->i] == NULL ||
            M->rowhead[p->i]->j >
            p->j) { /*如果是第一个结点或当前元素的列号小于表头指向的一个的元素*/
            p->right = M->rowhead[p->i];
            M->rowhead[p->i] = p;
        } else {
            q = M->rowhead[p->i];

            while (q->right && q->right->j < p->j) { /*找到要插入结点的位置*/
                q = q->right;
            }

            p->right = q->right;                /*将p插入到q结点之后*/
            q->right = p;
        }

        /*------------------------------列插入p结点-----------------------------------*/
        q = M->colhead[p->j];                   /*将q指向待插入的链表*/

        if (!q || p->i <
            q->i) {                /*如果p的行号小于表头指针的行号或为空表，则直接插入*/
            p->down = M->colhead[p->j];
            M->colhead[p->j] = p;
        } else {
            while (q->down &&
                   q->down->i < p->i) { /*如果q的行号小于p的行号，则在链表中查找插入位置*/
                q = q->down;
            }

            p->down = q->down;                  /*将p插入到q结点之下*/
            q->down = p;
        }
    }
}

//void PrintMatrix(CrossList M)
///*十字链表的输出*/
//{
//  int i,j;
//  OLink p;
//  printf("%d行%d列%d个非零元素\n",M.m,M.n,M.len);
//  for(j=0;j<M.m;j++)
//  {
//      p=M.rowhead[j];
//      while(p)
//      {
//          printf("%d行%d列的值为%d\n",p->i,p->j,p->e);
//          p=p->right;
//      }
//  }
//}

void PrintMatrix(CrossList M)
/*按矩阵形式输出十字链表*/
{
    int i, j;
    OLink p;

    for (i = 0; i < M.m; i++) {
        p = M.rowhead[i];           /*p指向该行的第1个非零元素*/

        for (j = 0; j < M.n; j++)   /*从第一列到最后一列进行输出*/
            if (!p || p->j != j) {  /*已到该行表尾或当前结点的列值不等于当前列值，则输出0*/
                printf("%-3d", 0);
            } else {
                printf("%-3d", p->e);
                p = p->right;
            }

        printf("\n");
    }
}


void InsertMatrix(CrossList* M, OLink p)
/*按照行序将p插入到稀疏矩阵中*/
{
    OLink q = M->rowhead[p->i];     /*q指向待插行表*/

    if (!q || p->j <
        q->j) {        /*待插的行表空或p所指结点的列值小于首结点的列值，则直接插入*/
        p->right = M->rowhead[p->i];
        M->rowhead[p->i] = p;
    } else {
        while (q->right &&
               q->right->j < p->j) { /*q所指不是尾结点且q的下一结点的列值小于p所指结点的列值*/
            q = q->right;
        }

        p->right = q->right;
        q->right = p;
    }

    q = M->colhead[p->j];               /*q指向待插列表*/

    if (!q || p->i <
        q->i) {            /*待插的列表空或p所指结点的行值小于首结点的行值*/
        p->down = M->colhead[p->j];
        M->colhead[p->j] = p;
    } else {
        while (q->down &&
               q->down->i < p->i) { /*q所指不是尾结点且q的下一结点的行值小于p所指结点的行值*/
            q = q->down;
        }

        p->down = q->down;
        q->down = p;
    }

    M->len++;
}

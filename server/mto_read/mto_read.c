#include "../commonsource/common.h"

typedef struct args
{
    int limit;
    int mapid;
    int row;
    int col;
}args;

#define ARGVNUM 5
#define COLUMN 12

int getArg(int argc,char *argv[],char *serverAddr,int *port);
int mysqlInit(MYSQL *conn_ptr);
int mysqlOpt(MYSQL *conn_ptr, const char *optStr, int *row, int *col, char **result[]);
int mysqlSelect(MYSQL *conn_ptr, const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[]);

int main(int argc,char *argv[])
{
    args arg;
    MYSQL *conn_ptr;
    int r, c;
    getArg(argc,argv,&arg);
    mysqlInit(conn_ptr);
}

int getArg(int argc,char *argv[],args *arg)
{
    const char *argvList[ARGVNUM] = {"--stest","--limit", "--mapid", "--row", "--col"};

    int i, j;

    arg->limit = 10;
    arg->mapid = -1;
    arg->row = -1;
    arg->col = -1;

    for (i = 1; i < argc; ++i)
    {
        for (j = 0; j < ARGVNUM; ++j)
            if (!strcmp(argv[i], argvList[j]))
            {
                switch (j)
                {
                    case 0:
                    {
                        break;
                    }
                    case 1:
                    {
                        arg->limit = atoi(argv[i + 1]);
                        break;
                    }
                    case 2:
                    {
                        arg->mapid = atoi(argv[i + 1]);
                        break;
                    }
                    case 3:
                    {
                        arg->row = atoi(argv[i + 1]);
                        break;
                    }
                    case 4:
                    {
                        arg->col = atoi(argv[i + 1]);
                        break;
                    }

                    default:
                        break;
                };
                break;
            }
    }
}

int mto_readSelect(MYSQL *conn_ptr, args *arg)
{
    char item[200]={0},opt[200]={0};
    char ***result;
    result = (char ***)malloc(sizeof(*result) * arg->limit);
    int i;
    for(i=0;i<arg->limit;++i)
    {
        result[i]=(char **)malloc(sizeof(result) * COLUMN);
        int j;
        for(j=0;j<COLUMN;++j)
            result[i][j]=(char *)malloc(100);
    }
    sprintf(item,"base.time as ��Ϸʱ��,base.stu_no as ѧ��,student.stu_name as ����,base.mapid as MAPID,base.row as ��,base.col as ��,base.score as ����,base.step as ����,base.max as �ϳ�ֵ,base.msec as ʱ��,base.end as ���,base.result as �÷�");
    sprintf(opt, "student.stu_no=base.stuno limit 0,%d ", arg->limit);
    mysqlSelect(conn_ptr,item,"student,base",opt,)
}

int mysqlInit(MYSQL *conn_ptr)
{
    MYSQL *conn_ptr = mysql_init(NULL);
    MYSQL *conn_ptr = mysql_real_connect(MYSQL *conn_ptr, "127.0.0.1", "u1753935", "u1753935", "hw-mto10-u1753935", 0, NULL, 0);
    if(!netif->conn_ptr)
        return -1;
}

int mysqlSelect(MYSQL *conn_ptr, const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[])
{
    char optStr[200] = {0};
    sprintf(optStr,"select %s from %s ",selectItem,tableName);
    if(opt)
    {
        strcat(optStr, "where ");
        strcat(optStr,opt);
    }    
    strcat(optStr,";");
    mysqlOpt(conn_ptr, optStr, row, col, result);
}

int mysqlOpt(MYSQL *conn_ptr, const char *optStr, int *row, int *col, char **result[])
{
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;
    int res, r, c;

    res = mysql_query(conn_ptr, optStr); //��ѯ���

    if (res)
    {
        //printf("SELECT error:%s\n",mysql_error(conn_ptr));��ѯ����
        return -1;
    }
    else
    {
        res_ptr = mysql_store_result(conn_ptr); //ȡ�������
        if (res_ptr && result)
        {
            r  =mysql_num_rows(res_ptr);
            c = mysql_num_fields(res_ptr);
            int i = 0, j = 0;
            while ((sqlrow = mysql_fetch_row(res_ptr)))
            { //����ȡ����¼

                for (i = 0; i < c; ++i)
                {
                    strcpy(result[j][i], sqlrow[i]);
                }    
                ++j;
            }
            if (mysql_errno(conn_ptr))
            {
                fprintf(stderr, "Retrive error:s\n", mysql_error(conn_ptr));
            }
        }
        else
        {
            return -1;
        }
        
        mysql_free_result(res_ptr);
        if (row)
            (*row) = r;
        if (col)
            (*col) = c;
    }
}
#include "common.h"
#include <pwd.h>

typedef struct args
{
    int stuno;
    int limit;
    int mapid;
    int row;
    int col;
}args;

#define ARGVNUM 6
#define COLUMN 12

int getArg(int argc,char *argv[],args* arg);
int mto_readSelect(MYSQL *conn_ptr, args *arg);
int mysqlInit(MYSQL **conn_ptr);
int mysqlOpt(MYSQL *conn_ptr, const char *optStr, int *row, int *col, char **result[]);
int mysqlSelect(MYSQL *conn_ptr, const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[]);

int main(int argc,char *argv[])
{
    args arg;
    MYSQL *conn_ptr;
    int r, c;
    getArg(argc,argv,&arg);
    if(mysqlInit(&conn_ptr)==-1)
        return 0;
    mto_readSelect(conn_ptr, &arg);
    mysql_close(conn_ptr);
    return 0;
}

int getArg(int argc,char *argv[],args *arg)
{
    const char *argvList[ARGVNUM] = {"--stest", "--limit", "--mapid", "--row", "--col", "--user"};

    int i, j;

    struct passwd *pwd = getpwuid(getuid());
    //arg->stuno = atoi(&pwd->pw_name[1]);
    //printf("%d\n", arg->stuno);
    arg->stuno = 1753935;
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
                    case 5:
                    {
                        struct passwd *pwd = getpwuid(getuid());
                        arg->stuno = atoi(pwd->pw_name);
                        printf("%d\n", arg->stuno);
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
    char item[1000]={0},opt[1000]={0};
    char ***result;
    int r = 0, c = 0;
    result = (char ***)malloc(sizeof(*result) * arg->limit);
    int i;
    for(i=0;i<arg->limit;++i)
    {
        result[i]=(char **)malloc(sizeof(result) * COLUMN);
        int j;
        for(j=0;j<COLUMN;++j)
        {
            result[i][j]=(char *)malloc(1000);
            memset(result[i][j], 0, 100);
        }    
    }
    sprintf(item,"base.time as '游戏时间',base.stu_no as '学号',student.stu_name as '姓名',base.mapid as MAPID,base.row as '行',base.col as '列',base.score as '分数',base.step as '步数',base.max as '合成值',base.msec as '时长',base.end as '结果',base.result as '得分'");
    
    if(arg->stuno>0)
        sprintf(opt, "student.stu_no=base.stu_no and base.stu_no=%d ",arg->stuno);

    if(arg->mapid>0)
        sprintf(&opt[strlen(opt)], "and base.stu_no=%d ", arg->mapid);

    if(arg->row>0)
        sprintf(&opt[strlen(opt)], "and base.row=%d ", arg->row);

    if(arg->col>0)
        sprintf(&opt[strlen(opt)], "and base.col=%d ", arg->col);

    if(arg->limit>0)
        sprintf(&opt[strlen(opt)], "limit 0,%d ", arg->limit);

    //mysqlSelect(conn_ptr, "*", "student", NULL, NULL, NULL, result);
    if (mysqlSelect(conn_ptr, item, "student,base", opt, &r, &c, result) == -1)
        ;
    printf("游戏时间\t\t学号\t姓名\tMAPID\t\t行\t列\t分数\t步数\t最大值\t时长\t结果\t\t得分\n");
    for(i=0;i<r;++i)
    {
        int j;
        for(j=0;j<c;++j)
            printf("%s\t", result[i][j]);
        printf("\n");
    }
}

int mysqlInit(MYSQL **conn_ptr)
{
    (*conn_ptr) = mysql_init(NULL);
    (*conn_ptr) = mysql_real_connect((*conn_ptr), "127.0.0.1", "u1753935", "u1753935", "hw-mto10-u1753935", 0, NULL, 0);
    if(!(*conn_ptr))
        return -1;
}

int mysqlSelect(MYSQL *conn_ptr, const char *selectItem, const char *tableName, const char *opt, int *row, int *col, char **result[])
{
    char optStr[1000] = {0};
    sprintf(optStr,"select %s from %s ",selectItem,tableName);
    if(opt)
    {
        strcat(optStr, "where ");
        strcat(optStr,opt);
    }    
    strcat(optStr,";");
    
    if(mysqlOpt(conn_ptr, optStr, row, col, result)==-1)
        return -1;
}

int mysqlOpt(MYSQL *conn_ptr, const char *optStr, int *row, int *col, char **result[])
{
    MYSQL_RES *res_ptr;  
    MYSQL_ROW sqlrow;  
    MYSQL_FIELD *fd;
    int res, r, c;

    res = mysql_query(conn_ptr, optStr); //查询语句

    if (res)
    {
        printf("SELECT error:%s\n",mysql_error(conn_ptr));
        return -1;
    }
    else
    {
        res_ptr = mysql_store_result(conn_ptr); //取出结果集
        if (res_ptr && result)
        {
            r  =mysql_num_rows(res_ptr);
            c = mysql_num_fields(res_ptr);
            int i = 0, j = 0;
            while ((sqlrow = mysql_fetch_row(res_ptr)))
            { //依次取出记录

                for (i = 0; i < c; ++i)
                {
                    strcpy(result[j][i], sqlrow[i]);
                }    
                ++j;
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
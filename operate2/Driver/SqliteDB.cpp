#include "SqliteDB.h"
#include <QDebug>

//sqlite3_errcode() 通常用来获取最近调用的API接口返回的错误代码.
//sqlite3_errmsg() 则用来得到这些错误代码所对应的文字说明. 这些错误信息将以 UTF-8 的编码返回,并且在下一次调用任何SQLite API函数的时候被清除.

SqliteDB::SqliteDB(char *filename):
    m_dbOperateMutex(QMutex::Recursive)
{
    m_db = NULL;
    m_isOpen = false;
    m_filename = (char*)malloc(strlen(filename)+1);
    memcpy(m_filename, filename, strlen(filename)+1);
}

//打开数据库
bool SqliteDB::open()
{
    QMutexLocker locker(&m_dbOperateMutex);

    int ret;

    ret = sqlite3_open(m_filename, &m_db);
    //rc = sqlite3_open_v2(filename,&db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (ret != SQLITE_OK)
    {
        fprintf(stderr,"can't open database%s\n",sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        qDebug("open failed.");

        return false;
    }
    else
    {
        m_isOpen = true;
        qDebug("open successful ...");
    }

    return true;
}

void SqliteDB::close()
{
    QMutexLocker locker(&m_dbOperateMutex);

    sqlite3_close(m_db);
    m_isOpen = false;
}

int SqliteDB::exec(char *sql, bool isQueryType, QList<QList<QVariant> > *data, QVariant ErrMsg)
{
    QMutexLocker locker(&m_dbOperateMutex);
    char * zErrMsg = NULL;
    int ret = SQLITE_OK;

    Callback call_back = (Callback)&SqliteDB::sqliteCallback;

    if(isQueryType)
    {
        if(data == NULL)
        {
            return -1;
        }

        ret = sqlite3_exec(m_db, sql, call_back, (void*)data, &zErrMsg);
    }
    else
    {
        ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
    }

    ErrMsg = QVariant(zErrMsg);

    return ret;
}

int SqliteDB::getTable(char *sql, QList<QList<QVariant> > &data, QVariant ErrMsg)
{
    QMutexLocker locker(&m_dbOperateMutex);
    int ret = 0;
    int nrow = 0;
    int ncolumn = 0;
    char **azResult; //二维数组存放结果
    char *zErrMsg = 0;

    ret = sqlite3_get_table( m_db , sql , &azResult , &nrow , &ncolumn, &zErrMsg);
    ErrMsg = QVariant(zErrMsg);

    qDebug( "row:%d column=%d" , nrow , ncolumn );
    qDebug( "The result of querying is : " );

    QList<QVariant> TmpColData;
    for(int i=1 ; i<(nrow+1); i++ )//第一行数据为表的字段名称
    {
        TmpColData.clear();

        for(int j=1; j<ncolumn; j++)
        {
            qDebug( "azResult[%d] = %s", i*ncolumn+j , azResult[i*ncolumn+j] );
            QVariant tmpData(azResult[i*ncolumn+j]);

            TmpColData.push_back(tmpData);
        }

        if(data.size() < Max_ReadData)
        {
            data.push_back(TmpColData);
            qDebug() << "###data size:" << data.size();
        }
        else
        {
            qDebug() << "###data size == 50, not push anydata";
        }
    }

    sqlite3_free_table(azResult);//释放掉  azResult 的内存空间

    return ret;
}

//回调函数
int SqliteDB::sqliteCallback(void *userData, int numCol, char **colData, char **colName)
{
    QList<QVariant> TmpColData;
    QList<QList<QVariant> > *data = (QList<QList<QVariant> > *)userData;

    for(int i=1; i<numCol; i++)
    {
        QVariant data(colData[i]);

        TmpColData.push_back(data);
    }

    if(data == NULL)
    {
        qDebug() << "data == NULL";
    }
    else if((*data).size()<50)
    {
        (*data).push_back(TmpColData);
        qDebug() << "data size:" << (*data).size();
    }
    else
    {
        qDebug() << "data size == 50, not push anydata";
    }

    //-----------------------Test-----------------------------------
    QString tmpStr;
    for(int i=0; i<TmpColData.size(); i++)
    {
        tmpStr.append(TmpColData.at(i).toString()).append(", ");
    }
    qDebug() << tmpStr;
    //-----------------------Test-----------------------------------


//    int i = 0;
//    int offset = 0;
//    char * buf;
//    char * tmp;

//    buf = (char *)malloc(80 * sizeof(char));
//    tmp = buf;
//    memset(buf,0,40);

//    printf("%d %d\n",sizeof(buf), strlen(buf));
//    for (i=1; i<numCol; i++)
//    {
//        buf = buf + offset;
//        sprintf(buf, "%s=%s ", colName[i], colData[i]);
//        offset = strlen(colData[i]) + 1 + strlen(colName[i])+1; //it's need one place for put a blank so the lenght add 1
//        //printf("i %d offset %d\n",i, offset);
//        //qDebug("");
//    }

//    printf("%s \n", tmp);

//    free(tmp);

//    tmp = NULL;
//    buf = NULL;

    return 0;
}

//创建表
void SqliteDB::createTable()
{
    QMutexLocker locker(&m_dbOperateMutex);
    char * zErrMsg = NULL;
    char * sql =  "CREATE TABLE save_data(num integer primary key, id int, data text, time text)";

    sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
}

//插入数据
void SqliteDB::insertRecord(char *table, int id, char *data, char *time)
{
    QMutexLocker locker(&m_dbOperateMutex);
    char * sql;
    char * zErrMsg = NULL;

    sql = sqlite3_mprintf("insert into %s values(null,%d,'%s','%s')", table, id, data, time);
    sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
    sqlite3_free(sql);
}

//查询特定表中的所有数据
void SqliteDB::searchAll(char *table)
{
    QMutexLocker locker(&m_dbOperateMutex);
    char * sql;
    char * zErrMsg = 0;

    Callback call_back = (Callback)&SqliteDB::sqliteCallback;
    sql = sqlite3_mprintf("select * from %s", table);
    sqlite3_exec(m_db, sql, call_back, 0, &zErrMsg);
    sqlite3_free(sql);
}

//查询特定表中的指定id号数据
void SqliteDB::searchById(char *table, char *id)
{
    QMutexLocker locker(&m_dbOperateMutex);
    char * sql;
    char * zErrMsg = 0;

    Callback call_back = (Callback)&SqliteDB::sqliteCallback;
    sql = sqlite3_mprintf("select * from %s where id=%s", table, id);
    sqlite3_exec(m_db, sql, call_back, 0, &zErrMsg);
    sqlite3_free(sql);
}

//删除特定表中的所有数据
void SqliteDB::deleteAll(char *table)
{
    QMutexLocker locker(&m_dbOperateMutex);
    char * sql;
    char * zErrMsg = 0;

    sql = sqlite3_mprintf("delete from %s" ,table);
    sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
    sqlite3_free(sql);
}

//删除特定表中的指定id号数据
void SqliteDB::deleteById(char *table, char *id)
{
    QMutexLocker locker(&m_dbOperateMutex);
    int rc ;
    char * sql;
    char * zErrMsg = 0;

    sql = sqlite3_mprintf("delete from %s where id=%s", table, id);
    rc = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
    sqlite3_free(sql);
}


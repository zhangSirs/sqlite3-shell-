#ifndef SQLITEDB_H
#define SQLITEDB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QMutex>
#include <QMutexLocker>
#include "sqlite3.h"
#include <vector>
#include <QVariant>
using namespace std;

#define Max_ReadData        50
typedef int (*Callback)(void * , int , char **, char **);

//SQL查询语句执行结果
typedef struct
{
     bool success;
     QVariant err;
     QList< QList<QVariant> > data;

} QueryResult;

class SqliteDB
{
public:
    explicit SqliteDB(char * filename);

    bool open();//打开数据库
    void close();

    int exec(char *sql, bool isQueryType=false, QList<QList<QVariant> > *data=NULL, QVariant ErrMsg=QVariant());

    int getTable(char *sql, QList<QList<QVariant> > &data, QVariant ErrMsg=QVariant());//获取特定表的指定数据

    void createTable();//创建表
    void insertRecord(char *table, int id, char *data, char *time);//插入数据

    void searchAll(char * table);//查询特定表中的所有数据
    void searchById(char * table, char * id);//查询特定表中的指定id号数据

    void deleteAll(char * table);//删除特定表中的所有数据
    void deleteById(char * table, char *id);//删除特定表中的指定id号数据

private:
    static int sqliteCallback(void * userData, int numCol, char **colData, char **colName);//回调函数

private:
    char * m_filename;
    sqlite3 * m_db;
    bool m_isOpen;
    QMutex m_dbOperateMutex;
};


#endif // SQLITEDB_H

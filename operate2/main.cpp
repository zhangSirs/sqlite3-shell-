#include <QApplication>
#include "./Driver/SqliteDB.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SqliteDB db("data.db");
    db.open();
    db.createTable();

    db.insertRecord("save_data", 2000, "5676", "2012-03-12 09:43:55");
    db.insertRecord("save_data", 2001, "5677", "2012-03-12 09:43:56");
//    db.insertRecord("save_data", 2002, "5678", "2012-03-12 09:43:56");
//    db.insertRecord("save_data", 2003, "5678", "2012-03-12 09:43:56");
//    db.insertRecord("save_data", 2004, "5678", "2012-03-12 09:43:56");
//    db.insertRecord("save_data", 2005, "5678", "2012-03-12 09:43:56");
//    db.insertRecord("save_data", 2006, "5678", "2012-03-12 09:43:56");
//    db.insertRecord("save_data", 2007, "5678", "2012-03-12 09:43:56");

//    db.searchAll("save_data");
    QList< QList<QVariant> > data;
    data.clear();
//    db.exec("select * from save_data", true, &data);

    db.getTable("select * from save_data where ie=2900", data);

    return a.exec();
}

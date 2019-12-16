#ifndef INITDB_H
#define INITDB_H

#include <QtSql>
#include <QObject>

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        return QSqlError();
    }
    return QSqlError();
}


#endif // INITDB_H

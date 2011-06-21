#ifndef SEARCHPAGE_H
#define SEARCHPAGE_H

#include <QWidget>

#include "ui_searchpage.h"



class CollectionDB;
class CollectionModel;
class QModelIndex;

class SearchPage : public QWidget, public Ui_SearchPage
{
    Q_OBJECT

public:
	SearchPage(QWidget *parent = 0);
	
	//workaround for needing showcontextmenu in bookmanagerpart FIXME
	//this exposes the indexAt function of the tableview so the context menu
	//can be contextual...
	QModelIndex indexAt(const QPoint& pos);
	
public slots:
    void createBook(dbusBook *book);
    void remBook();
    void updateModel();

private slots:
    void openBook(QModelIndex index);
    void openBook();
	void newQuery();
	void resetQuery();

signals:
	void query(QString *query, QString *column);
    void newBook(dbusBook *book);
    //can't emit a Kurl, so emit it as a string
   Q_SCRIPTABLE void loadBook(QString url);
private:
    CollectionDB *m_db;
    CollectionModel *m_model;

    enum columnLayout {ID, Title, Summary, Author, Release, ReleaseDate, Genre, Location};
	
};

#endif // SEARCHPAGE_H

/*
    Copyright (C) <2011>  Brian Korbein <bri.kor.21@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/
#include "searchpage.h"

#include "collectiondb.h"
#include "collectionmodel.h"
#include "bookstruct.h"

#include <QSqlTableModel>
#include <kdebug.h>
#include <QSqlError>
#include <qheaderview.h>
#include <klocale.h>
#include <QStringList>



SearchPage::SearchPage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);setFocusPolicy(Qt::ClickFocus);
	
	m_db = new CollectionDB();
    m_model = new CollectionModel();
    
	//set up the view
    resultTable->setModel(m_model);
    resultTable->setEditTriggers(QAbstractItemView::SelectedClicked);
    resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	resultTable->setSortingEnabled(true);//enable sorting for the table
    
	//create a prettier header for the view
    QHeaderView *head = resultTable->horizontalHeader();
    head->setResizeMode(QHeaderView::Stretch);
    head->setMovable(true);

    resultTable->hideColumn(ID);
    resultTable->hideColumn(Location);
    show();

    //lots of qstrings... should probably be a qstringlist?
    connect(this, SIGNAL(newBook(dbusBook *)),
            m_db, SLOT(addBook(dbusBook *)));

    //don't forget to update the model if the db gets dirty
    connect(m_db, SIGNAL(isDirty()),
            this, SLOT(updateModel()));

    //load the book on doubleclick anywhere in that row?
    connect(resultTable, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(openBook(QModelIndex)));
	
	//connect the ui signals to the query slots
	connect(searchButton, SIGNAL(clicked(bool)),
			this, SLOT(newQuery()));
			
	connect(resetButton, SIGNAL(clicked(bool)),
		this, SLOT(resetQuery()));
	connect(this, SIGNAL(query(QString*,QString*)),
		m_model, SLOT(query(QString*,QString*)));

}

void SearchPage::createBook(dbusBook *book)
{
    emit newBook(book);
}

void SearchPage::remBook()
{
    //the foreach loops over every column in each row so im using a counter to exit the loop when
    //it starts on the second column.
    QModelIndexList removeUs = resultTable->selectionModel()->selectedIndexes();
    //verify that we got at least one index... if the remove book command is called 
    //without having anything selected we segfault :(
    if(removeUs.length() > 0){
      int index = removeUs.at(0).row();

      foreach(const QModelIndex &removeMe,  removeUs) {
	  int row = removeMe.row();
	  if (row < index) {
	      return;
	  }
	  m_model->removeRow(row);
	  index += 1;
      }
    }
}


void SearchPage::updateModel()
{
    m_model->select();
}

void SearchPage::openBook(QModelIndex index)
{
    //use the layout enum to recreate the index with the books url selected
    //so that the model doesn't need to match the DB ordering...
    QModelIndex urlIndex = m_model->index(index.row(), Location);
    //this bit has caused a lot of issues so i'm leaving a debug in...
    kDebug() << m_model->data(urlIndex).toString();
    KUrl bookUrl = KUrl(m_model->data(urlIndex).toString());
    //emit it as a qstring so we can send it easily over dbus :D
    emit loadBook(bookUrl.url());

}

void SearchPage::openBook()
{
    //this uses selectedIndexes and a  foreach to let us open multiple books as needed without
    //needing to pass in the index. I should probably replace all the uses of 
    //Collection::openBook(QModelIndex index) with this, as its more flexible, in theory at least.
    
    //the foreach loops over every column in each row so im using a counter to exit the loop when
    //it starts on the second column.
    QModelIndexList openUs = resultTable->selectionModel()->selectedIndexes();
    //verify that we got at least one index... if the remove book command is called 
    //without having anything selected we segfault :(
    if(openUs.length() > 0){
      int index = openUs.at(0).row();

      foreach(const QModelIndex &openMe,  openUs) {
      int row = openMe.row();
      if (row < index) {
          return;
      }
      //here we cheat and just call the old version to actually do all the work.
      openBook(openMe);
      index += 1;
      }
    }
    
}

//workaround for needing showcontextmenu in bookmanagerpart FIXME
//this exposes the indexAt function of the tableview so the context menu
//can be contextual...
QModelIndex SearchPage::indexAt(const QPoint& pos)
{
	return resultTable->indexAt(pos);
}

//when the user clicks the search button, pull values from the lineedit and combobox 
//and emit the newQuery signal.
void SearchPage::newQuery()
{
	QString querytext;
	QString columnName;
	
	//get the values from the ui...
	//if the query edit is empty default to *, otherwise emit whatever is contained
	querytext = queryEdit->text();
	columnName = searchTypeBox->currentText();
	
	emit query(&querytext, &columnName);
}


void SearchPage::resetQuery()
{
	//clear the lineedit
	queryEdit->clear();
	//reset the query to the default by emiting a defaulted query
	QString text = "";
	QString column = "title"; //doesn't really matter which column we pick...
	emit query(&text, &column);
}

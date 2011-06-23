/*
    Copyright (C) <2010>  Brian Korbein <bri.kor.21@gmail.com>

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


#ifndef BOOKMANAGERPART_H
#define BOOKMANAGERPART_H

#include <kparts/part.h>

class SearchPage;
class QMenu;
class KAction;

/*
 * This Kpart will hold the book database, and eventually perhaps a dbus based interface to
 * allow the information to be modified by the surrounding app. Its basically just a kpart
 * wrapper around the collection class to make implementing the gui easier, since I'll be able
 * to pawn all the menu update work off on the part manager, like I already do for okular, gwenview
 * etc... also I'm kind of interested in trying out dbus, as I've never used it before.
 */

class BookManagerPart : public KParts::Part
{
    Q_OBJECT
public:
    BookManagerPart(QWidget*, QObject* parent, const QVariantList&);
        
signals:
    void loadBook(QString url);
    
private slots:
    void slotImport();
    void ShowContextMenu(const QPoint &pos);
private:
    void setupActions();
    
    SearchPage *m_searchpage;
    KAction *import;
    KAction *remove;
    KAction *openSelected;
	KAction *search;
    QMenu *m_contextMenu;
};

#endif // BOOKMANAGERPART_H

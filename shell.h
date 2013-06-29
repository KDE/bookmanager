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


#ifndef SHELL_H
#define SHELL_H

#include <QList>

#include <kparts/partmanager.h>
#include <kparts/mainwindow.h>
#include "readerpage.h"
#include <ktoggleaction.h>


class KUrl;
class KTabWidget;
//we want to be able to embed kparts to read the files managed here so we are going to derive from KParts::MainWindow rather than KMainWindow
class Shell : public KParts::MainWindow

{
    Q_OBJECT
public:
    //default Constructor
    Shell(QWidget *parent = 0);
    //default Destructor
    virtual ~Shell();

private slots:
    void slotPartConnect(KParts::Part *newPart);
    void slotPartDisconnect(KParts::BrowserExtension *be);
    void slotOpenFile();
    void slotOpenFileNewTab();
    void slotOpenFileNewTab(QString filename);
    void slotRemoveTab(int index);
    void slotReaderTab(KUrl *url);
    void slotUpdateMenu(int index);
    void slotSaveSession();
    void slotOpenSession();

    void slotToggleCollection();
    void slotSaveConfig();

    QSize sizeHint() const;
private:
    bool readConfig();
    void setupActions();
    void readerTab(const KUrl* url);

    void loadCollection();
    KParts::Part *m_collection;
    KParts::ReadOnlyPart *curPart;

    KTabWidget *mainView;
    KParts::PartManager *m_manager;
    
    QList<ReaderPage *> openPagesList;

    //need to declare actions here so they can be shared between the context and
    //top menu. I think?
    KAction *open;
    KAction *openNewTab;
    KAction *import;
    KAction *remove;
    KAction *print;
    KAction *saveSession;
    KAction *openSession;


    KToggleAction *showCollection;
};
#endif // SHELL_H

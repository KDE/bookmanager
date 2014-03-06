/*
    Copyright (C) 2014  Riccardo Bellini <riccardo.bellini1988@gmail.com>

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

#ifndef BACKUP_COLLECTION_ASSISTANT_H
#define BACKUP_COLLECTION_ASSISTANT_H


// KDE includes
#include <kassistantdialog.h>

// Uis includes
#include "ui_introductionpage.h"

// Forward declarations
class CollectionDB;
class IntroductionPage;
class OrganizeCollectionPage;
class CollectionOrganizerWidget;
class QLabel;
class QCheckBox;
class QProgressBar;
class QPushButton;
class KPageWidgetItem;


class BackupCollectionAssistant : public KAssistantDialog {
public:
    BackupCollectionAssistant(
            CollectionDB * collection,
            QWidget * parent = 0,
            Qt::WindowFlags flags = 0);
    virtual ~BackupCollectionAssistant();

private:
    IntroductionPage * m_introductionPage;
    OrganizeCollectionPage * m_organizeCollectionPage;

    KPageWidgetItem * m_introductionPageItem;
    KPageWidgetItem * m_backupCollectionPageItem;

    CollectionDB * m_collection;
};


// Widget class for introductory page
class IntroductionPage : public QWidget, private Ui::IntroductionPage {
public:
    IntroductionPage(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~IntroductionPage();

private:
};


// Widget class for collection backup
class OrganizeCollectionPage : public QWidget {
    Q_OBJECT
public:
    OrganizeCollectionPage(
            CollectionDB * collection,
            QWidget * parent = 0, 
            Qt::WindowFlags flags = 0);
    virtual ~OrganizeCollectionPage();
    
private slots:
    void collectionOrganizedClicked(bool checked);
    void updateGUI(const QString & book, int percentage);
    void organizationCompleted();
    void organizeCollectionClicked();

private:
    CollectionOrganizerWidget * m_collectionOrganizerWidget;
    
    QLabel * m_introductionLabel;
    QCheckBox * m_organizedCheckBox;
    QPushButton * m_organizeCollectionPushButton;
    QProgressBar * m_organizationProgressBar;
    QPushButton * m_cancelPushButton;
    QLabel * m_currentBookLabel;
    QWidget * m_progressContainerWidget;

    CollectionDB * m_collection;
};

#endif // BACKUP_COLLECTION_ASSISTANT_H

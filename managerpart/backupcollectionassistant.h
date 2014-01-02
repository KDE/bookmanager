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

// Forward declarations
class IntroductionPage;


class BackupCollectionAssistant : public KAssistantDialog {
public:
    BackupCollectionAssistant (QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~BackupCollectionAssistant ();

private:
    IntroductionPage * m_introductionPage;
};


// Widget class for introductory page
class IntroductionPage : public QWidget {
public:
    IntroductionPage (QWidget * parent = 0, Qt::WindowFlags flags = 0);
    virtual ~IntroductionPage ();

private:
    /* data */
};

#endif // BACKUP_COLLECTION_ASSISTANT_H

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

#include "backupcollectionassistant.h"

// KDE includes
#include <klocalizedstring.h>


BackupCollectionAssistant::BackupCollectionAssistant (QWidget * parent,
        Qt::WindowFlags flags)
    : KAssistantDialog (parent, flags)
{
    setCaption (i18n("Backup collection assistant"));
    m_introductionPage = new IntroductionPage;
    
    m_introductionPageItem = addPage (m_introductionPage, i18n("Introduction"));
}


BackupCollectionAssistant::~BackupCollectionAssistant ()
{

}

// IntroductionPage methods
IntroductionPage::IntroductionPage (QWidget * parent, Qt::WindowFlags flags)
    : QWidget (parent, flags)
{
    setupUi (this);
}


IntroductionPage::~IntroductionPage ()
{

}
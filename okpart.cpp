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


#include "okpart.h"

#include <KLocale>
#include <KDebug>
#include <KLibLoader>
#include <KMessageBox>


OkPart::OkPart(const KUrl* url, QWidget* parent)
  : QWidget(parent)
{
   //create the kpart before the tab to verify that we can create the part
  KLibFactory *factory = KLibLoader::self()->factory("okularpart");
  if(!factory){
    KMessageBox::error(this, i18n("Unable to find the Okular Part, please check your installation."));
    m_part = 0;
    return;
  }
  //if we're still here we can create the tab and try to load the file.
  m_part = factory->create<KParts::ReadOnlyPart>(this);
  m_part->openUrl(*url);
}

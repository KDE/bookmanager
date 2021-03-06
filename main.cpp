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
#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>



#include "shell.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("bookmanager", "bookmanager",
                         ki18n("Book Manager"), "0.04",
                         ki18n("An Ebook library manager."),
                         KAboutData::License_GPL,
                         ki18n("Copyright (c) 2010 Brian Korbein"),
                         KLocalizedString(),
                         "https://projects.kde.org/projects/playground/graphics/bookmanager"
                        );
    aboutData.addAuthor(ki18n("Brian Korbein"), ki18n("Creator & Developer"), "bri.kor.21@gmail.com");
    aboutData.addAuthor(ki18n("Riccardo Bellini"), ki18n("Developer"), "riccardo.bellini1988@gmail.com");
    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    Shell* shell = new Shell();
    shell->show();

    return app.exec();
}

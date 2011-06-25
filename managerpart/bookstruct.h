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


#ifndef BOOKSTRUCT_H
#define BOOKSTRUCT_H

#include <QString>

/* This struct defines the basic data needed for a book. and is much
 * nicer to use than remembering the layout for all the function calls :)
 * uses all strings so it can be streamed by dbus, if needed later
 */

struct dbusBook {
    QString title;
    QString summary;
    QString author;
    QString release;
    QString releaseDate;
    QString genre;
    QString url;
};


#endif // BOOKSTRUCT_H

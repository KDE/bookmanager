/*
 *  Copyright (C) 2012  Riccardo Bellini <ricky88ykcir@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <qsize.h>

// cache size of 40 MB
const int CacheSize = 40 * 1000 * 1000;

// size of the preview
const QSize ThumbnailSize(64, 64);

// size of the large preview
const QSize LargeThumbnailSize(300, 350);

#endif // CONSTANTS_H

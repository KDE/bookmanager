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

// Book Manager includes
#include "collectionorganizer.h"
#include "collectiondb.h"


CollectionOrganizer::CollectionOrganizer (CollectionDB * collectionDb,
        QObject * parent)
    : QObject (parent), m_collection(collectionDb)
{

}


CollectionOrganizer::~CollectionOrganizer ()
{

}


void CollectionOrganizer::organizeCollection()
{
    // TODO
}


// getters
KUrl CollectionOrganizer::getRootFolderUrl() const
{
    return m_rootFolderUrl;
}


QString CollectionOrganizer::getCollectionStructure() const
{
    return m_collectionStructure;
}


// setters
void CollectionOrganizer::setRootFolderUrl(const KUrl & url)
{
    m_rootFolderUrl = url;
}


void CollectionOrganizer::setCollectionStructure(const QString & structure)
{
    m_collectionStructure = structure;
}

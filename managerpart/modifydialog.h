/*
    Copyright (C) 2011  Riccardo Bellini <ricky88ykcir@gmail.com>

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


#ifndef MODIFYDIALOG_H
#define MODIFYDIALOG_H

#include <QModelIndexList>

#include <KDialog>

class QDataWidgetMapper;
class QCheckBox;
class CollectionModel;
class ImportWidget;

class ModifyDialog : public KDialog
{
    Q_OBJECT
public:
    ModifyDialog(const QModelIndexList &indexList, CollectionModel *model,
                 QWidget *parent = 0);

public slots:
    virtual void accept();
    virtual void reject();
    
private slots:
    void applyToAllToggled(bool toggle);
    
    void previous();
    void next();
    
    void updateButtons();
    
private:
    QWidget *createMainWidget();
    void setupMappings();
    void applyToAllPrivate();
    
    QWidget *mainWidget;
    ImportWidget *importWidget;
    QCheckBox *applyToAllCheckBox;
    
    QDataWidgetMapper *mapper;
    QModelIndexList indexList;
    int currentPosition;
    
    CollectionModel *collectionModel;
};

#endif // MODIFYDIALOG_H

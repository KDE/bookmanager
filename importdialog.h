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
#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <KDialog>

//don't forget to include the ui-file
#include "ui_importdialog.h"

class ImportDialog :KDialog
{
  Q_OBJECT
  ImportDialog(QWidget *parent = 0);
  
  //I'm creating a second constructor that will autofill some
  //of the fields for kns3 and a possible future directory scanner to use
  ImportDialog(QWidget* parent = 0,QString title = "", QString summary = "", 
				   QString author= "", QString release= "",
				   QString releaseDate= "", QString genre= "", 
				    KUrl *url = 0);
  ~ImportDialog();
  
  private slots:
    void slotImportClicked();
    
  private:
    Ui::importDialog *ui;
};

#endif // IMPORTDIALOG_H

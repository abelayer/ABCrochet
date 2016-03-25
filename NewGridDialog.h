/****************************************************************************
**
** Copyright (C) 2016 Andy Bray
**
** This file is part of AbCrochet.
**
** AbCrochet is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** AbCrochet is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with AbCrochet.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#ifndef NEWGRIDDIALOG_H
#define NEWGRIDDIALOG_H

#include <QDialog>

namespace Ui {
class NewGridDialog;
}

class NewGridDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGridDialog(QWidget *parent = 0);
    ~NewGridDialog();

    QSize getSize();
    void setSize( const QSize& size );

private:
    Ui::NewGridDialog *ui;
};

#endif // NEWGRIDDIALOG_H

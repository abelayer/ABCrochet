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
#ifndef IMAGEUNDOCOMMAND_H
#define IMAGEUNDOCOMMAND_H

#include "GridWidget.h"

#include <QUndoCommand>
#include <QImage>

class ImageUndoCommand : public QUndoCommand
{
public:
    ImageUndoCommand(GridWidget* gridWidget, const QImage& image, QUndoCommand *parent = 0);

    void undo() Q_DECL_OVERRIDE;
    void redo() Q_DECL_OVERRIDE;

private:
    GridWidget* gridWidget;
    QImage image;
};

#endif // IMAGEUNDOCOMMAND_H

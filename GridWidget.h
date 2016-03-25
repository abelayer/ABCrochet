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

#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QWidget>
#include <QPen>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QUndoStack>
#include <QRubberBand>
#include <QPrinter>


class GridWidget : public QWidget
{
    Q_OBJECT

public:
    enum ToolMode {
        None,
        Pen,
        Eraser,
        Select,
        Fill,
        Line,
        Rect,
        Circle
    };

    explicit GridWidget(QWidget* parent = 0);
    virtual ~GridWidget();

    //methods
    void setImage(const QImage& image, bool resetBG = false);
    QImage& getImage(){ return image; }
    int getZoom() const { return zoom; }

    void print( QPrinter& );

    QSize gridSize() { return image.size(); }

    void convertToBlackAndWhite( int threshold );

    void setUndoStack( QUndoStack* stack){ undoStack = stack; }

public slots:
    void onZoomOut();
    void onZoomIn();

    void onPaste();
    void onCut();
    void onCopy();

    void setZoom(int value);
    void setPenColor(const QColor& color);

    void onGreyScale();
    void onBlackAndWhite();

    void onPenMode();
    void onSelectMode();
    void onFillMode();

    void resize();

    void clearBackground(  );

signals:
    void colorChanged(const QColor& color);


protected:
    void paint(QPainter& painter);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent *event);

private:
    QUndoStack* undoStack;
    QRubberBand* rubberBand;

    qint32 completedRow;
    qint32 zoom;
    ToolMode currentTool;
    QImage  image;
    QImage  originalImage;
    QPen    pen;
    QPen    gridPen;

    QColor activeColor;     // this is either pan or backgroundColor depending on left or right button press
    QColor backgroundColor;

    //methods
    void setImagePixel(const QPoint& pos);
    void drawImageBorder(QPainter& qPainter);
    QRect pixelRect(int x, int y) const;
    void fill(int x, int y, const QRgb& initialColor);
    void addUndo();

};
#endif // GRIDWIDGET_H

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

#include "GridWidget.h"
#include "ConvertToBWDialog.h"
#include "UndoCommand.h"

#include <QApplication>
#include <QTimer>
#include <QClipboard>

#define BORDER 10

GridWidget::GridWidget(QWidget* parent) : QWidget(parent),
    pen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin),
    gridPen( Qt::darkGray, 1)
{
    rubberBand = new QRubberBand( QRubberBand::Rectangle, this );

    backgroundColor = Qt::white;
    activeColor = pen.color();

    onPenMode();

    zoom = 8;
    completedRow = 0;

    setAttribute(Qt::WA_StaticContents);
    setImage( QImage(64, 32, QImage::Format_RGB32), true);

    setFocusPolicy( Qt::StrongFocus );
}

GridWidget::~GridWidget() {

}

void GridWidget::addUndo()
{
    QUndoCommand *undoCommand = new ImageUndoCommand(this, image);
    undoStack->push(undoCommand);
}

//public methods
void GridWidget::setImage(const QImage& img, bool resetBG)
{
    image = img;
    completedRow = 0;

    if ( resetBG )
        image.fill(backgroundColor);


    resize();
}

//public slots
void GridWidget::onZoomOut()
{
    setZoom( --zoom );
}

void GridWidget::onZoomIn()
{
    setZoom( ++zoom );
}


void GridWidget::setZoom(int value)
{
    zoom = qMax( 0, qMin(value, 50 ));
    resize();
}

void GridWidget::setPenColor(const QColor& color)
{
    pen.setColor(color);
}

void GridWidget::onPenMode()
{
    setCursor(QCursor(QPixmap(":pics/brushCursor")));
    currentTool = Pen;
    rubberBand->close();
}

void GridWidget::onSelectMode()
{
    setCursor(Qt::CrossCursor);
    currentTool = Select;
}

void GridWidget::onFillMode()
{
    currentTool = Fill;
    setCursor(QCursor(QPixmap(":pics/fillCursor")));
    rubberBand->close();
}

//protected methods
void GridWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if ( currentTool == Pen )
        {
            activeColor = pen.color();

            addUndo();
            setImagePixel(event->pos());
        }
        else if ( currentTool == Fill )
        {
            activeColor = pen.color();

            addUndo();
            setImagePixel(event->pos());
        }
        else if ( currentTool == Select )
        {

            int x = event->pos().x()/zoom * zoom;
            int y = event->pos().y()/zoom * zoom;

            rubberBand->setGeometry(x, y, zoom, zoom);
            rubberBand->show();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if ( currentTool == Pen )
        {
            activeColor = backgroundColor;

            addUndo();
            setImagePixel(event->pos());
        }
        else if ( currentTool == Fill )
        {
            activeColor = backgroundColor;

            addUndo();
            setImagePixel(event->pos());
        }
    }
}

void GridWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if ( currentTool == Pen )
            setImagePixel(event->pos());
        else if ( currentTool == Select )
        {
            QRect rect = rubberBand->geometry();
            int x = qMin( (image.width()-1)*zoom, event->pos().x()/zoom * zoom );
            int y = qMin( (image.height()-1)*zoom, event->pos().y()/zoom * zoom );

            rubberBand->setGeometry(rect.left(), rect.top(), x-rect.left()+zoom, y-rect.top()+zoom);

        }
    }
    else if (event->buttons() & Qt::RightButton)
    {
        if ( currentTool == Pen )
            setImagePixel(event->pos());
    }

}


void GridWidget::keyPressEvent(QKeyEvent *event)
{
    switch( event->key() )
    {
        case Qt::Key_Up:
            completedRow++;
            completedRow = qMax( 0, qMin(completedRow, image.height() ));
            update();
            break;
        case Qt::Key_Down:
            completedRow--;
            completedRow = qMax( 0, qMin(completedRow, image.height() ));
            update();
            break;
        case Qt::Key_Delete:
            clearBackground();
            break;
    }

    QWidget::keyPressEvent( event );
}

void GridWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED( event);

    QPainter painter(this);

    paint( painter );
}

void GridWidget::paint(QPainter& painter)
{
    QRectF target(0.0, 0.0, image.width()*zoom, image.height()*zoom);
    QRectF source(0.0, 0.0, image.width(), image.height());

    painter.drawImage( target, image, source );

    if (zoom >= 5)
    {
        painter.setPen(gridPen);

        for (int i = 0; i <= image.width(); i++)
            painter.drawLine(zoom * i, 0, zoom * i, zoom * image.height());

        for (int i = 0; i <= image.height(); i++)
            painter.drawLine(0, zoom * i, zoom * image.width(), zoom * i);
    }

    drawImageBorder(painter);
}



//private methods
void GridWidget::setImagePixel(const QPoint& pos)
{
    int x = pos.x() / zoom;
    int y = pos.y() / zoom;

    if (image.rect().contains(x, y))
    {
        switch (currentTool)
        {
            case Pen:
            image.setPixel(x, y, activeColor.rgb());
            break;

            case Eraser:
            image.setPixel(x, y, backgroundColor.rgb());
            break;

            case Fill:
            fill(x, y, image.pixel(x, y));
            update();
            break;

            default:
            break;
        }
        update(pixelRect(x, y));
    }
}

void GridWidget::resize()
{
    QWidget::resize( (image.width()+2) * zoom, (image.height()+2) * zoom );
    update();
}

void GridWidget::drawImageBorder(QPainter& painter)
{
    painter.setPen( pen );


    if (zoom > 7)
    {
        for (int i = 0; i < image.height(); i++)
        {
            painter.drawText( image.width()*zoom +2, zoom*i, zoom, zoom, Qt::AlignHCenter|Qt::AlignVCenter,
                              QString::number( image.height()-i ));
        }
    }

    if ( completedRow > 0 )
    {
        QRect rect( 0, (image.height()-completedRow)*zoom, zoom * image.width(), completedRow*zoom);
        painter.fillRect(rect, QBrush(QColor(128, 128, 255, 128)));


        QPen redPen(Qt::red, 2);
        painter.setPen( redPen );
        painter.drawLine(0,(image.height()-completedRow)*zoom, zoom * image.width(), (image.height()-completedRow)*zoom);
    }

    if (zoom > 7)
    {
        painter.setPen( pen );
        for (int i = 0; i < image.width(); i++)
        {
            painter.drawText( zoom*i, (image.height()-completedRow)*zoom+2, zoom, zoom, Qt::AlignHCenter|Qt::AlignVCenter,
                              QString::number( image.width()-i ));
        }
    }
}

QRect GridWidget::pixelRect(int x, int y) const
{
    if (zoom >= 3)
        return QRect(zoom * x + 1, zoom * y + 1, zoom - 1, zoom - 1);

    return QRect(zoom * x, zoom * y, zoom, zoom);
}

void GridWidget::fill(int x, int y, const QRgb& initialColor)
{
    if (image.rect().contains(x, y))
    {
        if (image.pixel(x, y) == initialColor && image.pixel(x, y) != activeColor.rgb())
        {
            image.setPixel(x, y, activeColor.rgb());

            fill(x, y + 1, initialColor);
            fill(x, y - 1, initialColor);
            fill(x + 1, y, initialColor);
            fill(x - 1, y, initialColor);
        }
    }
}

void GridWidget::onGreyScale()
{
    addUndo();

    QRgb color;
    for (int i = 0; i < image.width(); i++)
    {
        for (int j = 0; j < image.height(); j++)
        {
            color = image.pixel(i, j);
            //int gray = qGray(color);
            int gray = (qRed(color) + qGreen(color) + qBlue(color))/3;

            image.setPixel(i, j, qRgb(gray, gray, gray));

        }
    }
    update();
}

void GridWidget::onBlackAndWhite()
{
    addUndo();

    originalImage = image;

    ConvertToBWDialog dlg(this);
    if ( dlg.exec() == QDialog::Rejected )
    {
        image = originalImage;
        update();
    }
}

void GridWidget::convertToBlackAndWhite( int threshold )
{
    for(int x=0; x < image.width(); x++)
        for(int y=0; y < image.height(); y++)
            image.setPixel(x, y, qGray(originalImage.pixel(x, y)) > threshold ? qRgb(255, 255, 255) : qRgb(0, 0, 0));

    update();
}

void GridWidget::onPaste()
{
    QImage newImage = QApplication::clipboard()->image( QClipboard::Clipboard);
    if ( newImage.isNull() )
        return;


    QRect rect = rubberBand->geometry();

    if ( rubberBand->isVisible() && !rect.isEmpty() )
    {
        int x = rect.left() / zoom;
        int y = rect.top() / zoom;
        int w = rect.width() / zoom;
        int h = rect.height() / zoom;

        QPainter p(&image);
        p.drawImage(x,y, newImage.scaled( w, h ) );
    }
    else
    {
        QPainter p(&image);
        p.drawImage(0,0, newImage.scaled( image.width(), image.height() ) );
    }
    update();
}

void GridWidget::onCut()
{
    QRect rect = rubberBand->geometry();

    if ( rubberBand->isVisible() && !rect.isEmpty() )
    {
        int x = rect.left() / zoom;
        int y = rect.top() / zoom;
        int w = rect.width() / zoom;
        int h = rect.height() / zoom;

        QImage newImage = QImage(w, h, QImage::Format_RGB32);
        QPainter p(&newImage);
        p.drawImage(0,0, image, x, y, w, h );

        QApplication::clipboard()->setImage(newImage, QClipboard::Clipboard);

        clearBackground();
    }
    else
    {
        QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
    }
}

void GridWidget::onCopy()
{
    QRect rect = rubberBand->geometry();

    if ( rubberBand->isVisible() && !rect.isEmpty() )
    {
        int x = rect.left() / zoom;
        int y = rect.top() / zoom;
        int w = rect.width() / zoom;
        int h = rect.height() / zoom;

        QImage newImage = QImage(w, h, QImage::Format_RGB32);
        QPainter p(&newImage);
        p.drawImage(0,0, image, x, y, w, h );

        QApplication::clipboard()->setImage(newImage, QClipboard::Clipboard);
    }
    else
    {
        QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
    }
}

void GridWidget::clearBackground(  )
{
    addUndo();

    QRect rect = rubberBand->geometry();

    if ( rubberBand->isVisible() && !rect.isEmpty() )
    {
        int x = rect.left() / zoom;
        int y = rect.top() / zoom;
        int w = rect.width() / zoom;
        int h = rect.height() / zoom;


        for(int c=x; c < x+w; c++)
            for(int r=y; r < y+h; r++)
                image.setPixel(c, r, backgroundColor.rgb() );

        update();
    }
}


void GridWidget::print( QPrinter& printer )
{
    QPainter painter(&printer);
    QRect rect = painter.viewport();
    rect = printer.pageRect();

    QSize size = this->size();
    size.scale(rect.size(), Qt::KeepAspectRatio);

    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow( QRect( QPoint(0,0), this->size() ));

    paint( painter );
}

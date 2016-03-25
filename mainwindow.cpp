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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "NewGridDialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QImageReader>
#include <QImageWriter>
#include <QSettings>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPrinter>
#include <QPrintDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle( QCoreApplication::applicationName() );

    undoStack = new QUndoStack(this);
    ui->gridWidget->setUndoStack( undoStack );
    createActions();
    createMenuAndToolbar();

    setAcceptDrops(true);

    restoreState();


    ui->gridWidget->setFocus(  );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::createActions()
{
    actionNewFile = new QAction( QIcon(":/pics/Document-Empty.png"), tr("New File"), this);
    connect(actionNewFile, SIGNAL(triggered(bool)), SLOT(onNew()));

    actionOpenFile = new QAction(QIcon(":/pics/Folder.png"), tr("Open"), this);
    actionOpenFile->setShortcuts(QKeySequence::Open);
    connect(actionOpenFile, SIGNAL(triggered(bool)), SLOT(onOpen()));

    actionSaveFile = new QAction(QIcon(":/pics/Disk.png"), tr("Save"),this);
    connect(actionSaveFile, SIGNAL(triggered(bool)), SLOT(onSave()));
    actionSaveFile->setShortcuts(QKeySequence::Save);


    actionSaveAs = new QAction(QIcon(":/pics/Save-As.png"), tr("Save As..."),this);
    actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    connect(actionSaveAs, SIGNAL(triggered(bool)), SLOT(onSaveAs()));

    actionImport = new QAction(QIcon(":/pics/Document-Export.png"), tr("Import..."),this);
    connect(actionImport, SIGNAL(triggered(bool)), SLOT(onImport()));

    actionPrint = new QAction( QIcon(":/pics/print.png"), tr("Print..."),this);
    connect(actionPrint, SIGNAL(triggered(bool)), SLOT(onPrint()));

    actionOptions = new QAction(QIcon(":/pics/Setting-Tools.png"), tr("Settings..."),this);

    actionExit = new QAction( tr("Exit"),this);
    actionExit->setShortcuts(QKeySequence::Quit);
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));


    actionUndo = undoStack->createUndoAction(this, tr("&Undo"));
    actionUndo->setShortcuts(QKeySequence::Undo);
    actionUndo->setIcon(QIcon(":/pics/Arrow-Undo.png"));

    actionRedo = undoStack->createRedoAction(this, tr("&Redo"));
    actionRedo->setShortcuts(QKeySequence::Redo);
    actionRedo->setIcon(QIcon(":/pics/Arrow-Redo.png"));

    actionCopy = new QAction(QIcon(":/pics/Page-White-Copy.png"), tr("Copy"),this);
    actionCopy->setShortcut(QKeySequence::Copy);
    connect(actionCopy, SIGNAL(triggered()), ui->gridWidget, SLOT(onCopy()));

    actionCut = new QAction(QIcon(":/pics/Cut.png"), tr("Cut"),this);
    actionCut->setShortcut(QKeySequence::Cut);
    connect(actionCut, SIGNAL(triggered()), ui->gridWidget, SLOT(onCut()));

    actionPaste = new QAction(QIcon(":/pics/Page-White-Paste.png"), tr("Paste"),this);
    actionPaste->setShortcut(QKeySequence::Paste);
    connect(actionPaste, SIGNAL(triggered()), ui->gridWidget, SLOT(onPaste()));

    actionZoomOut = new QAction(QIcon(":/pics/Zoom-Out.png"), tr("Zoom Out"),this);
    actionZoomOut->setShortcuts(QKeySequence::ZoomOut);
    connect(actionZoomOut, SIGNAL(triggered()), ui->gridWidget, SLOT(onZoomOut()));

    actionZoomIn = new QAction(QIcon(":/pics/Zoom-In.png"), tr("Zoom In"),this);
    actionZoomIn->setShortcuts(QKeySequence::ZoomIn);
    connect(actionZoomIn, SIGNAL(triggered()), ui->gridWidget, SLOT(onZoomIn()));

    actionGrayScale = new QAction(QIcon(":/pics/greyscale.png"), tr("Convert to Grey scale"), this);
    actionGrayScale->setObjectName(QStringLiteral("actionGrayScale"));
    connect(actionGrayScale, SIGNAL(triggered()), ui->gridWidget, SLOT(onGreyScale()));

    actionBlackAndWhite = new QAction(QIcon(":/pics/BlackWhite.png"), tr("Convert to B/W"),this);
    connect(actionBlackAndWhite, SIGNAL(triggered()), ui->gridWidget, SLOT(onBlackAndWhite()));

    actionPen = new QAction(QIcon(":/pics/Paintbrush.png"), tr("Pen"),this);
    actionPen->setCheckable(true);
    actionPen->setChecked( true );
    connect(actionPen, SIGNAL(triggered()), ui->gridWidget, SLOT(onPenMode()));

    actionSelect = new QAction(QIcon(":/pics/Select-Restangular.png"), tr("Select"),this);
    actionSelect->setCheckable(true);
    connect(actionSelect, SIGNAL(triggered()), ui->gridWidget, SLOT(onSelectMode()));

    actionFill = new QAction(QIcon(":/pics/Paintcan.png"), tr("Fill"),this);
    actionFill->setCheckable(true);
    connect(actionFill, SIGNAL(triggered()), ui->gridWidget, SLOT(onFillMode()));

    group = new QActionGroup(this);
    group->addAction( actionPen );
    group->addAction( actionSelect );
    group->addAction( actionFill );
}


void MainWindow::createMenuAndToolbar()
{
    menuFile = menuBar()->addMenu(tr("&File"));
    menuFile->addAction(actionNewFile);
    menuFile->addAction(actionOpenFile);
    menuFile->addAction(actionImport);
    menuFile->addSeparator();
    menuFile->addAction(actionSaveFile);
    menuFile->addAction(actionSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(actionPrint);
    menuFile->addSeparator();
    menuFile->addAction(actionExit);

    menuEdit = menuBar()->addMenu(tr("&Edit"));
    menuEdit->addAction(actionUndo);
    menuEdit->addAction(actionRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(actionCopy);
    menuEdit->addAction(actionCut);
    menuEdit->addAction(actionPaste);

    menuView = menuBar()->addMenu(tr("&View"));
    menuView->addAction(actionZoomIn);
    menuView->addAction(actionZoomOut);

    menuTool = menuBar()->addMenu(tr("&Tools"));
    menuTool->addAction(actionPen);
    menuTool->addAction(actionFill);
    menuTool->addAction(actionSelect);


//    menuHelp = menuBar()->addMenu(tr("&Help"));
//    menuHelp->addAction(actionHelp);

    // Add recent files to end of File menu
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(onOpenRecentFile()));
    }

    separatorAct = menuFile->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        menuFile->addAction(recentFileActs[i]);
    updateRecentFileActions();



    ui->mainToolBar->addAction(actionNewFile);
    ui->mainToolBar->addAction(actionOpenFile);
    ui->mainToolBar->addAction(actionSaveFile);
    ui->mainToolBar->addAction(actionPrint);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actionImport);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actionZoomOut);
    ui->mainToolBar->addAction(actionZoomIn);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actionGrayScale);
    ui->mainToolBar->addAction(actionBlackAndWhite);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(actionPen);
    ui->mainToolBar->addAction(actionFill);
    ui->mainToolBar->addAction(actionSelect);

}

void MainWindow::onNew()
{
    NewGridDialog dlg;
    dlg.setSize( defaultGridSize );

    if (dlg.exec() && dlg.result() == QDialog::Accepted)
    {
        ui->gridWidget->setImage( QImage( dlg.getSize(), QImage::Format_RGB32), true );
    }
}

void MainWindow::onOpen()
{
    QString myDocs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);


    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the Crochet file to open"),
                                                    myDocs,
                                                    QCoreApplication::applicationName() + tr(" Files (*.cht)"));
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::onImport()
{
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if ( mimeTypeFilters.count() == 0 )
    {
        foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
            mimeTypeFilters.append(mimeTypeName);
        mimeTypeFilters.sort();
    }

    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    if (dialog.exec() == QDialog::Accepted )
    {
        loadFile( dialog.selectedFiles().first(), true );
    }
}

void MainWindow::onPrint()
{
    QPrinter printer;
    printer.setPageMargins( 5,5,5,5, QPrinter::Millimeter);

    QPrintDialog dialog(&printer, this);

    if (dialog.exec())
    {
        ui->gridWidget->print( printer );
   }
}



void MainWindow::onSave()
{
    if (curFile.isEmpty())
        onSaveAs();
    else
        saveFile(curFile);
}

void MainWindow::onSaveAs()
{
    QString myDocs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    myDocs,
                                                    QCoreApplication::applicationName() + tr(" Files (*.cht)"));
    if (fileName.isEmpty())
        return;

    saveFile(fileName);
}

void MainWindow::onOpenRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}

bool MainWindow::loadFile(const QString &fileName, bool import)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull())
    {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    if ( import )
    {
        // Scale to the required size
        ui->gridWidget->setImage( image.scaled( ui->gridWidget->gridSize() ) );
    }
    else
    {
        ui->gridWidget->setImage( image );
        setCurrentFile(fileName);
    }
    statusBar()->showMessage(tr("File loaded"), 3000);

    return true;
}


void MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer( fileName );
    writer.setFormat("png");

    writer.write( ui->gridWidget->getImage() );


    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 3000);
}



void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;

    if ( !curFile.isEmpty() )
        setWindowTitle( QCoreApplication::applicationName() + " (" + curFile + ")" );
    else
        setWindowTitle( QCoreApplication::applicationName() );

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    updateRecentFileActions();
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::saveState()
{
    QSettings settings;
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("defaultGridSize", defaultGridSize);
    settings.setValue("zoomLevel", ui->gridWidget->getZoom());

}

void MainWindow::restoreState()
{
    QSettings settings;
    restoreGeometry(settings.value("Geometry").toByteArray());

    defaultGridSize = settings.value("defaultGridSize", QSize(64, 32) ).toSize();
    ui->gridWidget->setZoom( settings.value("zoomLevel", 8 ).toInt());

}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        QString fileName = e->mimeData()->urls().first().toLocalFile();


        if ( fileName.contains(".jpg", Qt::CaseInsensitive))
            e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if ( e->mimeData()->urls().count() > 0 )
    {
        loadFile( e->mimeData()->urls().first().toLocalFile() );
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveState();
    event->accept();
}

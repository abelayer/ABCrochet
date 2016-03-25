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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUndoStack>
#include <QActionGroup>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool loadFile(const QString &fileName, bool import = false );

protected:
    void createActions();
    void createMenuAndToolbar();

    void closeEvent(QCloseEvent *);
    void dropEvent(QDropEvent *);
    void dragEnterEvent(QDragEnterEvent *);

    void saveState();
    void restoreState();

    void saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

public slots:
    void onNew();
    void onOpen();
    void onPrint();
    void onSave();
    void onSaveAs();
    void onImport();
    void onOpenRecentFile();

private:
    Ui::MainWindow *ui;

    QSize defaultGridSize;
    QStringList mimeTypeFilters;

    QString curFile;

    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuTool;
    QMenu *menuHelp;


    QAction *actionNewFile;
    QAction *actionOpenFile;
    QAction *actionSaveFile;
    QAction *actionSaveAs;
    QAction *actionImport;
    QAction *actionOptions;
    QAction *actionPrint;
    QAction *actionExit;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCopy;
    QAction *actionCut;
    QAction *actionPaste;
    QAction *actionZoomOut;
    QAction *actionZoomIn;
    QAction *actionGrayScale;
    QAction *actionBlackAndWhite;
    QAction *actionPen;
    QAction *actionSelect;
    QAction *actionFill;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorAct;

    QActionGroup * group;
    QUndoStack *undoStack;

};

#endif // MAINWINDOW_H

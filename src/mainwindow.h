/*
=========================================================================
---[SLSK - Main Window/UI (header file)]---
Copyright (C) 2017 Supremist (aka supremesonicbrazil)
This file is part of Steam Linux Swiss Knife (or SLSK for short).
Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the
accompanying COPYING file for more details.
=========================================================================
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <progressmenu.h>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        Ui::MainWindow *ui;
        void closeEvent(QCloseEvent *);
        static ProgressThread *CopyThread;

    public slots:
        static void StartProgressBars(int TotalGames);
        static void UpdateCopyingLabel(QString text);
        static void UpdateCurrentProgress(int value);
        static void UpdateHeaderLabel(char mode);
        static void UpdateGameLabel(QString text);
        static void UpdateTotalProgress(int value);
        static void UpdateResultsScreen(char mode, char op, QString CurrentSubFolder);
        static void UpdateTotalSize();

    private slots:
        void on_BackupScanSaveBtn_clicked();
        void on_BackupScanConfigBtn_clicked();
        void on_BackupScanGameBtn_clicked();
        void on_RestoreScanSaveBtn_clicked();
        void on_RestoreScanConfigBtn_clicked();
        void on_RestoreScanGameBtn_clicked();
        void on_MarkAllBtn_clicked();
        void on_UnmarkAllBtn_clicked();
        void on_ChangeFolderBtn_clicked();
        void on_StartBtn_clicked();
        void on_CancelBtn_clicked();
        void on_MenuBtn_clicked();
};

#endif // MAINWINDOW_H

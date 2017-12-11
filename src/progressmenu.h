/*
=========================================================================
---[SLSK - Progress Menu functionalities (header file)]---
Copyright (C) 2017 Supremist (aka supremesonicbrazil)
This file is part of Steam Linux Swiss Knife (or SLSK for short).
Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the
accompanying COPYING file for more details.
=========================================================================
*/

#ifndef PROGRESSMENU_H
#define PROGRESSMENU_H

#include <QListWidget>

class ProgressThread : public QObject {
    Q_OBJECT

    public slots:
        static void Start();

    signals:
        void StartProgressBars(int TotalGames);                                      // Signal for setting progress bars
        void UpdateCopyingLabel(QString text);                                       // Signal for updating copying label
        void UpdateCurrentProgress(int value);                                       // Signal for updating current progress
        void UpdateHeaderLabel(char mode);                                           // Signal for updating header label
        void UpdateGameLabel(QString text);                                          // Signal for updating game label
        void UpdateTotalProgress(int value);                                         // Signal for updating total progress
        void UpdateResultsScreen(char mode, char op, QString CurrentSubFolder);      // Signal for bringing and updating results screen
};

class ProgressMenu {
    public:
        static std::string CurrentFolder;
        static bool CancelCheck;
        static bool StopProcess;
        static void StartProcess(char mode, char op, QListWidget* gamelist);
        static void CancelCountdown();

    private:
        static void SetBackupSubFolders(char op);
        static void FetchPaths(QString Game);
        static void Backup(char op, std::string path, int n);
        static void Restore(char op, std::string path, int n);
        static void Delay(int sec);
        static void WriteBackupDate(char op);
};

#endif // PROGRESSMENU_H

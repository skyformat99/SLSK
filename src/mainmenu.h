/*
=========================================================================
---[SLSK - Main Menu functionalities header file]---
Copyright (C) 2017 Supremist (aka supremesonicbrazil)
This file is part of Steam Linux Swiss Knife (or SLSK for short).
Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the
accompanying COPYING file for more details.
=========================================================================
*/

#ifndef MAINMENU_H
#define MAINMENU_H

#include <iostream>
#include "mainwindow.h"

extern QApplication *App;
extern MainWindow *Window;

class MainMenu {
    public:
        static char ProcessMode, ProcessOp;
        static std::string BackupFolder;

        static void LockUnlock();
        static void SwitchMode(char mode, char op);
        static void BackupScan();
        static void RestoreScan();
        static void OrganizeGameList();
        static void CheckUncheck(bool check);
        static bool OneItemChecked();
        static void RemoveUnchecked();
        static void CheckTotalSize();
        static bool CheckDiskSpace();

    private:
        static void ReadBackupDate(char op);
};

#endif // MAINMENU_H

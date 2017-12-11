/*
=========================================================================
---[SLSK - main.cpp]---
Copyright (C) 2017 Supremist (aka supremesonicbrazil)
This file is part of Steam Linux Swiss Knife (or SLSK for short).
Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the
accompanying COPYING file for more details.
=========================================================================
*/

//-----------------------------------------------------------------------------------------------------------
// INCLUDED LIBRARIES
//-----------------------------------------------------------------------------------------------------------

// Standard C++/Qt libs
#include <QApplication>
#include <QDir>
#include <QMessageBox>

// Program-specific libs
#include "mainmenu.h"
#include "progressmenu.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//-----------------------------------------------------------------------------------------------------------
// STARTUP
//-----------------------------------------------------------------------------------------------------------

QApplication *App;  // Pointer for application
MainWindow *Window; // Pointer for main window

// The main man
int main(int argc, char *argv[]){
    QApplication a(argc, argv); App = &a;   // Creating a new application
    MainWindow w; Window = &w;              // Creating the main window

    // Program only opens if the database is present
    if (QDir("/opt/SLSK").exists("SteamLinuxGames.db")){
        Window->show();         // Show the UI
        MainMenu::LockUnlock(); // Lock some buttons
        return App->exec();      // Execute the program

    // If database is not found, show an error dialog and return error code 1
    } else {
        QMessageBox err;
        err.critical(0, "Error", "Database is missing");
        err.setFixedSize(500,200);
        return 1;
    }
}

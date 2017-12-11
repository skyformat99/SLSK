/*
=========================================================================
---[SLSK - Main Menu functionalities]---
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
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QStorageInfo>
#include <QtDebug>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Program-specific/external libs
#include "mainmenu.h"
#include "progressmenu.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"

//-----------------------------------------------------------------------------------------------------------
// VARIABLES
//-----------------------------------------------------------------------------------------------------------

// Chars for process mode (B = backup | R = restore) and option (S = saves | C = configs | G = games)
char MainMenu::ProcessMode = 0, MainMenu::ProcessOp = 0;

// Strings for folder paths
QString FolderName, GameName, CurrentScanSave, CurrentScanConfig, CurrentScanGame, CurrentScanManifest;
std::string MainMenu::BackupFolder = "";

double TotalSize;   // Double for total size of operation

//-----------------------------------------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------------------------------------

/* Enables/Disables buttons on the screen, as in:
   1) Start Button - User has set a backup folder AND has checked at least ONE item in the list
   2) Mark/Unmark All - Game list has at least ONE valid item
   3) Restore Scan Buttons - User has set a backup folder AND, for each scanning button, its respective subfolder exists */
void MainMenu::LockUnlock(){
    // Block for 1)
    if (Window->ui->BackupEntry->text() == "" || !MainMenu::OneItemChecked()){ Window->ui->StartBtn->setDisabled(true); }
    else { Window->ui->StartBtn->setDisabled(false); }

    // Block for 2)
    if (Window->ui->GameList->count() == 0 || Window->ui->GameList->item(0)->text() == "No content found."){
        Window->ui->MarkAllBtn->setDisabled(true); Window->ui->UnmarkAllBtn->setDisabled(true);
    } else {
        Window->ui->MarkAllBtn->setDisabled(false); Window->ui->UnmarkAllBtn->setDisabled(false);
    }

    // Block for 3)
    if (Window->ui->BackupEntry->text() == ""){
        Window->ui->RestoreScanSaveBtn->setDisabled(true);
        Window->ui->RestoreScanConfigBtn->setDisabled(true);
        Window->ui->RestoreScanGameBtn->setDisabled(true);
    } else {
        if (QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamSaves")).exists()){    // Scan for Saves
            Window->ui->RestoreScanSaveBtn->setDisabled(false);
        } else {
            Window->ui->RestoreScanSaveBtn->setDisabled(true);
        }

        if (QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamConfigs")).exists()){  // Scan for Configs
            Window->ui->RestoreScanConfigBtn->setDisabled(false);
        } else {
            Window->ui->RestoreScanConfigBtn->setDisabled(true);
        }

        if (QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamGames")).exists() &&   // Scan for Games
        QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamManifests")).exists()){
            Window->ui->RestoreScanGameBtn->setDisabled(false);
        } else {
            Window->ui->RestoreScanGameBtn->setDisabled(true);
        }
    }
}

// Switches current process mode and option
void MainMenu::SwitchMode(char mode, char op){
    // Set text on screen accordingly
    if (mode == 'B'){
        Window->ui->CurrentMode->setText("Backup");
        switch (op){
            case 'S': Window->ui->CurrentOp->setText("Saves"); break;
            case 'C': Window->ui->CurrentOp->setText("Configs"); break;
            case 'G': Window->ui->CurrentOp->setText("Games"); break;
        }
    } else if (mode == 'R'){
        Window->ui->CurrentMode->setText("Restore");
        switch (op){
            case 'S': Window->ui->CurrentOp->setText("Saves"); break;
            case 'C': Window->ui->CurrentOp->setText("Configs"); break;
            case 'G': Window->ui->CurrentOp->setText("Games"); break;
        }
    }
    MainMenu::ProcessMode = mode; MainMenu::ProcessOp = op; // Set new process mode and option
}

// Read the last backup date of chosen option
void MainMenu::ReadBackupDate(char op){
    // Setting file name according to option
    QString filename;
    switch (op){
        case 'S': filename = Window->ui->BackupEntry->text() + "/LastSaveBackup.txt"; break;
        case 'C': filename = Window->ui->BackupEntry->text() + "/LastConfigBackup.txt"; break;
        case 'G': filename = Window->ui->BackupEntry->text() + "/LastGameBackup.txt"; break;
    }

    // Opening the file and reading content
    QFile file(filename); QString date, time;
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    date = stream.readLine(); time = stream.readLine();
    file.close();

    Window->ui->LastBackupLabel->setText("Backup date:\n" + date + "\n" + time);    // Display the content in the UI
}


// Functions that scan folders for content
// For Backup Mode - scans Steam folder
void MainMenu::BackupScan(){
    Window->ui->GameList->clear();                  // Clearing the list beforehand
    Window->ui->ScanningLabel->setVisible(true);    // Showing the scan message
    App->processEvents();                           // Updating GUI

    // Starting scan process based on option:
    // Saves
    if (MainMenu::ProcessOp == 'S'){
        for (int ct = 1; ct <= 3; ct++){
            QStringList SaveList = DB::FetchAllInfo('S', ct);   // Fill a list with save paths
            bool GameAdded = false;                             // Bool for knowing if a game has been added to the list

            // Iterate through each save path
            foreach (QString SavePath, SaveList){
                // If save path is valid:
                if (SavePath != "[N/A]" && SavePath != "[UNKNOWN]" && SavePath != "[CLOUD-ONLY]"){
                    // Fetch its respective game folder
                    ProgressMenu::CurrentFolder = DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames WHERE SavePath" + std::to_string(ct) + " = '" + SavePath.toStdString() + "'");
                    DB::ReplaceLabels(SavePath);    // Replace path labels

                    // If path happens to end in a file or has a wildcard, convert path to its parent directory
                    if (QFileInfo(SavePath).isFile() || SavePath.contains('*')){ SavePath = QFileInfo(SavePath).dir().path(); }

                    // If the folder exists in the system:
                    if (QDir(SavePath).exists()){
                        // Check if said game has been added to the list previously
                        for (int ct2 = 0; ct2 < Window->ui->GameList->count(); ct2++){
                            if (Window->ui->GameList->item(ct2)->text() == QString::fromStdString(ProgressMenu::CurrentFolder)){ GameAdded = true; }
                        }

                        if (GameAdded == false){ Window->ui->GameList->addItem(QString::fromStdString(ProgressMenu::CurrentFolder)); }    // If it hasn't, add it to the list
                    }
                }
            }
        }

    // Configs
    } else if (MainMenu::ProcessOp == 'C'){
        for (int ct = 1; ct <= 3; ct++){
            QStringList ConfigList = DB::FetchAllInfo('C', ct); // Fill a list with config paths
            bool GameAdded = false;                             // Bool for knowing if a game has been added to the list

            // Iterate through each config path
            foreach (QString ConfigPath, ConfigList){
                // If config path is valid:
                if (ConfigPath != "[N/A]" && ConfigPath != "[UNKNOWN]" && ConfigPath != "[CLOUD-ONLY]"){
                    // Fetch its respective game folder
                    ProgressMenu::CurrentFolder = DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames WHERE ConfigPath" + std::to_string(ct) + " = '" + ConfigPath.toStdString() + "'");
                    DB::ReplaceLabels(ConfigPath);    // Replace path labels

                    // If path happens to end in a file or has a wildcard, convert path to its parent directory
                    if (QFileInfo(ConfigPath).isFile() || ConfigPath.contains('*')){ ConfigPath = QFileInfo(ConfigPath).dir().path(); }

                    // If the folder exists in the system:
                    if (QDir(ConfigPath).exists()){
                        // Check if said game has been added to the list previously
                        for (int ct2 = 0; ct2 < Window->ui->GameList->count(); ct2++){
                            if (Window->ui->GameList->item(ct2)->text() == QString::fromStdString(ProgressMenu::CurrentFolder)){ GameAdded = true; }
                        }

                        if (GameAdded == false){ Window->ui->GameList->addItem(QString::fromStdString(ProgressMenu::CurrentFolder)); }    // If it hasn't, add it to the list
                    }
                }
            }
        }

    // Games
    } else if (MainMenu::ProcessOp == 'G'){
        // Setting game iterator and manifest path
        QDirIterator GameIterator(QString::fromStdString(DB::SteamPath), QDir::Dirs | QDir::NoDotAndDotDot);
        std::string ManifestPath = DB::SteamPath;
        ManifestPath.erase(ManifestPath.end() - 7, ManifestPath.end()); // Removing "/common"

        // Iterating through each game folder in steamapps:
        while (GameIterator.hasNext()){
            GameIterator.next();    // Go to next one

            // (Re)set manifest iterator and iterate through each appmanifest:
            QDirIterator ManifestIterator(QString::fromStdString(ManifestPath), QDir::Files);
            while (ManifestIterator.hasNext()){
                ManifestIterator.next();    // Go to next one

                // If the manifest's AppID matches the game's AppID, add the game to the list
                if (ManifestIterator.fileName().contains(QString::fromStdString(DB::FetchGameInfo("SELECT AppID FROM "
                    "RegisteredGames WHERE GameFolder = '" + GameIterator.fileName().toStdString() + "'")))){
                    Window->ui->GameList->addItem(GameIterator.fileName());
                }
            }
        }
    }

    ProgressMenu::CurrentFolder = "";               // Cleaning folder variable
    Window->ui->LastBackupLabel->setText("");       // Finally, erase content from last backup label...
    Window->ui->ScanningLabel->setVisible(false);   // ...hide the scan message...
    Window->ui->TotalSizeLabel->setText("");        // ...and the total size message
}

// For Restore Mode - scans backup subfolders
void MainMenu::RestoreScan(){
    Window->ui->GameList->clear();                  // Clearing the list beforehand
    Window->ui->ScanningLabel->setVisible(true);    // Showing the scan message
    App->processEvents();                           // Updating GUI
    bool done = false;                              // Setting boolean for scanning

    // Starting scan process based on option:
    // Saves
    if (MainMenu::ProcessOp == 'S' && QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamSaves")).exists()){
        // Setting save iterator
        QDirIterator SaveIterator(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamSaves"), QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);

        // Fetching content
        while (!done){
            if (SaveIterator.hasNext()){
                SaveIterator.next();    // Iterate to the next save folder

                for (int ct = 1; ct <= 3; ct++){
                    // Search for the respective game folder
                    CurrentScanSave = QString::fromStdString(DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames "
                    "WHERE SaveFolder" + std::to_string(ct) + " = '" + SaveIterator.fileName().toStdString() + "'"));

                    // Quick fix for multiple save directories
                    if (CurrentScanSave == "NONE"){
                        CurrentScanSave = QString::fromStdString(DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames "
                        "WHERE SaveFolder" + std::to_string(ct) + " = '" + DB::FetchGameInfo("SELECT SaveFolder" + std::to_string(ct) +
                        " FROM RegisteredGames WHERE SavePath" + std::to_string(ct) + " LIKE '%" + SaveIterator.fileName().toStdString() + "%'") + "'"));
                    }

                    // If game folder is found and game exists on database, add game to the list and exit inner loop
                    if (CurrentScanSave != "NONE" && DB::CheckGameOnDB(CurrentScanSave) == true){
                        Window->ui->GameList->addItem(CurrentScanSave); ct = 4;
                    }
                }
            } else { done = true; } // Exit outer loop if there are no more folders left
        }

    // Configs
    } else if (MainMenu::ProcessOp == 'C' && QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamConfigs")).exists()){
        // Setting config iterator
        QDirIterator ConfigIterator(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamConfigs"), QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);

        // Fetching content
        while (!done){
            if (ConfigIterator.hasNext()){
                ConfigIterator.next();  // Iterate to the next config folder

                for (int ct = 1; ct <= 3; ct++){
                    // Search for the respective folder
                    CurrentScanConfig = QString::fromStdString(DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames "
                    "WHERE ConfigFolder" + std::to_string(ct) + " = '" + ConfigIterator.fileName().toStdString() + "'"));

                    // If game folder is found and game exists on database, add game to the list and exit inner loop
                    if (CurrentScanConfig != "NONE" && DB::CheckGameOnDB(CurrentScanConfig) == true){
                        Window->ui->GameList->addItem(CurrentScanConfig); ct = 4;
                    }
                }
            } else { done = true; } // Exit outer loop if there are no more folders left
        }

    // Games
    } else if (ProcessOp == 'G' && QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamGames")).exists() &&
                            QDir(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamManifests")).exists()){
        // Setting game iterator
        QDirIterator GameIterator(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamGames"), QDir::Dirs | QDir::NoDotAndDotDot);

        // Fetching content
        while (!done){
            if (GameIterator.hasNext()){
                GameIterator.next();        // Iterate to the next game folder
                bool foundmanifest = false; // Setting flag for when the correct manifest is found

                // (Re)setting manifest iterator every loop
                QDirIterator ManifestIterator(Window->ui->BackupEntry->text() + QString::fromStdString("/SteamManifests"), QDir::Files | QDir::NoDotAndDotDot);

                // Fetching respective manifest
                while (ManifestIterator.hasNext() && !foundmanifest){
                    // Iterate to the next manifest file
                    ManifestIterator.next();

                    // Search for repective folder
                    CurrentScanManifest = QString::fromStdString(DB::FetchGameInfo("SELECT AppID FROM RegisteredGames "
                                          "WHERE GameFolder = '" + GameIterator.fileName().toStdString() + "'"));

                    /* If game folder is found, exists in database and its respective manifest is found,
                       add game to list and exit inner loop */
                    if (CurrentScanManifest != "NONE" && DB::CheckGameOnDB(GameIterator.fileName()) == true &&
                    ManifestIterator.fileName().contains(CurrentScanManifest)){
                        Window->ui->GameList->addItem(GameIterator.fileName());
                        foundmanifest = true;
                    }

                }
            } else { done = true; } // Exit outer loop if there are no more folders left
        }
    }
    MainMenu::ReadBackupDate(MainMenu::ProcessOp);  // Finally, scan for the backup date and time...
    Window->ui->ScanningLabel->setVisible(false);   // ...hide the scan message...
    Window->ui->TotalSizeLabel->setText("");        // ...and the total size message
}

// Function that organizes game list
void MainMenu::OrganizeGameList(){
    // If there's no items in the list, display a message
    if (Window->ui->GameList->count() == 0){ Window->ui->GameList->addItem("No content found."); }
    else {
        /* For each game in the list:
           1) - Switch folder name with proper game name
           2) - Spawn a checkbox
           3) - Set the checkbox as unchecked */
        for (int ct = 0; ct < Window->ui->GameList->count(); ct++){
            FolderName = Window->ui->GameList->item(ct)->text();
            GameName = QString::fromStdString(DB::FetchGameInfo("SELECT SteamName FROM RegisteredGames WHERE "
                                                                "GameFolder = '" + FolderName.toStdString() + "'"));
            Window->ui->GameList->item(ct)->setText(GameName);                                      // 1)
            Window->ui->GameList->item(ct)->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);  // 2)
            Window->ui->GameList->item(ct)->setCheckState(Qt::Unchecked);                           // 3)
        }
        Window->ui->GameList->sortItems(Qt::AscendingOrder); // Finally, order the game list in alphabetical order
    }
}

// Checks/Unchecks all games in the list
void MainMenu::CheckUncheck(bool check){
    // Iterate through each item in the list...
    for (int ct = 0; ct < Window->ui->GameList->count(); ct++){
        if (check == true){ Window->ui->GameList->item(ct)->setCheckState(Qt::Checked); }   // ...if unchecked, check...
        else { Window->ui->GameList->item(ct)->setCheckState(Qt::Unchecked); }              // ...if checked, uncheck
    }
}

// Checks if at least one item in the list was checked before starting backup/restore
bool MainMenu::OneItemChecked(){
    bool CheckConfirmed = false;    // Setting flag for when one checked item is found

    // Iterate through each item in the list - if one item is checked...
    for (int ct = 0; ct < Window->ui->GameList->count(); ct++)
        if (Window->ui->GameList->item(ct)->checkState() == Qt::Checked){ CheckConfirmed = true; } // ...change flag to "true"

    return CheckConfirmed;          // Returning the flag
}

// Removes items that are not checked from the list (for backup/restore process)
void MainMenu::RemoveUnchecked(){
    for (int ct = 0; ct < Window->ui->GameList->count(); ct++){ // Iterate through each item in the list and if it is unchecked...
        if (Window->ui->GameList->item(ct)->checkState() == Qt::Unchecked){ Window->ui->GameList->takeItem(ct); ct--; } // ...remove it from list and update counter
    }
}

// Calculates total size of checked items
void MainMenu::CheckTotalSize(){
    TotalSize = 0; QDir CurrentDir; QString CurrentPath;                        // Reset total size value and create temps for path handling
    for (int ct = 0; ct < Window->ui->GameList->count(); ct++){                 // For each game in the list:
        if (Window->ui->GameList->item(ct)->checkState() == Qt::Checked){       // If item is checked:
            if (ProcessMode == 'B'){                                            // Filter by mode and option
                switch (ProcessOp){
                case 'S':
                    for (int ct2 = 1; ct2 <= 3; ct2++){
                        // Find the respective path, replacing its labels
                        CurrentPath = QString::fromStdString(DB::FetchGameInfo("SELECT SavePath" + std::to_string(ct2) + " FROM RegisteredGames"
                                      " WHERE SteamName = '" + Window->ui->GameList->item(ct)->text().toStdString() + "'"));
                        DB::ReplaceLabels(CurrentPath);                                                                         // Replace path labels
                        CurrentDir.setPath(CurrentPath); CurrentDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);              // Set the temp dir
                        if (CurrentDir.exists()){                                                                               // If dir exists:
                            QDirIterator PathIterator(CurrentDir, QDirIterator::Subdirectories);                                // Iterate through it...
                            while (PathIterator.hasNext()){
                                PathIterator.next(); TotalSize += (double)(QFileInfo(PathIterator.filePath()).size()) / (double)1024;   // ...and add each file's size to the total (in KB)
                            }
                        }
                    }
                    break;
                case 'C':
                    for (int ct2 = 1; ct2 <= 3; ct2++){
                        CurrentPath = QString::fromStdString(DB::FetchGameInfo("SELECT ConfigPath" + std::to_string(ct2) + " FROM RegisteredGames"
                                      " WHERE SteamName = '" + Window->ui->GameList->item(ct)->text().toStdString() + "'"));
                        DB::ReplaceLabels(CurrentPath);
                        CurrentDir.setPath(CurrentPath); CurrentDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
                        if (CurrentDir.exists()){
                            QDirIterator PathIterator(CurrentDir, QDirIterator::Subdirectories);
                            while (PathIterator.hasNext()){
                                PathIterator.next(); TotalSize += (double)(QFileInfo(PathIterator.filePath()).size()) / (double)1024;
                            }
                        }
                    }
                    break;
                case 'G':
                    CurrentPath = QString::fromStdString(DB::SteamPath + "/" + DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames"
                                  " WHERE SteamName = '" + Window->ui->GameList->item(ct)->text().toStdString() + "'"));
                    DB::ReplaceLabels(CurrentPath);
                    CurrentDir.setPath(CurrentPath); CurrentDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
                    if (CurrentDir.exists()){
                        QDirIterator PathIterator(CurrentDir, QDirIterator::Subdirectories);
                        while (PathIterator.hasNext()){
                            PathIterator.next(); TotalSize += (double)(QFileInfo(PathIterator.filePath()).size()) / (double)1024;
                        }
                    }
                    break;
                }
            } else if (ProcessMode == 'R'){
                switch (ProcessOp){
                case 'S':
                    for (int ct2 = 1; ct2 <= 3; ct2++){
                        CurrentPath = Window->ui->BackupEntry->text() + QString::fromStdString("/SteamSaves/") +
                                      QString::fromStdString(DB::FetchGameInfo("SELECT SaveFolder" + std::to_string(ct2) + " FROM RegisteredGames"
                                      " WHERE SteamName = '" + Window->ui->GameList->item(ct)->text().toStdString() + "'"));
                        DB::ReplaceLabels(CurrentPath);
                        CurrentDir.setPath(CurrentPath); CurrentDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
                        if (CurrentDir.exists()){
                            QDirIterator PathIterator(CurrentDir, QDirIterator::Subdirectories);
                            while (PathIterator.hasNext()){
                                PathIterator.next(); TotalSize += (double)(QFileInfo(PathIterator.filePath()).size()) / (double)1024;
                            }
                        }
                    }
                    break;
                case 'C':
                    for (int ct2 = 1; ct2 <= 3; ct2++){
                        // Find the respective path, replacing its labels
                        CurrentPath = Window->ui->BackupEntry->text() + QString::fromStdString("/SteamConfigs/") +
                                      QString::fromStdString(DB::FetchGameInfo("SELECT ConfigFolder" + std::to_string(ct2) + " FROM RegisteredGames"
                                      " WHERE SteamName = '" + Window->ui->GameList->item(ct)->text().toStdString() + "'"));
                        DB::ReplaceLabels(CurrentPath);
                        CurrentDir.setPath(CurrentPath); CurrentDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
                        if (CurrentDir.exists()){
                            QDirIterator PathIterator(CurrentDir, QDirIterator::Subdirectories);
                            while (PathIterator.hasNext()){
                                PathIterator.next(); TotalSize += (double)(QFileInfo(PathIterator.filePath()).size()) / (double)1024;
                            }
                        }
                    }
                    break;
                case 'G':
                    CurrentPath = Window->ui->BackupEntry->text() + QString::fromStdString("/SteamGames/") +
                                  QString::fromStdString(DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames WHERE SteamName = '" +
                                  Window->ui->GameList->item(ct)->text().toStdString() + "'"));
                    DB::ReplaceLabels(CurrentPath);
                    CurrentDir.setPath(CurrentPath); CurrentDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
                    if (CurrentDir.exists()){
                        QDirIterator PathIterator(CurrentDir, QDirIterator::Subdirectories);
                        while (PathIterator.hasNext()){
                            PathIterator.next(); TotalSize += (double)(QFileInfo(PathIterator.filePath()).size()) / (double)1024;
                        }
                    }
                    break;
                }
            }
        }
    }

    // Display total value as KB by default
    Window->ui->TotalSizeLabel->setText("Total size:\n" + QString::number(TotalSize, 'f', 2) + " KB");
    // If it exceeds 1024KB, convert to MB
    if (TotalSize > 1024){
        TotalSize = TotalSize / (double)1024;
        Window->ui->TotalSizeLabel->setText("Total size:\n" + QString::number(TotalSize, 'f', 2) + " MB");
    }
    // If it exceeds 1024MB, convert to GB (stops here)
    if (TotalSize > 1024){
        TotalSize = TotalSize / (double)1024;
        Window->ui->TotalSizeLabel->setText("Total size:\n" + QString::number(TotalSize, 'f', 2) + " GB");
    }
}

// Checks if there's enough space on target path before starting the process
bool MainMenu::CheckDiskSpace(){
    double TotalBytes = 0.00; bool HasSpace = false;

    // Converting total size to bytes for comparison
    if (Window->ui->TotalSizeLabel->text().contains("GB")){ TotalBytes = TotalSize * (1024 * 1024 * 1024); }
    else if (Window->ui->TotalSizeLabel->text().contains("MB")){ TotalBytes = TotalSize * (1024 * 1024); }
    else if (Window->ui->TotalSizeLabel->text().contains("KB")){ TotalBytes = TotalSize * 1024; }

    // Checking if free space in target path is bigger than the total of bytes needed for process
    if (ProcessMode == 'B'){
        if (QStorageInfo(Window->ui->BackupEntry->text()).bytesFree() > TotalBytes){ HasSpace = true; } else { HasSpace = false; }
    } else if (ProcessMode == 'R'){
        if (QStorageInfo::root().bytesFree() > TotalBytes){ HasSpace = true; } else { HasSpace = false; }
    }
    return HasSpace;
}

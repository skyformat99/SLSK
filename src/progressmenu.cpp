/*
=========================================================================
---[SLSK - Progress Menu functionalities]---
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
#include <iostream>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QProcess>
#include <QtDebug>
#include <QTime>
#include <QThread>
#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

// Program-specific/external libs
#include "mainmenu.h"
#include "progressmenu.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "db.h"

//-----------------------------------------------------------------------------------------------------------
// VARIABLES
//-----------------------------------------------------------------------------------------------------------

// Integers for the total number of games/files and the number of games/files that were successfully copied
int TotalGames = 0, GamesDone = 0, TotalFiles = 0, FilesDone = 0;

// Booleans for Cancel button and to stop the process
bool ProgressMenu::CancelCheck = false, ProgressMenu::StopProcess = false;

QDir BackupFolder;  // Variable for backup folder directory

// Strings for paths (from database)
std::string CurrentSave[3] = {"", "", ""}, CurrentConfig[3] = {"", "", ""}, CurrentGame = "", CurrentAppID = "";

// Strings that help compose other paths
std::string ProgressMenu::CurrentFolder = "", AppManifest = "", CurrentSubFolder = "",
            CurrentBackupPath = "", CurrentRestorePath = "", CurrentPathFolder = "";

//-----------------------------------------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------------------------------------

// Slot function for starting thread
void ProgressThread::Start() { ProgressMenu::StartProcess(MainMenu::ProcessMode, MainMenu::ProcessOp, Window->ui->GameList); }

/* Creates subfolders in backup folder in case they don't exist - "SteamSaves" for saves, "SteamConfigs" for
   configs, "SteamGames" and "SteamManifests" for games */
void ProgressMenu::SetBackupSubFolders(char op){
    if (op == 'S'){ BackupFolder.mkpath("SteamSaves"); }
    else if (op == 'C') { BackupFolder.mkpath("SteamConfigs"); }
    else if (op == 'G'){ BackupFolder.mkpath("SteamGames"); BackupFolder.mkpath("SteamManifests"); }
}

// Writes the backup date in a text file
void ProgressMenu::WriteBackupDate(char op){
    // Setting file name according to option
    QString filename;
    switch (op){
        case 'S': filename = QString::fromStdString(MainMenu::BackupFolder + "/LastSaveBackup.txt"); break;
        case 'C': filename = QString::fromStdString(MainMenu::BackupFolder + "/LastConfigBackup.txt"); break;
        case 'G': filename = QString::fromStdString(MainMenu::BackupFolder + "/LastGameBackup.txt"); break;
    }

    // Fetching current date and time
    int year = QDate::currentDate().year(), month = QDate::currentDate().month(), day = QDate::currentDate().day(),
        hour = QTime::currentTime().hour(), minute = QTime::currentTime().minute(), second = QTime::currentTime().second();

    // Formatting current date and time
    QString date = QString::number(year) + "-";
    if (month < 10){ date += "0"; } date += QString::number(month) + "-";
    if (day < 10){ date += "0"; } date += QString::number(day) + " ";
    QString time;
    if (hour < 10){ time += "0"; } time += QString::number(hour) + ":";
    if (minute < 10){ time += "0"; } time += QString::number(minute) + ":";
    if (second < 10){ time += "0"; } time += QString::number(second);

    // Check if the file already exists - if yes, remove it and create a new blank one
    if (QFile(filename).exists()){ QFile::remove(filename); }
    QFile file(filename);

    // Opening the file and writing content
    file.open(QIODevice::ReadWrite);
    QTextStream stream(&file); stream << date << endl << time << endl;
    file.close();
}


// Function that fetches all raw paths (and other info) from a game in the database for the backup/restore process
void ProgressMenu::FetchPaths(QString Game){
    // Fetch game folder
    CurrentFolder = DB::FetchGameInfo("SELECT GameFolder FROM RegisteredGames WHERE SteamName = \"" + Game.toStdString() + "\"");
    CurrentGame = DB::SteamPath + "/" + CurrentFolder;

    // Fetch appmanifest (erases "common" from Steam path and appends the appmanifest file)
    CurrentAppID = DB::SteamPath; CurrentAppID.erase(CurrentAppID.end() - 6, CurrentAppID.end());
    AppManifest = "appmanifest_" + DB::FetchGameInfo("SELECT AppID FROM RegisteredGames WHERE SteamName = \"" + Game.toStdString() + "\"") + ".acf";
    CurrentAppID += AppManifest;

    // Fetch save and config paths (all 3 possible paths)
    for (int ct = 0; ct < 3; ct++){
        CurrentSave[ct] = DB::FetchGameInfo("SELECT SavePath" + std::to_string(ct + 1) + " FROM RegisteredGames WHERE SteamName = \"" + Game.toStdString() + "\"");
        CurrentConfig[ct] = DB::FetchGameInfo("SELECT ConfigPath" + std::to_string(ct + 1) + " FROM RegisteredGames WHERE SteamName = \"" + Game.toStdString() + "\"");
    }
}

// Functions that actually copies files over from the respective directories based on the following:
// Mode - 'B'ackup/'R'estore
// Option - 'S'ave/'C'onfig/'G'ame (appmanifest is copied separately from outside)
// NOTES: path = origin path

// For Backup Mode
void ProgressMenu::Backup(char op, std::string path, int n){
    // Setting paths for iterator based on option
    switch (op){
    case 'S': CurrentSubFolder = MainMenu::BackupFolder + "/SteamSaves/";
              CurrentPathFolder = DB::FetchGameInfo("SELECT SaveFolder" + std::to_string(n) + " FROM RegisteredGames "
                                  "WHERE SavePath" + std::to_string(n) + " = '" + path + "'"); break;
    case 'C': CurrentSubFolder = MainMenu::BackupFolder + "/SteamConfigs/";
              CurrentPathFolder = DB::FetchGameInfo("SELECT ConfigFolder" + std::to_string(n) + " FROM RegisteredGames "
                                  "WHERE ConfigPath" + std::to_string(n) + " = '" + path + "'"); break;
    case 'G': CurrentSubFolder = MainMenu::BackupFolder + "/SteamGames/";
              CurrentPathFolder = CurrentFolder; break;
    }

    // Replace any labels in the path and create it
    DB::ReplaceLabels(path);
    QDir PathDir(QString::fromStdString(path)); PathDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    if (PathDir.dirName().contains('*')){                                                                   // If dir name has a wildcard:
        if (PathDir.dirName().contains('.')){                                                               // If dir name contains a '.', it's a file (or multiple files)
            PathDir.setNameFilters(QStringList() << "*." + QFileInfo(PathDir.dirName()).completeSuffix());  // In this case, filter by file extension...
            PathDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);                                          // ...and by files only...
            PathDir.cdUp();                                                                                 // ...and move up to parent directory
        } else {                                                                                            // If it's not a file, it's multiple dirs
            PathDir.setNameFilters(QStringList() << PathDir.dirName());                                     // In this case, filter by directory name...
            PathDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);                                           // ...and by dirs only...
            PathDir.cdUp(); CurrentPathFolder = PathDir.dirName().toStdString();                            // ...move up to parent directory and set path folder correctly
        }
    }

    // Initialize the iterator, and while there are files to be copied and the process is permitted to continue:
    QDirIterator BackupIterator(PathDir, QDirIterator::Subdirectories);
    bool FilesCounted = false;  // Bool for signaling if all files have been counted already
    while (BackupIterator.hasNext() && ProgressMenu::StopProcess == false){
        BackupIterator.next();                                                                                              // Iterate to the next one
        if (QFileInfo(BackupIterator.filePath()).isDir()){                                                                  // If next entry is a dir:
            QDir SubfolderDir(BackupIterator.filePath()); SubfolderDir.setFilter(QDir:: Files | QDir::NoDotAndDotDot);      // Fetch the respective dir
            if (FilesCounted == false){                                                                                     // If files haven't been counted yet:
                QDirIterator SubfolderCounter(SubfolderDir, QDirIterator::Subdirectories);                                  // Create another iterator for counting files
                while (SubfolderCounter.hasNext()){ TotalFiles++; SubfolderCounter.next(); }                                // Count all files that will be backed up
                FilesCounted = true;                                                                                        // Update flag
            }

            // Create one more iterator for the dir and do the process with it instead
            QDirIterator SubfolderIterator(SubfolderDir, QDirIterator::Subdirectories);
            while (SubfolderIterator.hasNext() && ProgressMenu::StopProcess == false){
                SubfolderIterator.next();                                                                   // Go to the next file
                emit MainWindow::CopyThread->UpdateCopyingLabel("Copying file: " + SubfolderIterator.fileName());   // Emit a signal to update the current file label accordingly

                // Create the path for current content
                CurrentBackupPath = CurrentSubFolder + CurrentPathFolder +
                                    SubfolderIterator.filePath().remove(BackupIterator.path()).remove(
                                    SubfolderIterator.fileName()).toStdString();
                BackupFolder.mkpath(QString::fromStdString(CurrentBackupPath));

                // Copy the content, deleting old copies beforehand if they already exist
                if (QFile::exists(QString::fromStdString(CurrentBackupPath) + SubfolderIterator.fileName()))
                    QFile::remove(QString::fromStdString(CurrentBackupPath) + SubfolderIterator.fileName());
                QFile::copy(SubfolderIterator.filePath(), QString::fromStdString(CurrentBackupPath) + SubfolderIterator.fileName());

                // Update current progress
                FilesDone++;
                emit MainWindow::CopyThread->UpdateCurrentProgress((FilesDone/(double)TotalFiles) * 100);
            }
        } else {                                                                                        // If next entry is not a dir:
            if (FilesCounted == false){                                                                 // If files haven't been counted yet:
                QDirIterator BackupCounter(PathDir, QDirIterator::Subdirectories);                      // Create another iterator for counting files
                while (BackupCounter.hasNext()){ TotalFiles++; BackupCounter.next(); }                  // Count all files that will be backed up
                FilesCounted = true;                                                                    // Update flag
            }
            emit MainWindow::CopyThread->UpdateCopyingLabel("Copying file: " + BackupIterator.fileName());   // Emit a signal to update the current file label accordingly

            // Create the path for current content
            CurrentBackupPath = CurrentSubFolder + CurrentPathFolder +
                                BackupIterator.filePath().remove(PathDir.path()).remove(
                                BackupIterator.fileName()).toStdString();
            BackupFolder.mkpath(QString::fromStdString(CurrentBackupPath));

            // Copy the content, deleting old copies beforehand if they already exist
            if (QFile::exists(QString::fromStdString(CurrentBackupPath) + BackupIterator.fileName()))
                QFile::remove(QString::fromStdString(CurrentBackupPath) + BackupIterator.fileName());
            QFile::copy(BackupIterator.filePath(), QString::fromStdString(CurrentBackupPath) + BackupIterator.fileName());

            // Update current progress
            FilesDone++;
            emit MainWindow::CopyThread->UpdateCurrentProgress((FilesDone/(double)TotalFiles) * 100);
        }
    }

    // Reset counters at the end
    TotalFiles = 0; FilesDone = 0;
}

// For Restore Mode
void ProgressMenu::Restore(char op, std::string path, int n){
    // Setting paths for iterator based on option
    switch (op){
    case 'S': CurrentSubFolder = MainMenu::BackupFolder + "/SteamSaves/";
              CurrentPathFolder = DB::FetchGameInfo("SELECT SaveFolder" + std::to_string(n) + " FROM RegisteredGames "
                                  "WHERE SavePath" + std::to_string(n) + " = '" + path + "'"); break;
    case 'C': CurrentSubFolder = MainMenu::BackupFolder + "/SteamConfigs/";
              CurrentPathFolder = DB::FetchGameInfo("SELECT ConfigFolder" + std::to_string(n) + " FROM RegisteredGames "
                            "WHERE ConfigPath" + std::to_string(n) + " = '" + path + "'"); break;
    case 'G': CurrentSubFolder = MainMenu::BackupFolder + "/SteamGames/";
              CurrentPathFolder = CurrentFolder; break;
    }
    CurrentRestorePath = CurrentSubFolder + CurrentPathFolder;

    // Replace any labels in the path and create it
    DB::ReplaceLabels(path);
    QDir PathDir(QString::fromStdString(CurrentRestorePath)); PathDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    if (PathDir.dirName().contains('*')){                                                                   // If dir name has a wildcard:
        if (PathDir.dirName().contains('.')){                                                               // If dir name contains a '.', it's a file (or multiple files)
            PathDir.setNameFilters(QStringList() << "*." + QFileInfo(PathDir.dirName()).completeSuffix());  // In this case, filter by file extension...
            PathDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);                                          // ...and by files only...
            PathDir.cdUp();                                                                                 // ...and move up to parent directory
        } else {                                                                                            // If it's not a file, it's multiple dirs
            PathDir.setNameFilters(QStringList() << PathDir.dirName());                                     // In this case, filter by directory name...
            PathDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);                                           // ...and by dirs only...
            PathDir.cdUp(); CurrentPathFolder = PathDir.dirName().toStdString();                            // ...move up to parent directory and set path folder correctly
        }
    }

    // Remove the last dir from the path if it has a wildcard
    if (QDir(QString::fromStdString(path)).dirName().contains('*'))
        path = QDir(QString::fromStdString(path)).path().remove(QDir(QString::fromStdString(path)).dirName()).toStdString();

    // Initialize the iterator, and while there are files to be copied and the process is permitted to continue:
    QDirIterator RestoreIterator(PathDir, QDirIterator::Subdirectories);
    bool FilesCounted = false;  // Bool for signaling if all files have been counted already
    while (RestoreIterator.hasNext() && ProgressMenu::StopProcess == false){
        RestoreIterator.next();                                                                                             // Iterate to the next one
        if (QFileInfo(RestoreIterator.filePath()).isDir()){                                                                 // If next entry is a dir:
            QDir SubfolderDir(RestoreIterator.filePath()); SubfolderDir.setFilter(QDir:: Files | QDir::NoDotAndDotDot);     // Fetch the respective dir
            if (FilesCounted == false){                                                                                     // If files haven't been counted yet:
                QDirIterator SubfolderCounter(SubfolderDir, QDirIterator::Subdirectories);                                  // Create another iterator for counting files
                while (SubfolderCounter.hasNext()){ TotalFiles++; SubfolderCounter.next(); }                                // Count all files that will be backed up
                FilesCounted = true;                                                                                        // Update flag
            }

            // Create one more iterator for the dir and do the process with it instead
            QDirIterator SubfolderIterator(SubfolderDir, QDirIterator::Subdirectories);
            while (SubfolderIterator.hasNext() && ProgressMenu::StopProcess == false){
                SubfolderIterator.next();                                                                   // Go to the next file
                emit MainWindow::CopyThread->UpdateCopyingLabel("Copying file: " + SubfolderIterator.fileName());   // Emit a signal to update the current file label accordingly

                // Create the path for current content if necessary...
                QString DestPath = QString::fromStdString(path).remove(QDir(QString::fromStdString(path)).dirName() + "/") +
                                   SubfolderIterator.filePath().remove(QString::fromStdString(CurrentSubFolder)).remove(SubfolderIterator.fileName());
                QDir(QString::fromStdString(path)).mkpath(DestPath);

                // ...copy the content...
                QFile::copy(SubfolderIterator.filePath(), QDir(DestPath).path() + "/" + SubfolderIterator.fileName());

                // ...and update current progress
                FilesDone++;
                emit MainWindow::CopyThread->UpdateCurrentProgress((FilesDone/(double)TotalFiles) * 100);
            }
        } else {                                                                                            // If next entry is not a dir:
            if (FilesCounted == false){                                                                     // If files haven't been counted yet:
                QDirIterator RestoreCounter(PathDir, QDirIterator::Subdirectories);                         // Create another iterator for counting files
                while (RestoreCounter.hasNext()){ TotalFiles++; RestoreCounter.next(); }                    // Count all files that will be backed up
                FilesCounted = true;                                                                        // Update flag
            }

            emit MainWindow::CopyThread->UpdateCopyingLabel("Copying file: " + RestoreIterator.fileName());         // Emit a signal to update the current file label accordingly

            // Create the path for current content if necessary...
            QString DestPath = QString::fromStdString(path + RestoreIterator.filePath().remove(
                               QString::fromStdString(CurrentRestorePath)).remove(RestoreIterator.fileName()).toStdString());
            QDir(QString::fromStdString(path)).mkpath(DestPath);

            // ...copy the content...
            QFile::copy(RestoreIterator.filePath(), QDir(DestPath).path() + "/" + RestoreIterator.fileName());

            // ...and update current progress
            FilesDone++;
            emit MainWindow::CopyThread->UpdateCurrentProgress((FilesDone/(double)TotalFiles) * 100);
        }
    }

    // Reset counters at the end
    TotalFiles = 0; FilesDone = 0;
}

// Starts the proper backup/restore process
void ProgressMenu::StartProcess(char mode, char op, QListWidget* gamelist){
    ProgressMenu::StopProcess = false;                                      // Set the stop process bool to false...
    TotalGames = gamelist->count();                                         // ...count the total number of games...
    emit MainWindow::CopyThread->StartProgressBars(TotalGames);                     // ...emit a signal to set the progress bars...
    BackupFolder.setPath(QString::fromStdString(MainMenu::BackupFolder));   // ...set the backup folder's path...
    BackupFolder.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);              // ...along with its filters...
    if (mode == 'B'){ ProgressMenu::SetBackupSubFolders(op); }              // ...set its subfolders if on Backup Mode...
    emit MainWindow::CopyThread->UpdateHeaderLabel(mode);                           // ...and emit a signal to set header label accordingly

    // Creating a list with each game
    QStringList AllGames;
    for (int ct = 0; ct < TotalGames; ct++){ AllGames << gamelist->item(ct)->text(); }

    // For each game in the list, if the process is permitted to continue...
    foreach (QString Game, AllGames){
        if (ProgressMenu::StopProcess == false){
            emit MainWindow::CopyThread->UpdateGameLabel("Current game: " + Game);  // ...emit a signal to update the current game label...
            ProgressMenu::FetchPaths(Game);                                 // ...fetch all paths related to that game...

            // ...do the process:
            // For Backup
            if (mode == 'B'){
                // Saves
                if (op == 'S'){
                    // Backup each save folder (if they exist)
                    for (int ct = 0; ct < 3; ct++){
                        if (CurrentSave[ct] != "[N/A]" && CurrentSave[ct] != "[UNKNOWN]" &&
                        CurrentSave[ct] != "[CLOUD-ONLY]"){
                            ProgressMenu::Backup('S', CurrentSave[ct], ct + 1);
                        }
                    }

                // Configs
                } else if (op == 'C'){
                    // Backup each config folder (if they exist)
                    for (int ct = 0; ct < 3; ct++){
                        if (CurrentConfig[ct] != "[N/A]" && CurrentConfig[ct] != "[UNKNOWN]" &&
                        CurrentConfig[ct] != "[CLOUD-ONLY]"){
                            ProgressMenu::Backup('C', CurrentConfig[ct], ct + 1);
                        }
                    }

                // Games
                } else if (op == 'G'){
                    // Backup the game folder and its manifest
                    ProgressMenu::Backup('G', CurrentGame, 0);
                    QFile::copy(QString::fromStdString(CurrentAppID),
                                QString::fromStdString(MainMenu::BackupFolder + "/SteamManifests/" + AppManifest));
                }

            // For Restore
            } else if (mode == 'R'){
                // Saves
                if (op == 'S'){
                    // Restore each save folder (if they exist)
                    for (int ct = 0; ct < 3; ct++){
                        if (CurrentSave[ct] != "[N/A]" && CurrentSave[ct] != "[UNKNOWN]" &&
                        CurrentSave[ct] != "[CLOUD-ONLY]"){
                            ProgressMenu::Restore('S', CurrentSave[ct], ct + 1);
                        }
                    }

                // Configs
                } else if (op == 'C') {
                    // Restore each config folder (if they exist)
                    for (int ct = 0; ct < 3; ct++){
                        if (CurrentConfig[ct] != "[N/A]" && CurrentConfig[ct] != "[UNKNOWN]" &&
                        CurrentConfig[ct] != "[CLOUD-ONLY]"){
                            ProgressMenu::Restore('C', CurrentConfig[ct], ct + 1);
                        }
                    }

                // Games
                } else if (op == 'G'){
                    // Restore the game folder and its manifest
                    ProgressMenu::Restore('G', CurrentGame, 0);
                    QFile::copy(QString::fromStdString(MainMenu::BackupFolder + "/SteamManifests/" + AppManifest),
                                QString::fromStdString(CurrentAppID));
                }
            }

            // ...and update progress bars accordingly
            GamesDone++;
            emit MainWindow::CopyThread->UpdateTotalProgress(GamesDone);
        }
    }

    TotalGames = 0; GamesDone = 0;      // Reset counters

    // If process can continue (meaning it hasn't force stopped):
    if (ProgressMenu::StopProcess == false){
        ProgressMenu::WriteBackupDate(op);                                                                      // Write the current date and time on the database...
        emit MainWindow::CopyThread->UpdateResultsScreen(mode, op, QString::fromStdString(CurrentSubFolder));   // ...and emit a signal to bring the results screen
    }
}

// Delay for CancelCountdown, adapted from kshark27 on stackoverflow - https://stackoverflow.com/a/11487434
void ProgressMenu::Delay(int sec){
    QTime dieTime = QTime::currentTime().addSecs(sec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

// Countdown for foolproof Cancel button
void ProgressMenu::CancelCountdown(){
    int ct = 3; QString digit;  // Setting time limit for cooldown (3 seconds)

    // Start the countdown
    while (ct > 0 && ProgressMenu::CancelCheck == true){
        digit = QString::fromStdString(std::to_string(ct));                                                                             // Fetch new digit for label
        Window->ui->CancelWarningLabel->setText("Are you sure? Click again to confirm.\nIgnoring cancel request in " + digit + "...");  // Update label
        Delay(1); ct--;                                                                                                                 // Activate 1 second delay
    }

    // When countdown ends, resume process
    Window->ui->CancelWarningLabel->setText("");
    ProgressMenu::CancelCheck = false;
}

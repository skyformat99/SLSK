/*
=========================================================================
---[SLSK - Main Window/UI]---
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
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QThread>
#include <QObject>

// Program-specific libs
#include "mainmenu.h"
#include "progressmenu.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//-----------------------------------------------------------------------------------------------------------
// CONSTRUCTOR / DECONSTRUCTOR / OBJECTS / REIMPLEMENTATIONS
//-----------------------------------------------------------------------------------------------------------

// Pointer for non-GUI thread
ProgressThread *MainWindow::CopyThread = new ProgressThread();

// Reimplementation of QMainWindow::closeEvent, so it stops the backup/restore process if it's running
void MainWindow::closeEvent(QCloseEvent *){ ProgressMenu::StopProcess = true; }

// Constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);  // Create the UI

    // Set the main menu as visible
    ui->MainWidget->setVisible(true); ui->ProgressWidget->setVisible(false); ui->ResultsWidget->setVisible(false);
    ui->ScanningLabel->setVisible(false); ui->TotalSizeLabel->setText("");

    // Set signal/slot connections for thread
    QObject::connect(CopyThread, SIGNAL(StartProgressBars(int)), SLOT(StartProgressBars(int)));                                             // Connection for setting progress bars
    QObject::connect(CopyThread, SIGNAL(UpdateCopyingLabel(QString)), SLOT(UpdateCopyingLabel(QString)));                                   // Connection for updating copying label
    QObject::connect(CopyThread, SIGNAL(UpdateCurrentProgress(int)), SLOT(UpdateCurrentProgress(int)));                                     // Connection for updating current progress
    QObject::connect(CopyThread, SIGNAL(UpdateHeaderLabel(char)), SLOT(UpdateHeaderLabel(char)));                                           // Connection for updating header label
    QObject::connect(CopyThread, SIGNAL(UpdateGameLabel(QString)), SLOT(UpdateGameLabel(QString)));                                         // Connection for updating current game label
    QObject::connect(CopyThread, SIGNAL(UpdateTotalProgress(int)), SLOT(UpdateTotalProgress(int)));                                         // Connection for updating total progress
    QObject::connect(CopyThread, SIGNAL(UpdateResultsScreen(char, char, QString)), SLOT(UpdateResultsScreen(char, char, QString)));         // Connection for bringing results screen

    // Set signal/slot connections for GUI
    QObject::connect(ui->GameList, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(UpdateTotalSize()));    // Connection for calculating total size
}

// Deconstructor
MainWindow::~MainWindow(){ delete ui; ProgressMenu::StopProcess = true; }

//-----------------------------------------------------------------------------------------------------------
// GUI SLOTS
//-----------------------------------------------------------------------------------------------------------

// Initializes the progress bars
void MainWindow::StartProgressBars(int TotalGames){
    Window->ui->CurrentProgress->setRange(0, 100); Window->ui->CurrentProgress->setValue(0);    // Set current progress to 0
    Window->ui->TotalProgress->setRange(0, TotalGames); Window->ui->TotalProgress->setValue(0); // Set total progress to 0
}

// Updates the current file label
void MainWindow::UpdateCopyingLabel(QString text){ Window->ui->CurrentFileLabel->setText(text); }

// Updates the current progress bar
void MainWindow::UpdateCurrentProgress(int value){ Window->ui->CurrentProgress->setValue(value); }

// Sets the header label on progress menu
void MainWindow::UpdateHeaderLabel(char mode){
    if (mode == 'B'){ Window->ui->ProgressHeaderLabel->setText("Backup in progress... sit back and enjoy the ride."); }
    else if (mode == 'R'){ Window->ui->ProgressHeaderLabel->setText("Restore in progress... sit back and enjoy the ride."); }
}

// Updates the current game label
void MainWindow::UpdateGameLabel(QString text){ Window->ui->CurrentGameLabel->setText(text); }

// Updates the total progress bar
void MainWindow::UpdateTotalProgress(int value){
    Window->ui->TotalProgress->setValue(value);
    Window->ui->CurrentProgress->setValue(0);
}

// Updates and brings the results screen when process is done
void MainWindow::UpdateResultsScreen(char mode, char op, QString CurrentSubFolder){
    // Set header label accordingly...
    if (mode == 'B'){
        Window->ui->ResultsHeaderLabel->setText("Backup complete!");
        switch (op){
        case 'S': Window->ui->ResultsPathLabel->setText(QString::fromStdString("Your saves are in:\n") + CurrentSubFolder); break;
        case 'C': Window->ui->ResultsPathLabel->setText(QString::fromStdString("Your configs are in:\n") + CurrentSubFolder); break;
        case 'G': Window->ui->ResultsPathLabel->setText(QString::fromStdString("Your games are in:\n") + CurrentSubFolder); break;
        }
    } else if (mode == 'R'){
        Window->ui->ResultsHeaderLabel->setText("Restore complete!");
        switch (op){
        case 'S': Window->ui->ResultsPathLabel->setText(QString::fromStdString("Saves successfully restored.")); break;
        case 'C': Window->ui->ResultsPathLabel->setText(QString::fromStdString("Configs successfully restored.")); break;
        case 'G': Window->ui->ResultsPathLabel->setText(QString::fromStdString("Games successfully restored.")); break;
        }
    }

    // ...and bring it up
    Window->ui->ProgressWidget->setVisible(false);
    Window->ui->ResultsWidget->setVisible(true);
}

// Updates total size label
void MainWindow::UpdateTotalSize(){
    if (MainMenu::OneItemChecked()){                                                // If at least one item has been checked:
        Window->ui->TotalSizeLabel->setText("Calculating\nsize...");                // Update size label
        Window->ui->StartBtn->setDisabled(true);                                    // Disable the Start button for calculation
        App->processEvents();                                                       // Let the UI settle in
        MainMenu::CheckTotalSize();                                                 // Check total size of operation
        Window->ui->StartBtn->setDisabled(false);                                   // Enable the Start button again
    } else {                                                                        // If no item has been checked:
        Window->ui->TotalSizeLabel->setText("");                                    // "Hide" the size label
        Window->ui->StartBtn->setDisabled(true);                                    // Disable the Start button
    }
}

//-----------------------------------------------------------------------------------------------------------
// FUNCTION CALLS - MAIN MENU
//-----------------------------------------------------------------------------------------------------------

/* For every Scan button - each one will switch mode and option to its liking, scan the respective folder, organize the
   game list and lock/unlock UI buttons accordingly */
void MainWindow::on_BackupScanSaveBtn_clicked(){        // Backup Saves
    MainMenu::SwitchMode('B', 'S');
    Window->ui->StartBtn->setDisabled(true);
    MainMenu::BackupScan();
    Window->ui->StartBtn->setDisabled(false);
    MainMenu::OrganizeGameList(); MainMenu::LockUnlock();
}

void MainWindow::on_BackupScanConfigBtn_clicked(){      // Backup Configs
    MainMenu::SwitchMode('B', 'C');
    Window->ui->StartBtn->setDisabled(true);
    MainMenu::BackupScan();
    Window->ui->StartBtn->setDisabled(false);
    MainMenu::OrganizeGameList(); MainMenu::LockUnlock();
}

void MainWindow::on_BackupScanGameBtn_clicked(){        // Backup Games
    MainMenu::SwitchMode('B', 'G');
    Window->ui->StartBtn->setDisabled(true);
    MainMenu::BackupScan();
    Window->ui->StartBtn->setDisabled(false);
    MainMenu::OrganizeGameList(); MainMenu::LockUnlock();
}

void MainWindow::on_RestoreScanSaveBtn_clicked(){       // Restore Saves
    MainMenu::SwitchMode('R', 'S');
    Window->ui->StartBtn->setDisabled(true);
    MainMenu::RestoreScan();
    Window->ui->StartBtn->setDisabled(false);
    MainMenu::OrganizeGameList(); MainMenu::LockUnlock();
}

void MainWindow::on_RestoreScanConfigBtn_clicked(){     // Restore Configs
    MainMenu::SwitchMode('R', 'C');
    Window->ui->StartBtn->setDisabled(true);
    MainMenu::RestoreScan();
    Window->ui->StartBtn->setDisabled(false);
    MainMenu::OrganizeGameList(); MainMenu::LockUnlock();
}

void MainWindow::on_RestoreScanGameBtn_clicked(){       // Restore Games
    MainMenu::SwitchMode('R', 'G');
    Window->ui->StartBtn->setDisabled(true);
    MainMenu::RestoreScan();
    Window->ui->StartBtn->setDisabled(false);
    MainMenu::OrganizeGameList(); MainMenu::LockUnlock();
}

// For Mark/Unmark All buttons - check/uncheck all items in the list and lock/unlock UI buttons accordingly
void MainWindow::on_MarkAllBtn_clicked(){ MainMenu::CheckUncheck(true); MainMenu::LockUnlock(); }
void MainWindow::on_UnmarkAllBtn_clicked(){ MainMenu::CheckUncheck(false); MainMenu::LockUnlock(); }

// For Change Folder button - open a file dialog for choosing a backup folder, locking/unlocking UI buttons accordingly
void MainWindow::on_ChangeFolderBtn_clicked(){
    QString NewFolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (NewFolder != ""){ ui->BackupEntry->setText(NewFolder); }
    MainMenu::LockUnlock();
}

// For Start button - start the backup/restore process (it will only be enabled when user has set a backup folder AND checked at least ONE game)
void MainWindow::on_StartBtn_clicked(){                                              
    MainMenu::BackupFolder = ui->BackupEntry->text().toStdString();             // Set backup folder to static variable
    if (MainMenu::CheckDiskSpace() == true){                                    // If target path has enough space:
        MainMenu::RemoveUnchecked();                                            // Remove unchecked items from list
        ui->MainWidget->setVisible(false);                                      // Hide the main menu
        ui->ProgressWidget->setVisible(true);                                   // Show the progress menu
        App->processEvents();                                                   // Let the UI settle in
        QThread *thread = new QThread();                                        // Create a new thread
        CopyThread->moveToThread(thread);                                       // Move backup/restore process pointer to new thread
        QObject::connect(thread, SIGNAL(started()), CopyThread, SLOT(Start())); // Connect respective signal and slot
        thread->start();                                                        // Start the thread
    } else {                                                                    // If there's not enough space, display an error message
        QMessageBox err;
        err.critical(0, "Error", "Not enough space on device");
        err.setFixedSize(500,200);
    }
}

//-----------------------------------------------------------------------------------------------------------
// FUNCTION CALLS - PROGRESS MENU
//-----------------------------------------------------------------------------------------------------------

// For Cancel button - start the countdown/cancel the process
void MainWindow::on_CancelBtn_clicked(){
    // Cancel button is foolproof - so it starts a countdown first in case of an accidental click instead of cancelling
    if (!ProgressMenu::CancelCheck){ ProgressMenu::CancelCheck = true; ProgressMenu::CancelCountdown(); }

    // The button needs to be clicked again to confirm the cancelling
    else {
        ProgressMenu::CancelCheck = false; ProgressMenu::StopProcess = true;
        Window->ui->ProgressWidget->setVisible(false);
        Window->ui->MainWidget->setVisible(true);
        MainMenu::LockUnlock();
    }
}

//-----------------------------------------------------------------------------------------------------------
// FUNCTION CALLS - RESULTS MENU
//-----------------------------------------------------------------------------------------------------------

// For Back to menu button - hide this menu and bring up the main menu
void MainWindow::on_MenuBtn_clicked(){
    Window->ui->ResultsWidget->setVisible(false);
    Window->ui->MainWidget->setVisible(true);
    MainMenu::LockUnlock();
}

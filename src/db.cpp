/*
=========================================================================
---[SLSK - Database functionalities]---
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
#include <cstddef>
#include <iostream>
#include <QDir>
#include <QDate>
#include <QTime>
#include <QString>
#include <QStringList>
#include <QtDebug>
#include <stdio.h>
#include <stdlib.h>
#include <string>

// Program-specific/external libs
#include "db.h"
#include "mainmenu.h"
#include "progressmenu.h"
#include "sqlite3.h"

//-----------------------------------------------------------------------------------------------------------
// VARIABLES
//-----------------------------------------------------------------------------------------------------------

sqlite3 *conn;                                                                                                                  // Database connection handler
sqlite3_stmt *stmt;                                                                                                             // Database statement handler
QString UserFolder = QDir(QDir::homePath() + QString::fromStdString("/.local/share/Steam/userdata")).entryList(                 // Steam userdata folder (w/ user ID)
                     QDir::Dirs | QDir::NoDotAndDotDot).value(0);
std::string DBPath = "/opt/SLSK/SteamLinuxGames.db";                                                                            // Database raw path
std::string DB::SteamPath = QDir::homePath().toStdString() + "/.local/share/Steam/steamapps/common";                            // Steamapps raw path
std::string DB::UserPath = QDir::homePath().toStdString() + "/.local/share/Steam/userdata/" + UserFolder.toStdString();         // Steam userdata raw path

//-----------------------------------------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------------------------------------

// Checks if a game exists in the database
bool DB::CheckGameOnDB(QString foldername){
    // Variables
    bool game_exists;                                                                                                   // Flag for whether a game exists in the DB or not
    std::string query = "SELECT SteamName FROM RegisteredGames WHERE GameFolder = '" + foldername.toStdString() + "'";  // String for query

    sqlite3_open_v2(DBPath.c_str(), &conn, SQLITE_OPEN_READONLY, NULL); // Opening database
    sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, 0);              // Querying

    // Checking game - if the query returns a row, then obviously the game exists
    if (sqlite3_step(stmt) == SQLITE_ROW){ game_exists = true; }
    else { game_exists = false; }

    sqlite3_finalize(stmt); sqlite3_close_v2(conn); // Closing statement and database
    return game_exists;                             // Returning the flag
}

// Fetches a game's individual info in the database, returns "NONE" if info doesn't exist
std::string DB::FetchGameInfo(std::string query){
    // Variables
    std::string result; // String for result

    sqlite3_open_v2(DBPath.c_str(), &conn, SQLITE_OPEN_READONLY, NULL); // Opening database
    sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, 0);              // Querying

    // Fetches the info if the row exists, if it doesn't then we put "NONE" inside the result variable
    if (sqlite3_step(stmt) == SQLITE_ROW){ result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)); }
    else { result = "NONE"; }

    sqlite3_finalize(stmt); sqlite3_close_v2(conn); // Closing statement and database
    return result;                                  // Returning result
}

// Fetches a whole column from the database (ONLY USED FOR SAVE AND CONFIG OPTIONS)
QStringList DB::FetchAllInfo(char op, int n){
    QStringList List;   // Setting list
    std::string query;  // Setting query

    // Creating query according to option
    if (op == 'S'){ query = "SELECT SavePath" + std::to_string(n) + " FROM RegisteredGames"; }
    else if (op == 'C'){ query = "SELECT ConfigPath" + std::to_string(n) + " FROM RegisteredGames"; }

    sqlite3_open_v2(DBPath.c_str(), &conn, SQLITE_OPEN_READONLY, NULL); // Opening database
    sqlite3_prepare_v2(conn, query.c_str(), -1, &stmt, 0);              // Querying

    // Appending each entry individually in the result list
    while (sqlite3_step(stmt) == SQLITE_ROW){ List << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)); }

    sqlite3_finalize(stmt); sqlite3_close_v2(conn); // Closing statement and database
    return List;                                    // Returning list
}


// Replaces specific labels/aliases from the database - $STEAMAPPS, $STEAMUSER and ~ (home)
void DB::ReplaceLabels(std::string &string){
    /* Checking if the path has ANY of the special labels - "[N/A]", "[UNKNOWN]" or "[CLOUD-ONLY]" - if yes,
       rename it to "SKIP" so the program ignores it */
    if (string != "[N/A]" && string != "[UNKNOWN]" && string != "[CLOUD-ONLY]"){
        // If "$STEAMAPPS" is found, replace that with the Steam path for that game
        if (string.find("$STEAMAPPS") == 0){ string.replace(0, 10, DB::SteamPath + "/" + ProgressMenu::CurrentFolder); }

        // If "$STEAMUSER" is found, replace that with a fixed path in Steam that represents the user's ID
        else if (string.find("$STEAMUSER") == 0){ string.replace(0, 10, DB::UserPath); }

        // If the shortcut for the home folder ("~") is found, replace that with the absolute home path
        else if (string.find("~") == 0){ string.replace(0, 1, QDir::homePath().toStdString()); }
    } else {
        string = "SKIP";    // Replacing path with "SKIP"
    }
}

// Overload of ReplaceLabels for QStrings - see above
void DB::ReplaceLabels(QString &string){
    if (string != "[N/A]" && string != "[UNKNOWN]" && string != "[CLOUD-ONLY]"){
        if (string.contains("$STEAMAPPS")){ string.replace("$STEAMAPPS", QString::fromStdString(DB::SteamPath + "/" + ProgressMenu::CurrentFolder)); }
        else if (string.contains("$STEAMUSER")){ string.replace("$STEAMUSER", QString::fromStdString(DB::UserPath)); }
        else if (string.contains("~")){ string.replace("~", QDir::homePath()); }
    } else {
        string = "SKIP";
    }
}

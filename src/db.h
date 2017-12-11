/*
=========================================================================
---[SLSK - Database functionalities (header file)]---
Copyright (C) 2017 Supremist (aka supremesonicbrazil)
This file is part of Steam Linux Swiss Knife (or SLSK for short).
Steam Linux Swiss Knife is available under the GNU GPL v3.0 license. See the
accompanying COPYING file for more details.
=========================================================================
*/

#ifndef DB_H
#define DB_H

#include <cstddef>
#include <string>
#include <QString>
#include "sqlite3.h"

class DB {
    public:
        DB();
        static std::string SteamPath;
        static std::string UserPath;
        static bool CheckGameOnDB(QString foldername);
        static std::string FetchGameInfo(std::string query);
        static QStringList FetchAllInfo(char op, int n);
        static void ReplaceLabels(std::string &string);
        static void ReplaceLabels(QString &string);
};

#endif // DB_H

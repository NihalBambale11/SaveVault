#include "savecontroller.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QDirIterator>
#include <QSet>
#include <QStandardPaths>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>`

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>


extern bool addSaveEntry(QString gameName, QString originalPath, QString backupPath, QString notes);
extern bool performBackup(QString originalPath, QString backupPath);

SaveController::SaveController(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

// NEW: Cleans up the path from QML and stores it permanently
bool SaveController::registerCustomGame(QString gameName, QString rawPath) {
    if (gameName.isEmpty() || rawPath.isEmpty()) return false;

    // Convert QML file URL (file:///C:/...) into a normal C++ local file path
    QString cleanPath = QUrl(rawPath).toLocalFile();
    if (cleanPath.isEmpty()) {
        cleanPath = rawPath; // Fallback if it's already a clean string
    }

    // Generate a beautiful, isolated vault path on your E: drive
    QString backupPath = "E:/SaveVault/Backups/" + gameName + "_Vault";

    qDebug() << "Registering Dynamic Path for" << gameName << "->" << cleanPath;

    // Save it to our database table so the app remembers it forever
    return addSaveEntry(gameName, cleanPath, backupPath, "User Defined Custom Path");
}

// UPDATE: Loops through SQLite rows and executes backups dynamically
void SaveController::backupAllRegisteredGames() {
    qDebug() << "Executing Master Backup Cycle for all database entries...";

    QSqlQuery query("SELECT game_name, original_path, backup_path FROM Saves");
    int backupCount = 0;

    while (query.next()) {
        QString gameName = query.value(0).toString();
        QString originalPath = query.value(1).toString();
        QString backupPath = query.value(2).toString();

        if (performBackup(originalPath, backupPath)) {
            qDebug() << "SUCCESSFULLY BACKED UP:" << gameName;
            backupCount++;
        }
    }

    qDebug() << "Backup cycle finished." << backupCount << "games secured.";
}




// void SaveController::deepScanForSaves() {
//     qDebug() << "INITIATING GLOBAL MANIFEST SCAN...";

//     // 1. Open our Master Database File
//     QFile manifestFile("E:/SaveVault/master_games_list.json");

//     if (!manifestFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         qDebug() << "CRITICAL ERROR: Could not find master_games_list.json!";
//         return;
//     }

//     // 2. Parse the JSON Data
//     QByteArray jsonData = manifestFile.readAll();
//     manifestFile.close();

//     QJsonDocument document = QJsonDocument::fromJson(jsonData);
//     QJsonArray gamesArray = document.array();

//     // Setup our Windows environment variables
//     QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//     QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

//     int gamesDiscovered = 0;

//     // 3. Loop through the Manifest and verify if the game exists on this PC
//     for (int i = 0; i < gamesArray.size(); ++i) {
//         QJsonObject gameObj = gamesArray[i].toObject();
//         QString gameName = gameObj["name"].toString();
//         QString rawPath = gameObj["path"].toString();

//         // Translate the JSON variables into actual Windows paths
//         QString actualPath = rawPath;
//         actualPath.replace("%DOCUMENTS%", docsPath);
//         actualPath.replace("%USERPROFILE%", userPath);

//         // Ask Windows: Does this specific folder actually exist on this computer?
//         QDir checkDir(actualPath);
//         if (checkDir.exists()) {
//             qDebug() << "MANIFEST HIT: Found" << gameName << "at" << actualPath;

//             // Register it to our SQLite database
//             registerCustomGame(gameName, actualPath);
//             gamesDiscovered++;
//         }
//     }

//     qDebug() << "MANIFEST SCAN COMPLETE. Registered" << gamesDiscovered << "installed games.";
// }





void SaveController::deepScanForSaves() {
    qDebug() << "INITIATING HYBRID MASTER SCAN...";
    int gamesDiscovered = 0;

    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString roamingPath = homePath + "/AppData/Roaming";
    QString localPath = homePath + "/AppData/Local";
    QString localLowPath = homePath + "/AppData/LocalLow";
    QString publicPath = "C:/Users/Public/Documents";

    // ==========================================
    // PHASE 1: THE CRACKER HUB SCANNER (Fast)
    // ==========================================
    QStringList crackerHubs;
    crackerHubs << publicPath + "/Steam/RUNE" << publicPath + "/Steam/CODEX" << publicPath + "/Steam/TENOKE" << publicPath + "/OnlineFix";
    crackerHubs << roamingPath + "/Goldberg SteamEmu Saves" << roamingPath + "/Goldberg Uplay Emu Saves" << localPath + "/FLT";
    crackerHubs << homePath + "/Saved Games";

    for (const QString& hubPath : crackerHubs) {
        QDir hubDir(hubPath);
        if (!hubDir.exists()) continue;

        hubDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo& gameFolder : hubDir.entryInfoList()) {
            QString cleanGameName = "Cracked Game (" + gameFolder.fileName() + ")";
            if (registerCustomGame(cleanGameName, gameFolder.absoluteFilePath())) gamesDiscovered++;
        }
    }

    // ==========================================
    // PHASE 2: THE APPDATA CRAWLER (Deep & Filtered)
    // ==========================================
    qDebug() << "Launching Deep Crawler...";

    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    // We added documentsPath here so it scans your Documents folder again!
    QStringList deepScanRoots = {localPath, localLowPath, roamingPath, documentsPath};

    // THE BLACKLIST: Block your Java/CDAC files so the app doesn't back up your homework!
    QStringList blacklist = {"Microsoft", "Google", "Discord", "Spotify", "NVIDIA", "AMD",
                             "Code Cache", "BraveSoftware", "Adobe", "Intel",
                             ".metadata", "CDAC", "Eclipse", "workspace", "java"};

    // THE TARGETS: Added "PROFSAVE*" to catch EA/Battlefield games without extensions
    QStringList saveFilters = {"*.sav", "*.rpgsave", "save*.dat", "PROFSAVE*"};

    for (const QString& rootDir : deepScanRoots) {
        QDirIterator it(rootDir, saveFilters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

        while (it.hasNext()) {
            it.next();
            QString filePath = it.filePath();
            QString folderPath = it.fileInfo().absolutePath();

            // Check if the file is hiding inside a blacklisted folder (like your CDAC folders)
            bool isBlacklisted = false;
            for (const QString& badFolder : blacklist) {
                if (filePath.contains("/" + badFolder + "/", Qt::CaseInsensitive)) {
                    isBlacklisted = true;
                    break;
                }
            }

            // If it's a real game save, figure out the game's name and register it
            if (!isBlacklisted) {
                QString guessedGameName = QDir(folderPath).dirName();

                // Climb up the folder tree if the folder is just named "settings" or "saves"
                if (guessedGameName.toLower() == "saves" || guessedGameName.toLower() == "savegames"
                    || guessedGameName.toLower() == "settings" || guessedGameName.toLower() == "steam") {

                    QDir parentDir(folderPath);
                    parentDir.cdUp();
                    guessedGameName = parentDir.dirName(); // This will grab "Battlefield 6"

                    // If it's STILL a generic name like "settings", go up one more level
                    if (guessedGameName.toLower() == "settings") {
                        parentDir.cdUp();
                        guessedGameName = parentDir.dirName();
                    }
                }

                if (registerCustomGame(guessedGameName, folderPath)) {
                    qDebug() << "CRAWLER HIT: Found deep save ->" << guessedGameName << "at" << folderPath;
                    gamesDiscovered++;
                }
            }
        }
    }

    qDebug() << "HYBRID SCAN COMPLETE. Total games secured:" << gamesDiscovered;
}

void SaveController::resolveSteamNames() {
    qDebug() << "CONTACTING STEAM API FOR METADATA...";

    // 1. Ask SQLite for any game that still has a raw AppID name
    QSqlQuery query("SELECT id, game_name FROM Saves WHERE game_name LIKE 'Cracked Game (%)'");

    while (query.next()) {
        int dbId = query.value(0).toInt();
        QString oldName = query.value(1).toString();

        // 2. Extract just the number (e.g., "1627720" out of "Cracked Game (1627720)")
        int startIndex = oldName.indexOf('(') + 1;
        int endIndex = oldName.indexOf(')');
        QString appId = oldName.mid(startIndex, endIndex - startIndex);

        // 3. Prepare the HTTP GET Request to the official Steam Store API
        QString urlString = "https://store.steampowered.com/api/appdetails?appids=" + appId;
        QNetworkRequest request((QUrl(urlString)));
        QNetworkReply *reply = networkManager->get(request);

        // 4. Create an async listener for when the internet responds
        connect(reply, &QNetworkReply::finished, this, [this, reply, dbId, appId, oldName]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray responseData = reply->readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
                QJsonObject rootObj = jsonDoc.object();

                // 5. Parse the JSON response to grab the real game name
                if (rootObj.contains(appId) && rootObj[appId].toObject()["success"].toBool()) {
                    QString realName = rootObj[appId].toObject()["data"].toObject()["name"].toString();

                    // Clean the name so Windows doesn't crash (remove colons, slashes, etc.)
                    realName.replace(":", "");
                    realName.replace("/", "-");
                    realName.replace("\\", "-");

                    // NEW: Generate the updated Backup Path using the real name
                    QString newBackupPath = "E:/SaveVault/Backups/" + realName + "_Vault";

                    // 6. Update BOTH columns in the SQLite Database permanently
                    QSqlQuery updateQuery;
                    updateQuery.prepare("UPDATE Saves SET game_name = :newName, backup_path = :newPath WHERE id = :id");
                    updateQuery.bindValue(":newName", realName);
                    updateQuery.bindValue(":newPath", newBackupPath); // Save the new path!
                    updateQuery.bindValue(":id", dbId);

                    if (updateQuery.exec()) {
                        qDebug() << "API SUCCESS: Translated" << oldName << "->" << realName << "and updated vault path!";
                    }
                } else {
                    qDebug() << "API FAIL: Steam doesn't recognize AppID" << appId;
                }
            }
            reply->deleteLater(); // Clean up memory
        });
    }
}


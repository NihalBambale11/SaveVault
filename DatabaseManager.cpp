#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

#include <filesystem>
namespace fs = std::filesystem;

bool performBackup(QString originalPath, QString backupPath) {
    try {
        // Convert Qt's QString to standard C++ filesystem paths
        fs::path source(originalPath.toStdString());
        fs::path destination(backupPath.toStdString());

        // 1. Check if the original game save folder actually exists
        if (!fs::exists(source)) {
            qDebug() << "ERROR: Source path does not exist:" << originalPath;
            return false;
        }

        // 2. Create the backup directory if it's not already there
        if (!fs::exists(destination)) {
            fs::create_directories(destination);
        }

        // 3. Perform a recursive copy (includes all subfolders/User IDs)
        // copy_options::overwrite_existing ensures we update old backups
        fs::copy(source, destination, fs::copy_options::recursive | fs::copy_options::overwrite_existing);

        qDebug() << "SUCCESS: Files copied to" << backupPath;
        return true;
    } catch (const fs::filesystem_error& e) {
        qDebug() << "FILESYSTEM ERROR:" << e.what();
        return false;
    }
}


bool initializeDatabase() {
    // Tell Qt we are using the local SQLite engine
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // We will save the database right next to where the app runs
    db.setDatabaseName("vault_metadata.db");

    // Try to open it (SQLite creates the file automatically if it's missing)
    if (!db.open()) {
        qDebug() << "CRITICAL ERROR: Failed to connect to database." << db.lastError().text();
        return false;
    }

    qDebug() << "SUCCESS: SQLite Database Connected!";

    // Create our Saves table to track the backed-up files
    QSqlQuery query;
    bool tableCreated = query.exec(
        "CREATE TABLE IF NOT EXISTS Saves ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "game_name TEXT UNIQUE, " // Added UNIQUE here to stop duplicates forever!
        "original_path TEXT, "
        "backup_path TEXT, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "notes TEXT"
        ")"
        );

    if (!tableCreated) {
        qDebug() << "ERROR: Could not create table:" << query.lastError().text();
        return false;
    }

    qDebug() << "SUCCESS: 'Saves' table is ready!";
    return true;
}




bool addSaveEntry(QString gameName, QString originalPath, QString backupPath, QString notes) {
    QSqlQuery query;
    query.prepare("INSERT INTO Saves (game_name, original_path, backup_path, notes) "
                  "VALUES (:gameName, :originalPath, :backupPath, :notes)");

    query.bindValue(":gameName", gameName);
    query.bindValue(":originalPath", originalPath);
    query.bindValue(":backupPath", backupPath);
    query.bindValue(":notes", notes);

    if (!query.exec()) {
        qDebug() << "ERROR: Failed to add save entry:" << query.lastError().text();
        return false;
    }

    qDebug() << "SUCCESS: Registered backup for" << gameName;
    return true;
}


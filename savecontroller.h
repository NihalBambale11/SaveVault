#ifndef SAVECONTROLLER_H
#define SAVECONTROLLER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>

class SaveController : public QObject
{
    Q_OBJECT
public:
    explicit SaveController(QObject *parent = nullptr);

    Q_INVOKABLE void backupAllRegisteredGames();
    Q_INVOKABLE bool registerCustomGame(QString gameName, QString rawPath);

    // NEW: The Automated Deep-Scanner Engine
    Q_INVOKABLE void deepScanForSaves();

    // NEW: The API Trigger
    Q_INVOKABLE void resolveSteamNames();

private:
    // NEW: Handles our HTTP GET requests
    QNetworkAccessManager *networkManager;
};

#endif // SAVECONTROLLER_H
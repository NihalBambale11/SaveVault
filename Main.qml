import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs // Crucial: Gives us access to native Windows Dialogs

Window {
    width: 800
    height: 600
    visible: true
    title: "SaveVault Dashboard"
    color: "#121212"

    // This is the native Windows Folder picker window popup
    FolderDialog {
        id: customFolderPicker
        title: "Select Game Save Directory"
        currentFolder: "file:///C:/"

        onAccepted: {
            // This pops up a quick text prompt to ask for the game name
            // (A placeholder until we build a clean popup dialog box)
            console.log("Selected folder path: " + selectedFolder);
            saveController.registerCustomGame("Dynamic Manual Game", selectedFolder);
        }
    }

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Text {
            text: "GAME SAVE VAULT"
            color: "#00FF66"
            font.pixelSize: 24
            font.bold: true
        }

        Rectangle {
            width: parent.width
            height: 400
            color: "#1E1E1E"
            radius: 8
            border.color: "#333333"
            border.width: 1

            Text {
                text: "Your backed-up games will appear here dynamically soon."
                color: "#AAAAAA"
                anchors.centerIn: parent
                font.pixelSize: 16
            }
        }

        // Layout row for our controls
        Row {
            spacing: 15

            // BUTTON 1: Master Sync Button
            Button {
                text: "Run Master Backup"
                width: 180
                height: 45
                onClicked: saveController.backupAllRegisteredGames()

                background: Rectangle {
                    color: "#00FF66"
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "#121212"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            // BUTTON 2: Dynamic Add Folder Button
            Button {
                text: "+ Add Random Save Folder"
                width: 200
                height: 45
                onClicked: customFolderPicker.open() // Opens the native Windows folder navigator

                background: Rectangle {
                    color: "#1E1E1E"
                    radius: 4
                    border.color: "#00FF66"
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: "#00FF66"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: "Auto-Detect Games"
                width: 180
                height: 45
                onClicked: saveController.deepScanForSaves()

                background: Rectangle {
                    color: "#1E1E1E"
                    radius: 4
                    border.color: "#00BFFF"
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: "#00BFFF"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button {
                text: "Translate Names (Steam API)"
                width: 220
                height: 45
                onClicked: saveController.resolveSteamNames()

                background: Rectangle {
                    color: "#1E1E1E"
                    radius: 4
                    border.color: "#FFA500"
                    border.width: 1
                }
                contentItem: Text {
                    text: parent.text
                    color: "#FFA500"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}

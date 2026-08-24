import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: "Settings"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        // --- Game path ---
        ColumnLayout {
            spacing: 4
            Layout.fillWidth: true

            Label {
                text: "Game folder (Stardew Valley)"
                color: "#cccccc"
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                TextField {
                    id: gamePathField
                    Layout.fillWidth: true
                    text: configManager.gamePath
                    placeholderText: "Path to the game installation folder..."
                    onEditingFinished: configManager.gamePath = text
                }

                Button {
                    text: "Browse..."
                    onClicked: gamePathDialog.open()
                }
            }
        }

        // --- Mods path ---
        ColumnLayout {
            spacing: 4
            Layout.fillWidth: true

            Label {
                text: "Mods folder"
                color: "#cccccc"
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                TextField {
                    id: modsPathField
                    Layout.fillWidth: true
                    text: configManager.modsPath
                    placeholderText: "Path to the mods folder..."
                    onEditingFinished: configManager.modsPath = text
                }

                Button {
                    text: "Browse..."
                    onClicked: modsPathDialog.open()
                }
            }
        }

        Item { Layout.fillHeight: true }
    }

    FolderDialog {
        id: gamePathDialog
        title: "Select the game installation folder"
        onAccepted: {
            var path = selectedFolder.toString().replace("file://", "")
            configManager.gamePath = path
            gamePathField.text = path
        }
    }

    FolderDialog {
        id: modsPathDialog
        title: "Select the mods folder"
        onAccepted: {
            var path = selectedFolder.toString().replace("file://", "")
            configManager.modsPath = path
            modsPathField.text = path
        }
    }
}

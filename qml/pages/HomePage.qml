import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string statusText: ""
    property bool isLaunching: false
    property string runningListName: ""

    Label {
        visible: modListManager.rowCount() === 0
        anchors.centerIn: parent
        text: "No mod lists yet. Create one in \"Mod Lists\"."
        color: "#999999"
        font.pixelSize: 16
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16
        visible: modListManager.rowCount() > 0

        Label {
            text: "Your Mod Lists"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        Label {
            id: statusLabel
            text: root.statusText
            color: "#e07b7b"
            visible: text.length > 0
        }

        GridView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            cellWidth: 220
            cellHeight: 260

            model: modListManager

            delegate: Item {
                width: 200
                height: 240

                property bool isThisRunning: root.runningListName === model.name
                property bool anyRunning: root.runningListName.length > 0

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 8
                    radius: 8
                    color: "#242424"
                    border.color: isThisRunning ? "#5a9bd4" : "#3a3a3a"
                    border.width: isThisRunning ? 2 : 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 130
                            radius: 6
                            color: "#1a1a1a"
                            clip: true

                            Image {
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectCrop
                                source: model.thumbnailUrl
                                visible: model.thumbnailUrl.length > 0
                            }

                            Label {
                                anchors.centerIn: parent
                                text: "No image"
                                color: "#555555"
                                visible: model.thumbnailUrl.length === 0
                            }
                        }

                        Label {
                            text: model.name
                            color: "white"
                            font.pixelSize: 15
                            font.bold: true
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        Label {
                            text: model.modCount + " mods"
                            color: "#999999"
                            font.pixelSize: 11
                        }

                        Item { Layout.fillHeight: true }

                        Button {
                            Layout.fillWidth: true
                            text: isThisRunning ? "Running" : (root.isLaunching ? "Launching..." : "Play")
                            enabled: !anyRunning && !root.isLaunching

                            onClicked: {
                                root.statusText = ""
                                root.isLaunching = true

                                var gameModsPath = configManager.gamePath + "/Mods"
                                var deployOk = modListManager.deploy(model.name, configManager.modsPath, gameModsPath)

                                if (!deployOk) {
                                    root.statusText = "Deploy finished with warnings — some mods may be missing."
                                }

                                var customSaves = modListManager.savesPathOf(model.name)
                                if (customSaves.length > 0) {
                                    if (!saveManager.activateCustomSaves(customSaves)) {
                                        root.statusText = "Could not activate custom saves — launching with default saves."
                                    }
                                }

                                var launchOk = gameLauncher.launch(configManager.gamePath)
                                if (!launchOk) {
                                    root.statusText = "Could not launch the game. Check the Game folder in Settings."
                                } else {
                                    root.runningListName = model.name
                                }

                                root.isLaunching = false
                            }

                            background: Rectangle {
                                color: {
                                    if (isThisRunning) return "#3a8f5f"
                                    if (!parent.enabled) return "#2a2a2a"
                                    return parent.hovered ? "#3a6fa5" : "#2f5f8f"
                                }
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                color: parent.enabled || isThisRunning ? "white" : "#777777"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: gameLauncher
        function onGameClosed() {
            root.runningListName = ""
            root.statusText = ""
        }
    }
}

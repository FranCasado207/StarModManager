import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    Component.onCompleted: modListModel.refresh()

    property string modPendingDeletion: ""

    Label {
        visible: configManager.modsPath.length === 0
        anchors.centerIn: parent
        text: "Set a mods folder in Settings first."
        color: "#999999"
        font.pixelSize: 16
    }

    ColumnLayout {
        visible: configManager.modsPath.length > 0
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Label {
            text: "Mods"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        Rectangle {
            id: dropZone
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            radius: 8
            color: dropArea.containsDrag ? "#2f2f2f" : "#242424"
            border.color: dropArea.containsDrag ? "#5a9bd4" : "#3a3a3a"
            border.width: 2

            Label {
                anchors.centerIn: parent
                text: "Drag mod archives here (.zip, .7z, .tar.gz)"
                color: "#999999"
            }

            DropArea {
                id: dropArea
                anchors.fill: parent
                onDropped: (drop) => {
                    if (drop.hasUrls) {
                        for (var i = 0; i < drop.urls.length; i++) {
                            modInstaller.installMod(drop.urls[i], configManager.modsPath)
                        }
                    }
                }
            }
        }

        Label {
            id: statusLabel
            color: "#e07b7b"
            visible: text.length > 0
            text: ""
        }

        Label {
            text: "Installed mods (" + modListModel.count + ")"
            color: "#cccccc"
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: modListModel
            spacing: 4
            clip: true

            delegate: Rectangle {
                width: ListView.view.width
                height: 40
                color: "#242424"
                radius: 4

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8

                    Label {
                        Layout.fillWidth: true
                        text: model.name
                        color: "white"
                    }

                    Button {
                        text: "Delete"
                        onClicked: {
                            root.modPendingDeletion = model.name
                            deleteDialog.open()
                        }

                        background: Rectangle {
                            color: parent.hovered ? "#7a3030" : "transparent"
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#e07b7b"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: deleteDialog
        title: "Delete mod"
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Yes | Dialog.No

        Label {
            text: "Are you sure you want to delete \"" + root.modPendingDeletion + "\"?\nThis cannot be undone."
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            if (!modListModel.removeMod(root.modPendingDeletion)) {
                statusLabel.text = "Failed to delete \"" + root.modPendingDeletion + "\"."
            }
            root.modPendingDeletion = ""
        }

        onRejected: {
            root.modPendingDeletion = ""
        }
    }

    Connections {
        target: modInstaller
        function onModInstalled(modName) {
            statusLabel.text = ""
            modListModel.refresh()
        }
        function onInstallFailed(fileName, reason) {
            statusLabel.text = "Failed to install " + fileName + ": " + reason
        }
    }
}

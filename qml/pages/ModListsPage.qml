import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property string selectedListName: ""
    property var selectedMods: []
    property bool hasUnsavedChanges: false

    function loadList(name) {
        root.selectedListName = name
        var current = modListManager.modsInList(name)
        root.selectedMods = current.slice()
        root.hasUnsavedChanges = false
        renameField.text = name
        statusLabel.text = ""
    }

    function isModSelected(modName) {
        return root.selectedMods.indexOf(modName) !== -1
    }

    function toggleMod(modName, checked) {
        var mods = root.selectedMods.slice()
        var i = mods.indexOf(modName)
        if (checked && i === -1) mods.push(modName)
        if (!checked && i !== -1) mods.splice(i, 1)
        root.selectedMods = mods
        root.hasUnsavedChanges = true
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 24

        ColumnLayout {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            spacing: 8

            Label {
                text: "Mod Lists"
                color: "white"
                font.pixelSize: 18
                font.bold: true
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 4

                TextField {
                    id: newListField
                    Layout.fillWidth: true
                    placeholderText: "New list name..."
                }

                Button {
                    text: "+"
                    onClicked: {
                        if (modListManager.createModList(newListField.text)) {
                            root.loadList(newListField.text.trim())
                            newListField.text = ""
                        }
                    }
                }
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: modListManager
                spacing: 4
                clip: true

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 44
                    radius: 4
                    color: model.name === root.selectedListName ? "#2f4f6f" : "#242424"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.loadList(model.name)
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 0

                        Label {
                            text: model.name
                            color: "white"
                        }
                        Label {
                            text: model.modCount + " mods"
                            color: "#999999"
                            font.pixelSize: 11
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12
            visible: root.selectedListName.length > 0

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                TextField {
                    id: renameField
                    Layout.fillWidth: true
                }

                Button {
                    text: "Rename"
                    onClicked: {
                        var newName = renameField.text.trim()
                        if (newName.length > 0 && newName !== root.selectedListName) {
                            if (modListManager.renameModList(root.selectedListName, newName)) {
                                root.selectedListName = newName
                            } else {
                                statusLabel.text = "Could not rename (name empty or already exists)."
                            }
                        }
                    }
                }
            }

            Label {
                text: "Select mods for this list:"
                color: "#cccccc"
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: modListModel
                spacing: 2
                clip: true

                delegate: RowLayout {
                    width: ListView.view.width
                    height: 32

                    CheckBox {
                        checked: root.isModSelected(model.name)
                        onToggled: root.toggleMod(model.name, checked)
                    }

                    Label {
                        text: model.name
                        color: "white"
                        Layout.fillWidth: true
                    }
                }
            }

            Label {
                id: statusLabel
                color: "#e07b7b"
                visible: text.length > 0
            }

            Button {
                Layout.fillWidth: true
                text: root.hasUnsavedChanges ? "Save" : "Deploy"

                onClicked: {
                    if (root.hasUnsavedChanges) {
                        modListManager.setModsInList(root.selectedListName, root.selectedMods)
                        root.hasUnsavedChanges = false
                        statusLabel.text = ""
                    } else {
                        var gameModsPath = configManager.gamePath + "/Mods"
                        var ok = modListManager.deploy(root.selectedListName, configManager.modsPath, gameModsPath)
                        statusLabel.text = ok ? "Deployed successfully." : "Deploy finished with errors — check the log."
                    }
                }
            }
        }

        Label {
            visible: root.selectedListName.length === 0
            text: "Create or select a mod list to get started."
            color: "#999999"
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

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
        savesPathField.text = modListManager.savesPathOf(name)
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

        // --- Left: list of mod lists ---
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

        // --- Right: editor for the selected list ---
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

            // --- Thumbnail ---
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Rectangle {
                    width: 64
                    height: 64
                    radius: 6
                    color: "#242424"
                    border.color: "#3a3a3a"
                    clip: true

                    Image {
                        anchors.fill: parent
                        anchors.margins: 2
                        fillMode: Image.PreserveAspectCrop
                        source: modListManager.thumbnailUrl(root.selectedListName)
                        visible: source.toString().length > 0
                    }

                    Label {
                        anchors.centerIn: parent
                        text: "No image"
                        color: "#666666"
                        font.pixelSize: 10
                        visible: modListManager.thumbnailUrl(root.selectedListName).length === 0
                    }
                }

                Button {
                    text: "Set thumbnail..."
                    onClicked: thumbnailDialog.open()
                }
            }

            // --- Custom saves folder ---
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Label {
                    text: "Custom saves folder (optional)"
                    color: "#cccccc"
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    TextField {
                        id: savesPathField
                        Layout.fillWidth: true
                        placeholderText: "Leave empty to use the game's default saves"
                        onEditingFinished: modListManager.setSavesPath(root.selectedListName, text)
                    }

                    Button {
                        text: "Browse..."
                        onClicked: savesPathDialog.open()
                    }

                    Button {
                        text: "Clear"
                        enabled: savesPathField.text.length > 0
                        onClicked: {
                            savesPathField.text = ""
                            modListManager.setSavesPath(root.selectedListName, "")
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

    FileDialog {
        id: thumbnailDialog
        title: "Choose a thumbnail image"
        nameFilters: ["Images (*.png *.jpg *.jpeg *.webp)"]
        onAccepted: {
            modListManager.setThumbnail(root.selectedListName, selectedFile)
        }
    }

    FolderDialog {
        id: savesPathDialog
        title: "Select a custom saves folder for this mod list"
        onAccepted: {
            var path = selectedFolder.toString().replace("file://", "")
            savesPathField.text = path
            modListManager.setSavesPath(root.selectedListName, path)
        }
    }
}

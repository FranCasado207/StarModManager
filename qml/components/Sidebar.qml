import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: sidebar
    color: "#2b2b2b"

    signal homeClicked()
    signal settingsClicked()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        Button {
            text: "Homepage"
            Layout.fillWidth: true
            onClicked: sidebar.homeClicked()

            background: Rectangle {
                color: parent.hovered ? "#3d3d3d" : "transparent"
                radius: 4
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: 8
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            text: "Settings"
            Layout.fillWidth: true
            onClicked: sidebar.settingsClicked()

            background: Rectangle {
                color: parent.hovered ? "#3d3d3d" : "transparent"
                radius: 4
            }
            contentItem: Text {
                text: parent.text
                color: "white"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: 8
            }
        }
    }
}

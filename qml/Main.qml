import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"
import "pages"

ApplicationWindow {
    id: root
    width: 900
    height: 600
    visible: true
    title: "Star Mod Manager"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Sidebar {
            Layout.preferredWidth: 180
            Layout.fillHeight: true

            onHomeClicked: contentLoader.source = "pages/HomePage.qml"
            onSettingsClicked: contentLoader.source = "pages/SettingsPage.qml"
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"

            Loader {
                id: contentLoader
                anchors.fill: parent
                source: "pages/HomePage.qml"
            }
        }
    }
}

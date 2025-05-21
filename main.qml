import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Window 2.12
import QtQml 2.12

ApplicationWindow {
    visible: true
    width: 400
    height: 600
    title: "TimeTrack"
    Material.theme: Material.Dark
    Material.foreground: "white"

    ListModel {
        id: appStatsModel
    }

    Timer {
        interval: 100
        running: true
        repeat: false
        onTriggered: updateStats()
    }

    Item {
        Connections {
            target: backend
            onAppStatsChanged: updateStats()
        }
    }

    function updateStats() {
        const stats = backend.getAppUsageStats()
        for (let i = 0; i < stats.length; ++i) {
            if (i < appStatsModel.count) {
                const existing = appStatsModel.get(i)
                if (existing.seconds !== stats[i].seconds || existing.app !== stats[i].app) {
                    appStatsModel.set(i, stats[i])
                }
            } else {
                appStatsModel.append(stats[i])
            }
        }

        while (appStatsModel.count > stats.length) {
            appStatsModel.remove(appStatsModel.count - 1)
        }       
    }

    Column {
        anchors.fill: parent
        spacing: 16
        padding: 16

        Text {
            id: currentAppLabel
            text: "Текущее приложение: " + backend.currentAppName
            font.pixelSize: 16
            color: "white"

            property bool hovered: false

            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: backend.currentApp

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: currentAppLabel.hovered = true 
                onExited: currentAppLabel.hovered = false
            }
        }

        Text {
            text: "Общее время: " + backend.formattedTime
            font.pixelSize: 16
            color: "white"
        }

        Button {
            text: "Экспорт в CSV"
            onClicked: backend.exportToCSV()
        }

        Button {
            text: "Очистить статистику"
            onClicked: backend.resetStats()
        }

        Text {
            text: "Статистика по приложениям:"
            font.bold: true
            font.pixelSize: 14
            padding: 8
            color: "white"
        }

        ListView {
            model: appStatsModel
            height: 400
            width: parent.width
            clip: true

            delegate: Row {
                spacing: 16
                width: parent.width
                height: 24

                property bool hovered: false

                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: appPath

                Text {
                    id: appText
                    text: app
                    width: 250
                    elide: Text.ElideRight
                    color: "white"

                    property bool hovered: false

                    ToolTip.visible: hovered
                    ToolTip.delay: 500
                    ToolTip.text: appPath

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: hovered = true
                        onExited: hovered = false
                    }  
                }

                Text {
                    text: seconds + " сек."
                    color: "white"
                }

            }
        }
    }
}

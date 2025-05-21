import QtQuick 2.12
import QtQuick.Controls 2.12
import Tracker 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 480
    height: 320
    title: qsTr("TimeTrack")

    TrackerBackend {
        id: tracker
    }

    Column {
        anchors.centerIn: parent
        spacing: 16

        Text {
            id: currentAppLabel
            text: qsTr("Текущее приложение: \n") + (tracker.currentApp.length ? tracker.currentApp : qsTr("—"))
            font.pixelSize: 18
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            width: parent.width * 0.9
        }

        Text {
            id: totalTimeLabel
            text: qsTr("Общее время: ") + tracker.totalTime + qsTr(" сек.")
            font.pixelSize: 16
        }

        Button {
            id: exportBtn
            text: qsTr("Экспорт в CSV")
            onClicked: tracker.exportToCSV()
            width: 160
            height: 36
        }
    }
}

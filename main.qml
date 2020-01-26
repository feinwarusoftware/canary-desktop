import QtQuick 2.3
import QtQuick.Window 2.3
import QtWebEngine 1.1
import QtWebChannel 1.0

Window {
    width: 1024
    height: 750
    visible: true

    WebChannel {
        id: channel
        registeredObjects: [playerObject]
    }

    QtObject {
        id: playerObject
        WebChannel.id: "playerObject"
    }

    WebEngineView {
        anchors.fill: parent
        url: "http://192.168.0.105:3000/"
        webChannel: channel
    }
}
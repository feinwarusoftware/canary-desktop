import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtWebChannel 1.0
import QtWebEngine 1.1

Window {
    visible: true
    width: 1280
    height: 480
    title: qsTr("Hello World")

    WebEngineView {
        anchors.fill: parent
        url: "qrc:/index.html"

        webChannel: channel
    }

	Text{
		id:debug
		text:"Oi, eu sou o debug"
		font.pointSize: 24
		y:200
	}

	function updateCSInfo(length){
		//debug.text = "oi, a nova length:" + length;

		playerObject.setCSInfo(length);
	}

    QtObject {
        id: playerObject

        // ID, under which this object will be known at WebEngineView side
        WebChannel.id: "playerObject"

        signal setCSInfo(int len);
        signal setCSPos(int pos);

        function play() {
            timer.start()
            return player.play();
        }

		function pause() {
            timer.stop()
            return player.pause();
        }

        function changeVolume(val){
            return player.changeVolume(val);
        }

		function loadSong(n){
			return player.loadSong(n);
		}

        function seek(to){
			timer.restart();
            return player.seek(to);
        }

        function jump(direction){
            if(direction == "previous"){
                if(player.getPositionInSeconds() >= 5){
                    player.seek(0)
                }
                else{
                    player.jump(0);
                }
            }
            else{
                player.jump(1)
            }

            timer.restart()
        }
    }

    Timer {
		id:timer
		property double currentPos
        interval: 1000; //to 1000?
		running: false 
		repeat: true
		triggeredOnStart: true //set to false(?)
        onTriggered: {
            currentPos = player.getPositionInSeconds()
            playerObject.setCSPos(currentPos);
		}
    }

    WebChannel {
        id: channel
        registeredObjects: [playerObject]
    }
}

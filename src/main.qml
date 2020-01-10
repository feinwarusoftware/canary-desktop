import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4
import QtWebChannel 1.0
import QtWebEngine 1.1

Window {
    visible: true
    width: 1280
    height: 720
    title: qsTr("Canary Pre-Alpha")

	WebEngineView {
		anchors.fill: parent
		//url: "qrc:/vue/index.html"
		url: "http://localhost:8080/"

		settings.localContentCanAccessRemoteUrls: true;
		settings.localContentCanAccessFileUrls : true;

		webChannel: channel
	}

	Text{
		id:debug
		//text:"Oi, eu sou o debug"
		font.pointSize: 24
		y:200
	}

	function updateCSInfo(length, coverData, artist, album, title){
		//debug.text = "oi, a nova length:" + length;

		playerObject.setCSInfo(length, coverData, artist, album, title);
	}

    QtObject {
        id: playerObject

        // ID, under which this object will be known at WebEngineView side
        WebChannel.id: "playerObject"

		property bool isDraggingSeekbar;
		property string location;
		property string config;
		property string userLibrary;
		property string albums;

		signal appLoaded();
        signal setCSInfo(int len, string coverData, string artist, string album, string songTitle);
        signal setCSPos(int pos);
		signal libraryLoaded(string lib);

		//TODO: transform to object

		function loadApp(){
			isDraggingSeekbar = false;
			location = library.getAppLocation().toString();
			config = library.getData("config").toString();
			userLibrary = library.getData("library").toString();
			albums = library.getData("albums").toString();
			return playerObject.appLoaded();
		}

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

		function insertToQueue(pos, dir){
			console.log(dir);
			return player.insertToQueue(pos, dir);
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
        interval: 500;
		running: false 
		repeat: true
		triggeredOnStart: true //set to false(?)
        onTriggered: {
			if(playerObject.isDraggingSeekbar){
				return;
			}
            currentPos = player.getPositionInSeconds()
            playerObject.setCSPos(currentPos);
		}
    }

    WebChannel {
        id: channel
        registeredObjects: [playerObject]
    }
}

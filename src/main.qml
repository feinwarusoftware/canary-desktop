import QtQuick 2.3
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4
import QtWebChannel 1.0
import QtWebEngine 1.1

Window {
    visible: true
    width: 1280
    height: 480
    title: qsTr("Canary Pre-Alpha")

	/*TabView{
		anchors.fill: parent
		Tab{
			title:"mainTab"
			    WebEngineView {
					anchors.fill: parent
					url: "qrc:/index.html"

					settings.localContentCanAccessRemoteUrls: true;
					settings.localContentCanAccessFileUrls : true;

					webChannel: channel
				}
		}

		Tab{
			title:"albumViewer"
		}
	
	}*/

	WebEngineView {
		anchors.fill: parent
		url: "qrc:/vue/index.html"

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

		signal appLoaded(string location);
        signal setCSInfo(int len, string coverData, string artist, string album, string songTitle);
        signal setCSPos(int pos);
		signal libraryLoaded(string lib);

		function loadApp(){
			return playerObject.appLoaded(library.getAppLocation().toString());
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

		function getLibrary(){
			return playerObject.libraryLoaded(library.getLibrary().toString());
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
            currentPos = player.getPositionInSeconds()
            playerObject.setCSPos(currentPos);
		}
    }

    WebChannel {
        id: channel
        registeredObjects: [playerObject]
    }
}

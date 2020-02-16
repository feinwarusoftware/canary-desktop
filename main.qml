import QtQuick 2.3
import QtQuick.Window 2.3
import QtWebEngine 1.1
import QtWebChannel 1.0

Window {

	function changeNowPlaying(data, isLoadedSong){
        //(maybe) TODO: pass as property, use signal to force re-render; property variant nowPlaying, etc. - not necessary now, may be
		playerObject.setNowPlayingInfo(data);
        if(!isLoadedSong){
            playerObject.queueUpdate();
        }
	}

    function changeTime(time){
        if(!playerObject.isDraggingSeekbar){
            playerObject.setNowPlayingPos(time);
        }
    }

    function clear(){
        playerObject.setNowPlayingPos(0);
        playerObject.clear();
    }

    width: 1024
    height: 750
    visible: true

    WebChannel {
        id: channel
        registeredObjects: [playerObject, appObject]
    }

    QtObject {
        id: playerObject
        objectName:"playerObject"
        WebChannel.id: "playerObject"

        property var playerClass;
        property bool isDraggingSeekbar;

        signal setNowPlayingPos(double pos);
        signal setNowPlayingInfo(variant data);
        signal queueUpdate();
        signal clear();

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
        }

        function loadSong(pos){
            player.loadSong(pos);
        }

        function insertToQueue(pos, dir){
            player.insertToQueue(pos, dir);
            playerObject.queueUpdate();
        }
    }

    QtObject {
        id: appObject
        WebChannel.id: "appObject"

        signal appLoaded();

        function loadApp(){
            playerObject.playerClass = player;
			playerObject.isDraggingSeekbar = false;
			return appObject.appLoaded();
	    }
    }

    WebEngineView {
        anchors.fill: parent
        url: "http://localhost:8080/"
        webChannel: channel
    }
}
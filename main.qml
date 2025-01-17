import QtQuick 2.3
import QtQuick.Window 2.3
import QtWebEngine 1.1
import QtWebChannel 1.0

Window {

	function changeNowPlaying(data){
        //(maybe) TODO: pass as property, use signal to force re-render; property variant nowPlaying, etc. - not necessary now, may be
        playerObject.setNowPlayingPos(0);
		playerObject.setNowPlayingInfo(data);
        playerObject.queueUpdate(player.getQueue());
	}

    function changeTime(time){
        if(!playerObject.isDraggingSeekbar){
            playerObject.setNowPlayingPos(time);
        }
    }

    function clear(){
        playerObject.setNowPlayingPos(0);
        playerObject.clear();
        playerObject.queueUpdate(player.getQueue());
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
        signal queueUpdate(variant data);
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

        function insertToQueue(pos, dir){
            player.insertToQueue(pos, dir);
            playerObject.queueUpdate(player.getQueue());
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
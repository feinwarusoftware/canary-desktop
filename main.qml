import QtQuick 2.3
import QtQuick.Window 2.3
import QtWebEngine 1.1
import QtWebChannel 1.0

Window {

	function changeNowPlaying(length, coverData, artist, album, title){
		playerObject.setNowPlayingInfo(length, coverData, artist, album, title);
	}

    function changeTime(time){
        if(!playerObject.isDraggingSeekbar){
            playerObject.setNowPlayingPos(time);
        }
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
        WebChannel.id: "playerObject"

        property var playerClass;
        property bool isDraggingSeekbar;

        signal setNowPlayingPos(double pos);
        signal setNowPlayingInfo(int len, string coverData, string artist, string album, string songTitle);

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
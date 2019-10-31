import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.5

Window {
    visible: true
    width: 1500
    height: 900
    title: qsTr("Canary")
	color: "#333333"

	function setLength(length){
		trackbar.songLen = length
		sC.text = player.toMinHourFormat(length)
		innerTrackbar.width = 0
		dCounter.text = "0:00"
	}

	function setCurrentSongData(title, album, artist){
		debugbox.text = title + " from " + album + " by " + artist;
	}

	function reload(){
		var oldSource = coverArt.source;
        coverArt.source = ""
        coverArt.source = oldSource
    }

    Timer {
		id:timer
		property int currentPos
        interval: 500; //to 1000?
		running: false 
		repeat: true
		triggeredOnStart: true //set to false(?)
        onTriggered: {
			currentPos = player.getPosition()
			dCounter.text = player.toMinHourFormat(currentPos)
			innerTrackbar.width = parseInt((currentPos * trackbar.width) / trackbar.songLen)
		}
    }

	//pass toMinHourFormat to JS

	function timerControl(arg){
		if(arg == 1){
			return timer.start()
		}
		else{
			return timer.stop()
		}

		//TODO: ADD RESTART TIMER FOR JUMP!!!!
	}

	Text {
		id: debugbox
		objectName: "debug"
		text: "oi" 
		font.family: "Helvetica"
		font.pointSize: 24
		color: "red"
	}

	Rectangle { //pass this to multiple files later on - CONTROLS BAR BG
		width: 1500
		height: 67
		color: "#ffa800"
		anchors.bottom: parent.bottom //or root.bottom?

		Rectangle {
			objectName: "back"
			width:24
			height: 18
			x: 44
			y: 25
			color:"red"
			MouseArea {
				anchors.fill: parent
				onClicked: {
					player.jump(-1)
				}
			}
		}

		Rectangle {
			width:24
			height: 18
			x: 138
			y: 25
			color:"red"
			MouseArea {
				anchors.fill: parent
				onClicked: {
					player.jump(1)
				}
			}
		}

		Rectangle {
			width:24
			height: 28
			x: 91
			y: 20
			color:"red"

			MouseArea {
				anchors.fill: parent
				onClicked: {
					if(player.active())
						player.pause()
					else
						player.play()
					}
			}
		}

		Slider {
			from: 0
			value: 1
			to: 1
			x: 170
			y: 20
			handle: Rectangle { //"Rectangle" necessary?
				visible:false
			}
			onValueChanged:{
				//debugbox.text = trackbar.songLen
				player.changeVolume(this.value)
			}
		}

		Rectangle{
			id:trackbar
			objectName:"trackbar"
			property int songLen: 0
			x:370
			y:5
			width:700
			height:50
			color:"green"
			Rectangle{
				objectName:"innerTrackbar"
				id:innerTrackbar
				//width:5
				width:0
				height:parent.height
				color:"red"
			}
			MouseArea{
				anchors.fill: parent
				onClicked:{
					innerTrackbar.width = this.mouseX
					player.seek(innerTrackbar.width, parent.width)
				}
				onPressed:{
					player.pause(1)
					innerTrackbar.width = this.mouseX //necessary? (currently think so, try holding but not moving)
					player.seek(innerTrackbar.width, parent.width)
				}
				onReleased:{
					if(player.active())
						player.play()
					player.seek(innerTrackbar.width, parent.width)
				}
				onPositionChanged:{

					if(this.mouseX < 0){ //(apparently) had a little logic problem before (seemed to be related to order) but now it seems to be working
						innerTrackbar.width = 0
					}
					else if(this.mouseX <= this.width){
						innerTrackbar.width = this.mouseX
					}
					else{
						innerTrackbar.width = parent.width
					}

					debugbox.text = parseInt((innerTrackbar.width * parent.songLen) / parent.width);


					dCounter.text = player.toMinHourFormat(parseInt((innerTrackbar.width * parent.songLen) / parent.width))
				}
			}
		}

		Text{
			id:sC
			objectName:"staticCounter"
			anchors.bottom: parent.bottom
			x: 1450
			text:"0:00"
			color:"blue"
			font.pointSize: 14
		}

		Text{
			id:dCounter
			objectName:"dynamicCounter"
			anchors.bottom: parent.bottom
			text:"0:00"
			color:"blue"
			font.pointSize: 14
			x: 1400
		}
	}

	Rectangle {
		id:coverBox
		width:256
		height:256
		color:"blue"
		anchors {
			bottom: parent.bottom
			bottomMargin: 67
		}
        Image {
			objectName:"coverArt"
            id: coverArt
            anchors.fill:coverBox
			//mipmap: true
			cache: false
        }
	}
}

//TODO - KNOWN BUG: RELEASED NOT SEEK (-1 THING!!!)
//TODO - PASS TOMINHOURFORMAT TO HERE
//TODO - RESET TIMER WHEN SONG BEGINS(???)
//TODO - REFORM TIMER (PASS SECONDS, SECONDS TO MM:SS IN QML)
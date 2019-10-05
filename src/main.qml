import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.5

Window {
    visible: true
    width: 1500
    height: 900
    title: qsTr("Canary")
	color: "#333333"

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
				player.changeVolume(this.value)
			}
		}

		Slider{
			objectName: "trackbar"
			from: 0
			value: 0
			to: 0
			x: 370
			y: 20
			onValueChanged:{
				//player.seek(this.value)
				player.setCurrentLen(1) //this does magic, apparently
			}
			//onPressAndHold:{}
			onPressedChanged:{
				//player.setCurrentLen()
				player.seek(this.value)
				if(!pressed && player.active()) //TODO: DOES NOT WORK AS INTENDED - ONLY PAUSES ON CHACE - SUPPOSED TO PAUSE WHENEVER CLICKED
					player.play()
			}
		}

		Text{
			objectName:"staticCounter"
			anchors.bottom: parent.bottom
			x: 1450
			text:"0:00"
			color:"blue"
			font.pointSize: 14
		}

		Text{
			objectName:"dynamicCounter"
			anchors.bottom: parent.bottom
			text:"0:00"
			color:"blue"
			font.pointSize: 14
			x: 1400
		}
	}

	Rectangle {
		width:256
		height:256
		color:"blue"
		anchors {
			bottom: parent.bottom
			bottomMargin: 67
		}
	}
}
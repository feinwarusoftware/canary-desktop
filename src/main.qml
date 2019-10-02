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
			from: 0
			value: 0
			to: 0
			x: 370
			y: 20
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
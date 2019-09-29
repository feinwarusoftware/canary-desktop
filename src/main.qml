import QtQuick 2.13
import QtQuick.Window 2.13

Window {
    visible: true
    width: 1500
    height: 900
    title: qsTr("Canary")
	color: "#333333"

	Text {
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
		}

		Rectangle {
			width:24
			height: 18
			x: 138
			y: 25
			color:"red"
		}

		Rectangle {
			width:24
			height: 28
			x: 91
			y: 20
			color:"red"
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
import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

Window {
    id: window
    property int snoreBaseSize: body.font.pixelSize


    width: snoreBaseSize * 30
    height: snoreBaseSize * 9
    color: notifyWidget.color


    onVisibleChanged: {
        if(visible){
            x = 0
            animation.from = notifyWidget.animationFrom
            animation.start()
        }
    }

    NumberAnimation{
        id: animation
        properties: "x"
        duration: 500
        target: window
        from: notifyWidget.animationFrom
        to: notifyWidget.animationTo
    }

    Rectangle{
        id: root

        // don't anchor as we need he xChanged signal
        // a drag event directly on the window is not supported
        height: window.height
        width: window.width
        color: window.color

        Drag.active: mouseAreaAccept.drag.active


        onXChanged: {
            // There is a Qt bug which will not stop the mouse tracking if the
            // item is hid during a drag event.
            if(Drag.active){
                window.x += x
                if(Math.abs(window.x - (notifyWidget.isOrientatedLeft?
                                            notifyWidget.animationTo:
                                            notifyWidget.animationFrom) + mouseAreaAccept.mouseX) <= width * 0.05){
                    Drag.cancel()
                    notifyWidget.dismissed()
                }
            } else {
                x = 0
            }
        }

        MouseArea {
            id: mouseAreaAccept
            anchors.fill: parent
            z: 90
            onClicked: {
                notifyWidget.invoked()
            }
            drag.target: root
            drag.axis: Drag.XAxis
            drag.maximumX: notifyWidget.dragMaxX
            drag.minimumX: notifyWidget.dragMinX
            drag.smoothed: true
            onPressed: {
                animation.stop()
            }
            onReleased: {
                animation.from = window.x
                animation.start()
            }
        }

        RowLayout {
            spacing:  snoreBaseSize / 2
            anchors.fill: parent
            anchors.margins: snoreBaseSize

            Image {
                id: image
                Layout.fillHeight: true
                width: height
                fillMode: Image.PreserveAspectFit
                smooth: true
                z: 4
                onWidthChanged: notifyWidget.imageSize = width
                source: notifyWidget.image
            }

            ColumnLayout{
                spacing: 0

                RowLayout{
                    spacing:  snoreBaseSize / 2
                    Text {
                        id: title
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        verticalAlignment: Qt.AlignTop
                        color: notifyWidget.textColor
                        text: notifyWidget.title
                        font.pointSize: body.font.pointSize * 1.5
                        font.bold: true
                        textFormat: Text.StyledText
                        font.family: notifyWidget.fontFamily
                        elide: Text.ElideRight
                    }

                    Canvas{
                        id: closeButton
                        height: title.height
                        width: height
                        z: 91

                        onPaint: {
                            var context = getContext("2d");
                            context.lineWidth = snoreBaseSize * 0.25;

                            context.beginPath();
                            context.clearRect(0, 0, width, height);
                            context.fill();

                            if(mouseAreaCloseButton.containsMouse)
                            {
                                context.beginPath();
                                context.strokeStyle = root.color
                                context.fillStyle = body.color
                                context.globalAlpha = 0.5
                                context.moveTo(width/2 + width/2 , height/2);
                                context.arc(width/2, height/2, width/2 , 0, 2*Math.PI, true)
                                context.fill();
                                context.stroke();
                            }

                            var margin = snoreBaseSize * 0.5
                            context.beginPath();
                            context.globalAlpha = 1
                            context.strokeStyle = body.color
                            context.moveTo(margin, margin);
                            context.lineTo(width - margin, height - margin);
                            context.moveTo(width - margin, 0 + margin);
                            context.lineTo(margin, height - margin);
                            context.stroke();
                        }

                        MouseArea {
                            id: mouseAreaCloseButton
                            anchors.fill: parent
                            onClicked: notifyWidget.dismissed()
                            hoverEnabled: true
                            onEntered: parent.requestPaint()
                            onExited: parent.requestPaint()
                        }
                    }
                }

                RowLayout{
                    spacing:  snoreBaseSize / 2

                    Layout.alignment: Qt.AlignTop
                    Text {
                        id: body
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignTop
                        verticalAlignment: Qt.AlignTop
                        color: notifyWidget.textColor
                        text: notifyWidget.body
                        font.pointSize: 10
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        onLinkActivated: Qt.openUrlExternally(link)
                        textFormat: Text.StyledText
                        font.family: notifyWidget.fontFamily
                        elide: Text.ElideRight
                    }

                    Image {
                        id: appIcon
                        fillMode: Image.PreserveAspectFit
                        Layout.fillHeight: true
                        Layout.maximumHeight: title.height
                        Layout.alignment: Qt.AlignBottom
                        width: height
                        smooth: true
                        onWidthChanged: notifyWidget.appIconSize = width
                        source: notifyWidget.appIcon
                    }
                }
            }
        }
    }
}


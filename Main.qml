import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: window
    width: 1120
    height: 700
    visible: true
    title: "Park Out - UI Primeiro Incremento"
    color: "#F3F4F6"

    property int selectedBusIndex: -1
    readonly property int boardPadding: 12
    readonly property real boardCellSize: Math.max(24, Math.floor(Math.min((boardArea.width - boardPadding * 2) / gameController.cols, (boardArea.height - boardPadding * 2) / gameController.rows)))
    readonly property real boardPixelWidth: boardCellSize * gameController.cols
    readonly property real boardPixelHeight: boardCellSize * gameController.rows

    function directionArrow(direction) {
        switch (direction) {
        case "right":
            return "▶";
        case "left":
            return "◀";
        case "up":
            return "▲";
        case "down":
            return "▼";
        default:
            return "•";
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#F3F4F6"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 78
            radius: 12
            color: "#1E293B"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 14

                Label {
                    text: "Park Out • Pré-visualização da interface"
                    color: "white"
                    font.pixelSize: 22
                    font.bold: true
                    Layout.fillWidth: true
                }

                Label {
                    text: "Grelha: " + gameController.rows + "x" + gameController.cols
                    color: "#CBD5E1"
                    font.pixelSize: 14
                }

                Button {
                    text: "Restart"
                    onClicked: {
                        selectedBusIndex = -1;
                        gameController.restartPreview();
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 16

            Rectangle {
                id: boardFrame
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 16
                color: "#E2E8F0"
                border.color: "#94A3B8"
                border.width: 1

                Item {
                    id: boardArea
                    anchors.fill: parent
                    anchors.margins: 10

                    Rectangle {
                        id: boardBackground
                        width: window.boardPixelWidth
                        height: window.boardPixelHeight
                        anchors.centerIn: parent
                        color: "#F8FAFC"
                        border.color: "#94A3B8"
                        border.width: 1
                        radius: 8
                    }

                    Repeater {
                        model: gameController.rows * gameController.cols
                        delegate: Rectangle {
                            required property int index
                            readonly property int col: index % gameController.cols
                            readonly property int row: Math.floor(index / gameController.cols)
                            x: boardBackground.x + col * window.boardCellSize
                            y: boardBackground.y + row * window.boardCellSize
                            width: window.boardCellSize
                            height: window.boardCellSize
                            color: (row + col) % 2 === 0 ? "#EEF2FF" : "#E2E8F0"
                            border.color: "#CBD5E1"
                            border.width: 0.6
                        }
                    }

                    Repeater {
                        model: gameController.buses
                        delegate: Rectangle {
                            required property var modelData
                            required property int index
                            property var bus: modelData
                            readonly property bool horizontal: bus.orientation === "horizontal"
                            x: boardBackground.x + bus.col * window.boardCellSize + 2
                            y: boardBackground.y + bus.row * window.boardCellSize + 2
                            width: (horizontal ? bus.lengthCells : 1) * window.boardCellSize - 4
                            height: (horizontal ? 1 : bus.lengthCells) * window.boardCellSize - 4
                            radius: 10
                            color: bus.color
                            border.color: window.selectedBusIndex === index ? "#111827" : "#F8FAFC"
                            border.width: window.selectedBusIndex === index ? 3 : 1.3
                            antialiasing: true

                            Label {
                                anchors.centerIn: parent
                                text: bus.capacity + "/0"
                                color: "white"
                                font.bold: true
                                font.pixelSize: Math.max(12, window.boardCellSize * 0.28)
                            }

                            Label {
                                text: window.directionArrow(bus.direction)
                                color: "white"
                                font.pixelSize: Math.max(16, window.boardCellSize * 0.35)
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.right: parent.right
                                anchors.rightMargin: 8
                            }

                            TapHandler {
                                onTapped: window.selectedBusIndex = index
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 260
                Layout.fillHeight: true
                radius: 16
                color: "#FFFFFF"
                border.color: "#CBD5E1"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    Label {
                        text: "Plataformas"
                        font.bold: true
                        font.pixelSize: 19
                    }

                    Label {
                        text: "Cada slot aceita um autocarro da mesma cor."
                        wrapMode: Text.WordWrap
                        color: "#475569"
                        font.pixelSize: 13
                    }

                    Repeater {
                        model: gameController.platforms
                        delegate: Rectangle {
                            required property var modelData
                            property var platform: modelData
                            Layout.fillWidth: true
                            Layout.preferredHeight: 50
                            radius: 10
                            color: Qt.alpha(platform.color, 0.18)
                            border.color: platform.color
                            border.width: 2

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 10

                                Rectangle {
                                    width: 18
                                    height: 18
                                    radius: 9
                                    color: platform.color
                                }

                                Label {
                                    text: "Slot " + platform.index
                                    Layout.fillWidth: true
                                    font.bold: true
                                }

                                Label {
                                    text: platform.occupied ? "Ocupado" : "Livre"
                                    color: platform.occupied ? "#B91C1C" : "#166534"
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }

                    Label {
                        text: "Parte de hoje: UI inicial + dados mockados em C++."
                        wrapMode: Text.WordWrap
                        color: "#334155"
                        font.pixelSize: 12
                    }
                }
            }
        }
    }
}

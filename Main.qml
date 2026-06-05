import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: window
    width: 1120
    height: 720
    visible: true
    title: "Park Out"
    color: "#F3F4F6"

    property int selectedBusIndex: -1
    property int timeElapsed: 0
    property bool gameStarted: false

    readonly property var selectedBus: (selectedBusIndex >= 0 && selectedBusIndex < gameController.buses.length) ? gameController.buses[selectedBusIndex] : null
    readonly property bool hasSelection: selectedBus !== null

    readonly property real boardCellSize: Math.max(24, Math.floor(Math.min((boardArea.width - 24) / gameController.cols, (boardArea.height - 24) / gameController.rows)))
    readonly property real boardPixelWidth: boardCellSize * gameController.cols
    readonly property real boardPixelHeight: boardCellSize * gameController.rows

    function directionArrow(direction) {
        switch (direction) {
            case "right": return "▶";
            case "left": return "◀";
            case "up": return "▲";
            case "down": return "▼";
            default: return "•";
        }
    }

    Timer {
        id: gameTimer
        interval: 1000
        running: window.gameStarted && !gameController.levelCompleted && !gameController.isGenerating
        repeat: true
        onTriggered: window.timeElapsed++
    }

    Connections {
        target: gameController
        function onCurrentLevelChanged() { window.timeElapsed = 0; window.gameStarted = true; window.selectedBusIndex = -1; }
        function onLevelCompletedChanged() { if (gameController.levelCompleted) window.gameStarted = false; }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        // Painel Superior de Níveis
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 90
            radius: 12
            color: "#1E293B"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 12

                ColumnLayout {
                    Layout.fillWidth: true
                    RowLayout {
                        spacing: 10
                        Button { text: "Nível 1"; highlighted: gameController.currentLevel === 1; onClicked: gameController.loadLevel(1) }
                        Button { text: "Nível 2"; highlighted: gameController.currentLevel === 2; onClicked: gameController.loadLevel(2) }
                        Button { text: "Nível 3"; highlighted: gameController.currentLevel === 3; onClicked: gameController.loadLevel(3) }
                        Button { text: "Gerar Nível ⚙️"; highlighted: gameController.currentLevel === 99; onClicked: gameController.generateProceduralLevelAsync() }
                    }
                }

                Label { text: "Tempo: " + window.timeElapsed + "s"; color: "#CBD5E1"; font.bold: true }
                Label { text: "Movimentos: " + gameController.moveCount; color: "#CBD5E1"; font.bold: true }
                Label { text: gameController.bestScore > 0 ? "🏆 Melhor: " + gameController.bestScore : ""; color: "#FDE68A"; font.bold: true }

                Button { text: "Desfazer ↩️"; enabled: gameController.canUndo; onClicked: gameController.undoLastMove() }
                Button { text: "Reiniciar"; onClicked: gameController.restartLevel() }
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

                Item {
                    id: boardArea
                    anchors.fill: parent
                    anchors.margins: 12

                    Rectangle {
                        id: boardBackground
                        width: window.boardPixelWidth
                        height: window.boardPixelHeight
                        anchors.centerIn: parent
                        color: "#F8FAFC"
                        border.color: "#94A3B8"
                        radius: 8

                        Repeater {
                            model: gameController.rows * gameController.cols
                            delegate: Rectangle {
                                required property int index
                                width: window.boardCellSize; height: window.boardCellSize
                                x: (index % gameController.cols) * window.boardCellSize
                                y: Math.floor(index / gameController.cols) * window.boardCellSize
                                color: (Math.floor(index / gameController.cols) + (index % gameController.cols)) % 2 === 0 ? "#EEF2FF" : "#E2E8F0"
                                border.color: "#CBD5E1"
                                border.width: 0.5
                            }
                        }

                        Repeater {
                            model: gameController.buses
                            delegate: Rectangle {
                                required property var modelData
                                required property int index
                                property var bus: modelData
                                readonly property bool isH: bus.orientation === "horizontal"

                                x: bus.col * window.boardCellSize + 2
                                y: bus.row * window.boardCellSize + 2
                                width: (isH ? bus.lengthCells : 1) * window.boardCellSize - 4
                                height: (isH ? 1 : bus.lengthCells) * window.boardCellSize - 4
                                radius: 6
                                color: bus.color
                                border.color: window.selectedBusIndex === index ? "#0F172A" : "#00000000"
                                border.width: 3

                                Label { anchors.centerIn: parent; text: bus.capacity + " Lugares"; color: "white"; font.bold: true; font.pixelSize: 11 }
                                Label { text: window.directionArrow(bus.direction); color: "white"; anchors.right: parent.right; anchors.rightMargin: 4; anchors.verticalCenter: parent.verticalCenter; visible: isH && bus.direction === "right" }
                                Label { text: window.directionArrow(bus.direction); color: "white"; anchors.left: parent.left; anchors.leftMargin: 4; anchors.verticalCenter: parent.verticalCenter; visible: isH && bus.direction === "left" }
                                Label { text: window.directionArrow(bus.direction); color: "white"; anchors.top: parent.top; anchors.topMargin: 2; anchors.horizontalCenter: parent.horizontalCenter; visible: !isH && bus.direction === "up" }
                                Label { text: window.directionArrow(bus.direction); color: "white"; anchors.bottom: parent.bottom; anchors.bottomMargin: 2; anchors.horizontalCenter: parent.horizontalCenter; visible: !isH && bus.direction === "down" }

                                TapHandler { onTapped: window.selectedBusIndex = index }
                            }
                        }
                    }

                    Rectangle {
                        anchors.fill: parent; color: "#DD1E293B"; visible: gameController.isGenerating; radius: 12
                        Label { anchors.centerIn: parent; text: "A gerar puzzle proceduralmente..."; color: "white"; font.bold: true; font.pixelSize: 16 }
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 300
                Layout.fillHeight: true
                radius: 16; color: "white"; border.color: "#CBD5E1"

                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 14; spacing: 12

                    Label { text: "Plataformas"; font.bold: true; font.pixelSize: 16 }

                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 4
                        Repeater {
                            model: gameController.platforms
                            delegate: Rectangle {
                                required property var modelData
                                Layout.fillWidth: true; Layout.preferredHeight: 40; radius: 6
                                color: modelData.occupied ? modelData.color : Qt.alpha(modelData.color, 0.15)
                                border.color: modelData.color; border.width: 1.5
                                RowLayout {
                                    anchors.fill: parent; anchors.margins: 8
                                    Label { text: "Slot " + modelData.index; font.bold: true; color: modelData.occupied ? "white" : "#1E293B"; Layout.fillWidth: true }
                                    Label { text: modelData.occupied ? "CHEIO" : "LIVRE"; font.bold: true; color: modelData.occupied ? "white" : "#166534" }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true; Layout.fillHeight: true; color: "#F8FAFC"; radius: 8; border.color: "#E2E8F0"
                        ColumnLayout {
                            anchors.fill: parent; anchors.margins: 12
                            Label { text: window.hasSelection ? "Configuração: " + window.selectedBus.orientation + "\nSentido: " + window.selectedBus.direction : "Seleciona um veículo."; color: "#64748B"; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                            Item { Layout.fillHeight: true }
                            Button {
                                text: "Avançar para Plataforma 🚀"
                                Layout.fillWidth: true; highlighted: true
                                enabled: window.hasSelection && !gameController.levelCompleted
                                onClicked: gameController.moveBusToPlatform(window.selectedBusIndex)
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 70; color: "#0F172A"; radius: 8
                        ColumnLayout {
                            anchors.fill: parent; anchors.margins: 8
                            Label { text: "Consola:"; color: "#64748B"; font.pixelSize: 10 }
                            Label { text: gameController.statusMessage; color: "white"; font.pixelSize: 11; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        }
                    }
                }
            }
        }
    }
}
#include "gamecontroller.h"
#include <QtConcurrent>
#include <QRandomGenerator>
#include <QPoint>
#include <algorithm> // Necessário para std::reverse e std::swap

namespace {
QVariantMap makeBus(int row, int col, int lengthCells, int capacity, const QString &orientation, const QString &direction, const QString &color) {
    return QVariantMap{{"row", row}, {"col", col}, {"lengthCells", lengthCells}, {"capacity", capacity}, {"orientation", orientation}, {"direction", direction}, {"color", color}};
}

QVariantMap makePlatform(int index, const QString &color, bool occupied) {
    return QVariantMap{{"index", index}, {"color", color}, {"occupied", occupied}};
}
}

GameController::GameController(QObject *parent) : QObject(parent) {
    // Configura o observador da thread assíncrona (Geração Procedural)
    connect(&m_watcher, &QFutureWatcher<QVariantList>::finished, this, [this]() {
        m_buses = m_watcher.result();
        m_isGenerating = false;
        m_levelCompleted = false;
        m_moveCount = 0;
        m_exitedBuses = 0;
        m_undoHistory.clear();

        m_initialLevelState = GameState{m_buses, m_platforms, m_moveCount, m_exitedBuses};
        loadBestScore();

        setStatusMessage("Nível procedural gerado via Assincronismo.");
        emit isGeneratingChanged();
        emit busesChanged(); emit moveCountChanged(); emit exitedBusesChanged(); emit levelCompletedChanged(); emit canUndoChanged();
    });

    loadLevel(1);
}

int GameController::rows() const { return m_rows; }
int GameController::cols() const { return m_cols; }
QVariantList GameController::buses() const { return m_buses; }
QVariantList GameController::platforms() const { return m_platforms; }
QString GameController::statusMessage() const { return m_statusMessage; }
int GameController::moveCount() const { return m_moveCount; }
int GameController::exitedBuses() const { return m_exitedBuses; }
bool GameController::levelCompleted() const { return m_levelCompleted; }
int GameController::currentLevel() const { return m_currentLevel; }
bool GameController::isGenerating() const { return m_isGenerating; }
bool GameController::canUndo() const { return !m_undoHistory.isEmpty(); }
int GameController::bestScore() const { return m_bestScore; }

void GameController::saveCurrentStateToHistory() {
    m_undoHistory.append(GameState{m_buses, m_platforms, m_moveCount, m_exitedBuses});
    emit canUndoChanged();
}

void GameController::undoLastMove() {
    if (m_undoHistory.isEmpty()) return;
    GameState state = m_undoHistory.takeLast();
    m_buses = state.buses; m_platforms = state.platforms; m_moveCount = state.moveCount; m_exitedBuses = state.exitedBuses;
    m_levelCompleted = false;
    setStatusMessage("Movimento desfeito via Histórico Imutável.");
    emit busesChanged(); emit platformsChanged(); emit moveCountChanged(); emit exitedBusesChanged(); emit levelCompletedChanged(); emit canUndoChanged();
}

void GameController::restartLevel() {
    m_buses = m_initialLevelState.buses;
    m_platforms = m_initialLevelState.platforms;
    m_moveCount = m_initialLevelState.moveCount;
    m_exitedBuses = m_initialLevelState.exitedBuses;
    m_levelCompleted = false;
    m_statusMessage = "Nível reiniciado.";
    m_undoHistory.clear();
    emit busesChanged(); emit platformsChanged(); emit moveCountChanged(); emit exitedBusesChanged(); emit levelCompletedChanged(); emit canUndoChanged(); emit statusMessageChanged();
}

void GameController::loadBestScore() {
    QSettings settings("Autonoma", "ParkOut");
    m_bestScore = settings.value(QString("level_%1_best").arg(m_currentLevel), 0).toInt();
    emit bestScoreChanged();
}

void GameController::saveProgress() {
    QSettings settings("Autonoma", "ParkOut");
    QString key = QString("level_%1_best").arg(m_currentLevel);
    int currentBest = settings.value(key, 9999).toInt();
    if (currentBest == 0 || m_moveCount < currentBest) {
        settings.setValue(key, m_moveCount);
        loadBestScore();
    }
}

bool GameController::checkPathToExitClear(const QVariantMap &bus, int busIndex) const {
    int row = bus.value("row").toInt(), col = bus.value("col").toInt(), len = bus.value("lengthCells").toInt();
    QString orient = bus.value("orientation").toString(), dir = bus.value("direction").toString();

    if (orient == "horizontal") {
        if (dir == "right") { for (int c = col + len; c < m_cols; ++c) if (isCellOccupied(row, c, busIndex)) return false; }
        else { for (int c = col - 1; c >= 0; --c) if (isCellOccupied(row, c, busIndex)) return false; }
    } else {
        if (dir == "down") { for (int r = row + len; r < m_rows; ++r) if (isCellOccupied(r, col, busIndex)) return false; }
        else { for (int r = row - 1; r >= 0; --r) if (isCellOccupied(r, col, busIndex)) return false; }
    }
    return true;
}

bool GameController::moveBusToPlatform(int busIndex) {
    if (busIndex < 0 || busIndex >= m_buses.size() || m_levelCompleted) return false;
    QVariantMap bus = m_buses[busIndex].toMap();

    if (!checkPathToExitClear(bus, busIndex)) {
        setStatusMessage("Caminho bloqueado por outro veículo!");
        return false;
    }

    const int platformIndex = matchingFreePlatformIndex(bus.value("color").toString());
    if (platformIndex < 0) {
        setStatusMessage("Impossível sair: Não há plataforma livre com a mesma cor.");
        return false;
    }

    saveCurrentStateToHistory(); // Grava estado imutável antes da alteração

    QVariantMap platform = m_platforms[platformIndex].toMap();
    platform["occupied"] = true;
    m_platforms[platformIndex] = platform;

    m_buses.removeAt(busIndex);
    ++m_moveCount;
    ++m_exitedBuses;

    setStatusMessage("Autocarro movimentou-se diretamente para o embarque.");

    if (m_buses.isEmpty()) {
        m_levelCompleted = true;
        setStatusMessage("Nível Concluído!");
        saveProgress();
    }

    emit busesChanged(); emit platformsChanged(); emit moveCountChanged(); emit exitedBusesChanged(); emit levelCompletedChanged();
    return true;
}

void GameController::loadLevel(int levelIndex) {
    m_currentLevel = levelIndex;
    m_undoHistory.clear();

    m_platforms = QVariantList{
        makePlatform(1, "#C62828", false), makePlatform(2, "#2E7D32", false),
        makePlatform(3, "#F9A825", false), makePlatform(4, "#1565C0", false),
        makePlatform(5, "#8E24AA", false), makePlatform(6, "#EF6C00", false),
    };

    if (levelIndex == 1) {
        m_buses = QVariantList{makeBus(0, 0, 4, 12, "horizontal", "right", "#C62828"), makeBus(1, 3, 3, 8, "vertical", "down", "#2E7D32"), makeBus(2, 1, 2, 6, "horizontal", "left", "#F9A825"), makeBus(4, 6, 3, 8, "vertical", "up", "#1565C0")};
    } else if (levelIndex == 2) {
        m_buses = QVariantList{makeBus(2, 2, 3, 8, "horizontal", "right", "#EF6C00"), makeBus(0, 5, 4, 12, "vertical", "down", "#C62828"), makeBus(5, 1, 2, 4, "vertical", "up", "#8E24AA"), makeBus(6, 3, 3, 8, "horizontal", "left", "#1565C0")};
    } else {
        m_buses = QVariantList{makeBus(1, 1, 4, 12, "horizontal", "right", "#2E7D32"), makeBus(3, 4, 2, 6, "vertical", "down", "#F9A825"), makeBus(0, 6, 3, 8, "vertical", "down", "#C62828"), makeBus(5, 2, 3, 8, "horizontal", "left", "#8E24AA")};
    }

    m_moveCount = 0; m_exitedBuses = 0; m_levelCompleted = false;
    m_initialLevelState = GameState{m_buses, m_platforms, m_moveCount, m_exitedBuses};
    setStatusMessage("Nível " + QString::number(levelIndex) + " carregado.");

    loadBestScore();

    emit currentLevelChanged(); emit busesChanged(); emit platformsChanged(); emit moveCountChanged(); emit exitedBusesChanged(); emit levelCompletedChanged(); emit canUndoChanged();
}

void GameController::generateProceduralLevelAsync() {
    if (m_isGenerating) return;
    m_isGenerating = true; m_currentLevel = 99;
    emit isGeneratingChanged(); emit currentLevelChanged();
    setStatusMessage("A correr algoritmo da Espiral em thread separada...");
    QFuture<QVariantList> future = QtConcurrent::run(&GameController::generateSpiralPuzzle, this, m_rows, m_cols);
    m_watcher.setFuture(future);
}

QVariantList GameController::generateSpiralPuzzle(int rows, int cols) {
    QVariantList puzzle;
    QVector<QPoint> order;
    QVector<QVector<bool>> grid(rows, QVector<bool>(cols, false));

    // 1. Gerar o caminho da Espiral (do centro para fora)
    int r_curr = 0, c_curr = 0, dr = 0, dc = 1;
    for (int i = 0; i < rows * cols; ++i) {
        order.append(QPoint(c_curr, r_curr));
        grid[r_curr][c_curr] = true;
        int nr = r_curr + dr, nc = c_curr + dc;
        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !grid[nr][nc]) {
            r_curr = nr; c_curr = nc;
        } else {
            int temp = dr; dr = dc; dc = -temp;
            r_curr += dr; c_curr += dc;
        }
    }
    std::reverse(order.begin(), order.end()); // Inverte para começar no centro

    // Limpar a grelha para iniciar o preenchimento real
    for (int r = 0; r < rows; ++r) grid[r].fill(false);

    // 2. Preparar Cores Únicas
    QVector<QString> colors = {"#C62828", "#2E7D32", "#F9A825", "#1565C0", "#8E24AA", "#EF6C00"};
    for (int i = colors.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        std::swap(colors[i], colors[j]); // Embaralha as cores
    }
    int colorIndex = 0;

    // 3. Configurações possíveis de autocarros
    QVector<int> sizes = {3, 2, 4}; // Tenta primeiro os médios, depois pequenos, depois grandes

    struct DirOpt { QString orient; QString dir; };
    QVector<DirOpt> dirs = {
        {"horizontal", "right"},
        {"horizontal", "left"},
        {"vertical", "down"},
        {"vertical", "up"}
    };

    // 4. Percorrer a espiral e tentar encaixar
    for (const QPoint &p : order) {
        if (puzzle.size() >= colors.size()) break; // Máximo 6 autocarros para as 6 plataformas

        int r = p.y(), c = p.x();
        if (grid[r][c]) continue; // Se a célula já estiver ocupada, ignora

        // Embaralha a ordem das direções para ficar um puzzle imprevisível
        for (int i = dirs.size() - 1; i > 0; --i) {
            int j = QRandomGenerator::global()->bounded(i + 1);
            std::swap(dirs[i], dirs[j]);
        }

        bool placed = false;

        for (int len : sizes) {
            for (const auto &d : dirs) {
                bool fits = true;
                int startR = r, startC = c;

                // Ajusta a posição de "desenho" no QML consoante a direção de saída
                if (d.orient == "horizontal" && d.dir == "left") startC = c - len + 1;
                else if (d.orient == "vertical" && d.dir == "up") startR = r - len + 1;

                // Valida se sai fora dos limites do tabuleiro
                if (startR < 0 || startR + (d.orient == "vertical" ? len : 1) > rows ||
                    startC < 0 || startC + (d.orient == "horizontal" ? len : 1) > cols) {
                    continue;
                }

                // Valida colisões com outros veículos
                for (int i = 0; i < len; ++i) {
                    int checkR = startR + (d.orient == "vertical" ? i : 0);
                    int checkC = startC + (d.orient == "horizontal" ? i : 0);
                    if (grid[checkR][checkC]) { fits = false; break; }
                }

                if (fits) {
                    // Preenche as células no mapa de ocupação
                    for (int i = 0; i < len; ++i) {
                        int checkR = startR + (d.orient == "vertical" ? i : 0);
                        int checkC = startC + (d.orient == "horizontal" ? i : 0);
                        grid[checkR][checkC] = true;
                    }

                    // Mapeia tamanho com capacidade (Regra de negócio)
                    int capacity = (len == 2) ? 6 : (len == 3) ? 8 : 12;

                    puzzle.append(makeBus(startR, startC, len, capacity, d.orient, d.dir, colors[colorIndex++]));
                    placed = true;
                    break;
                }
            }
            if (placed) break; // Passa para a próxima célula da espiral
        }
    }

    return puzzle;
}

bool GameController::isCellOccupied(int row, int col, int ignoreBusIndex) const {
    for (int i = 0; i < m_buses.size(); ++i) {
        if (i == ignoreBusIndex) continue;
        QVariantMap bus = m_buses[i].toMap();
        int bRow = bus.value("row").toInt(), bCol = bus.value("col").toInt(), len = bus.value("lengthCells").toInt();
        QString orient = bus.value("orientation").toString();
        for (int step = 0; step < len; ++step) {
            int tRow = bRow, tCol = bCol;
            if (orient == "horizontal") tCol += step; else tRow += step;
            if (tRow == row && tCol == col) return true;
        }
    }
    return false;
}

int GameController::matchingFreePlatformIndex(const QString &color) const {
    for (int i = 0; i < m_platforms.size(); ++i) {
        QVariantMap plat = m_platforms[i].toMap();
        if (plat.value("color").toString() == color && !plat.value("occupied").toBool()) return i;
    }
    return -1;
}

void GameController::setStatusMessage(const QString &message) {
    if (m_statusMessage != message) { m_statusMessage = message; emit statusMessageChanged(); }
}
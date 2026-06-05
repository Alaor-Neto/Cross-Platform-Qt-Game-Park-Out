#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>
#include <QFutureWatcher>
#include <QSettings>

// Paradigma Funcional: Snapshot de estado imutável
struct GameState {
    QVariantList buses;
    QVariantList platforms;
    int moveCount;
    int exitedBuses;
};

class GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows NOTIFY rowsChanged)
    Q_PROPERTY(int cols READ cols NOTIFY colsChanged)
    Q_PROPERTY(QVariantList buses READ buses NOTIFY busesChanged)
    Q_PROPERTY(QVariantList platforms READ platforms NOTIFY platformsChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(int moveCount READ moveCount NOTIFY moveCountChanged)
    Q_PROPERTY(int exitedBuses READ exitedBuses NOTIFY exitedBusesChanged)
    Q_PROPERTY(bool levelCompleted READ levelCompleted NOTIFY levelCompletedChanged)
    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY currentLevelChanged)
    Q_PROPERTY(bool isGenerating READ isGenerating NOTIFY isGeneratingChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(int bestScore READ bestScore NOTIFY bestScoreChanged)

public:
    explicit GameController(QObject *parent = nullptr);

    int rows() const;
    int cols() const;
    QVariantList buses() const;
    QVariantList platforms() const;
    QString statusMessage() const;
    int moveCount() const;
    int exitedBuses() const;
    bool levelCompleted() const;
    int currentLevel() const;
    bool isGenerating() const;
    bool canUndo() const;
    int bestScore() const;

    Q_INVOKABLE void loadLevel(int levelIndex);
    Q_INVOKABLE void restartLevel();
    Q_INVOKABLE bool moveBusToPlatform(int busIndex);
    Q_INVOKABLE void undoLastMove();
    Q_INVOKABLE void generateProceduralLevelAsync();

signals:
    void rowsChanged();
    void colsChanged();
    void busesChanged();
    void platformsChanged();
    void statusMessageChanged();
    void moveCountChanged();
    void exitedBusesChanged();
    void levelCompletedChanged();
    void currentLevelChanged();
    void isGeneratingChanged();
    void canUndoChanged();
    void bestScoreChanged();

private:
    void saveCurrentStateToHistory();
    void saveProgress();
    void loadBestScore();
    bool checkPathToExitClear(const QVariantMap &bus, int busIndex) const;
    bool isCellOccupied(int row, int col, int ignoreBusIndex = -1) const;
    int matchingFreePlatformIndex(const QString &color) const;
    QVariantList generateSpiralPuzzle(int rows, int cols);

    // CORREÇÃO: A declaração que faltava para resolver o erro no compilador
    void setStatusMessage(const QString &message);

    int m_rows = 8;
    int m_cols = 8;
    int m_currentLevel = 1;
    int m_moveCount = 0;
    int m_exitedBuses = 0;
    int m_bestScore = 0;
    bool m_levelCompleted = false;
    bool m_isGenerating = false;
    QString m_statusMessage;

    QVariantList m_buses;
    QVariantList m_platforms;

    QVector<GameState> m_undoHistory;
    GameState m_initialLevelState;
    QFutureWatcher<QVariantList> m_watcher;
};

#endif // GAMECONTROLLER_H
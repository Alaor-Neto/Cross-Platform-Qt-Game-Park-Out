#include "gamecontroller.h"

#include <QVariantMap>

namespace {
QVariantMap makeBus(int row,
                    int col,
                    int lengthCells,
                    int capacity,
                    const QString &orientation,
                    const QString &direction,
                    const QString &color)
{
    return QVariantMap{
        {"row", row},
        {"col", col},
        {"lengthCells", lengthCells},
        {"capacity", capacity},
        {"orientation", orientation},
        {"direction", direction},
        {"color", color},
    };
}

QVariantMap makePlatform(int index, const QString &color, bool occupied)
{
    return QVariantMap{
        {"index", index},
        {"color", color},
        {"occupied", occupied},
    };
}
} // namespace

GameController::GameController(QObject *parent)
    : QObject(parent)
{
    loadPreviewLevel();
}

int GameController::rows() const
{
    return m_rows;
}

int GameController::cols() const
{
    return m_cols;
}

QVariantList GameController::buses() const
{
    return m_buses;
}

QVariantList GameController::platforms() const
{
    return m_platforms;
}

void GameController::restartPreview()
{
    loadPreviewLevel();
}

void GameController::loadPreviewLevel()
{
    m_buses = QVariantList{
        makeBus(0, 0, 4, 12, "horizontal", "right", "#C62828"),
        makeBus(1, 3, 3, 8, "vertical", "down", "#2E7D32"),
        makeBus(2, 1, 2, 6, "horizontal", "left", "#F9A825"),
        makeBus(3, 6, 3, 8, "vertical", "up", "#1565C0"),
        makeBus(5, 2, 2, 4, "vertical", "up", "#8E24AA"),
        makeBus(6, 4, 3, 8, "horizontal", "right", "#EF6C00"),
    };

    m_platforms = QVariantList{
        makePlatform(1, "#C62828", false),
        makePlatform(2, "#2E7D32", false),
        makePlatform(3, "#1565C0", false),
        makePlatform(4, "#8E24AA", false),
    };

    emit busesChanged();
    emit platformsChanged();
}

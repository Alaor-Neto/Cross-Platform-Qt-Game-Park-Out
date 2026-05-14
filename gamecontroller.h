#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QVariantList>

class GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int rows READ rows CONSTANT)
    Q_PROPERTY(int cols READ cols CONSTANT)
    Q_PROPERTY(QVariantList buses READ buses NOTIFY busesChanged)
    Q_PROPERTY(QVariantList platforms READ platforms NOTIFY platformsChanged)

public:
    explicit GameController(QObject *parent = nullptr);

    int rows() const;
    int cols() const;
    QVariantList buses() const;
    QVariantList platforms() const;

    Q_INVOKABLE void restartPreview();

signals:
    void busesChanged();
    void platformsChanged();

private:
    void loadPreviewLevel();

    int m_rows = 8;
    int m_cols = 8;
    QVariantList m_buses;
    QVariantList m_platforms;
};

#endif // GAMECONTROLLER_H

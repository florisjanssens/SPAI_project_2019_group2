#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QWidget>
#include <QtMultimedia>
#include <QByteArray>
#include <QtEndian>
#include <QDebug>
#include <QtMath>
#include <QObject>
#include <QScopedPointer>
#include <QIODevice>
#include "wavfile.h"

#include "udpclient.h"

class AudioInfo : public QIODevice
{
    Q_OBJECT

public:
    AudioInfo();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

signals:
    void update();
};

class AudioIN : public QObject
{
    Q_OBJECT

public:
    AudioIN();
    ~AudioIN();
    QAudioInput * audioInput() const;

signals:
    void warningMessage(const QString &message);

public slots:
    void sendAudio(std::shared_ptr<UDPClient> client, QHostAddress address, quint16 port);

private:
    QByteArray received;

    QScopedPointer<AudioInfo> m_audioInfo;
    QScopedPointer<QAudioInput> m_audioInput;
    bool started = false;
};

#endif // AUDIO_H




#ifndef AUDIOOUT_H
#define AUDIOOUT_H

#include <memory>
#include "udpserver.h"
#include <QtMultimedia>

class AudioOUT : public QObject
{
    Q_OBJECT
public:
    AudioOUT();
    ~AudioOUT();
    std::shared_ptr<QAudioOutput> getAudio() const;

    std::shared_ptr<QAudioDeviceInfo> getInfo() const;

public slots:
    void playMessage(QByteArray sound);

signals:
    void cancel();

private:
    std::shared_ptr<QByteArray> receiveArray = nullptr;
    std::shared_ptr<QAudioOutput> audio = nullptr;
    std::shared_ptr<QAudioDeviceInfo> info = nullptr;
    QIODevice * dev;
};

#endif // AUDIOOUT_H

#include "audioin.h"

/************************************************* AUDIOINFO *************************************************/
/*
 * Class that implements the abstract QIODevice functionality - mainly read and write
 */
AudioInfo::AudioInfo()
{
}

void AudioInfo::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInfo::stop()
{
    close();
}

// These need to be implemented in order to work as a QIODevice

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)

    return 0;
}

/************************************************* AUDIO *************************************************/
AudioIN::AudioIN()
{
    auto deviceInfo = QAudioDeviceInfo::defaultInputDevice();

    // The same format needs to be chosen for both devices (sever and client)
    QAudioFormat format;
    // 8 kHz is commonly chosen for VoIP applications which are comparable
    // to this project. Can be put higher but just consumes more bandwidth.
    // For this application, having a very high sampling rate also doesn't
    // really matter (it's just speech anyway).
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    if (!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        format = deviceInfo.nearestFormat(format);
    }

    m_audioInfo.reset(new AudioInfo());

    m_audioInput.reset(new QAudioInput(deviceInfo, format));
    // On Windows, the buffersize needs to be different because of a
    // Windows specific bug in QAudioInput
    #ifdef Q_OS_WIN
    m_audioInput->setBufferSize(2048);
    #else
    m_audioInput->setBufferSize(512);
    #endif

    m_audioInfo->start();
}
AudioIN::~AudioIN()
{
}
/*
 * The data incoming to the device is collected in the
 * buffer and send as a datagram to the UDP socket on the
 * server device.
 */
void AudioIN::sendAudio(std::shared_ptr<UDPClient> client, QHostAddress address, quint16 port)
{
    auto io = m_audioInput->start();
    connect(io, &QIODevice::readyRead,
            [&, io, address, port, client]() {
        int len = m_audioInput->bytesReady();
        const int bufferSize = 512;
        if (len >= 0)
        {
            len = bufferSize;
            QByteArray block = io->readAll();
            received.append(block);
            while(received.length() >= bufferSize)
            {
                if (!client->sendDatagram(received.left(bufferSize), address, port)) emit warningMessage("Datagram has not been sent.");
                received.remove(0, bufferSize);
            }
        }
    });
}

QAudioInput * AudioIN::audioInput() const
{
    return m_audioInput.get();
}

#include "audioout.h"

AudioOUT::AudioOUT()
{
    // Set up the format, eg.
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

    info = std::make_shared<QAudioDeviceInfo>(QAudioDeviceInfo::defaultOutputDevice());

    audio = std::make_shared<QAudioOutput>(format, this);
    audio->setBufferSize(2048);

    dev = audio->start();
}

/*
 * This function takes the received data and
 * output them into the audio output device.
 */
void AudioOUT::playMessage(QByteArray &sound)
{
    dev->write(sound.data(), sound.size());
}

std::shared_ptr<QAudioDeviceInfo> AudioOUT::getInfo() const
{
    return info;
}

std::shared_ptr<QAudioOutput> AudioOUT::getAudio() const
{
    return audio;
}


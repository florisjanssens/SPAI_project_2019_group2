% this file is used to add some noise to the audio signals. The color noise
% includes white, pink and blue noise, which should be changed for
% different output. The noises are also recorded to .wav file.
%% setting
clc; clear;
fileReader1=dsp.AudioFileReader('audio1.wav');
fileReader2=dsp.AudioFileReader('audio2.wav');
fileReader3=dsp.AudioFileReader('audio3.wav');
fileInfo = audioinfo('audio1.wav');
deviceWriter = audioDeviceWriter('SampleRate',fileInfo.SampleRate);
NoiseFileWriter=dsp.AudioFileWriter('noise_blue.wav');
AudioNoiseFileWriter1=dsp.AudioFileWriter('AudioNoise1_blue.wav');
AudioNoiseFileWriter2=dsp.AudioFileWriter('AudioNoise2_blue.wav');
AudioNoiseFileWriter3=dsp.AudioFileWriter('AudioNoise3_blue.wav');
cn =dsp.ColoredNoise(0,1024,'Color','blue');%set the color noise, other options are "white" and "blue"

%% processing data
while ~isDone(fileReader1)
    audioData1 = fileReader1();
    audioData2 = fileReader2();
    audioData3 = fileReader3();
    noisyData = 0.001*cn();% add a color noise to the signal
    audionoisyData1 = audioData1+noisyData;% input signal (audio+noise)
    audionoisyData2 = audioData2+noisyData;
    audionoisyData3 = audioData3+noisyData;
    NoiseFileWriter(noisyData);% write the noise audio
    AudioNoiseFileWriter1(audionoisyData1);% write the speech signal with noise
    AudioNoiseFileWriter2(audionoisyData2);
    AudioNoiseFileWriter3(audionoisyData3);

end

release(fileReader1)
release(fileReader2)
release(fileReader3)
release(AudioNoiseFileWriter1)
release(AudioNoiseFileWriter2)
release(AudioNoiseFileWriter3)
release(NoiseFileWriter)
release(deviceWriter)
release(cn)
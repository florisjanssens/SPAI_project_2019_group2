% this file is used to add different white noise to the audio signals. The noises are also recorded to .wav file.
[speechSig,fs] = audioread('audiotest.wma');
speechSig = speechSig(:,1);
noise1 = 0.013*randn(length(speechSig),1);%3dB
noise2 = 0.007*randn(length(speechSig),1);%7dB
noise3 = 0.024*randn(length(speechSig),1);%1dB
SNR1=snr(speechSig+noise1,noise1)
SNR2=snr(speechSig+noise2,noise2)
SNR3=snr(speechSig+noise3,noise3)
audiowrite('noise_3dB.wav',noise1,fs);
audiowrite('noise_7dB.wav',noise2,fs);
audiowrite('noise_1dB.wav',noise3,fs);
audiowrite('audionoise_3dB.wav',noise1+speechSig,fs);
audiowrite('audionoise_7dB.wav',noise2+speechSig,fs);
audiowrite('audionoise_1dB.wav',noise3+speechSig,fs);
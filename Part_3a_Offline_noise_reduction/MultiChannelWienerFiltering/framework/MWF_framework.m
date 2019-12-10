% Framework for the Multichannel Wiener Filter Algorithm
% Created:          November 19, 2019
% Last update:      December 9, 2019
%
%
% SPAI-Project 2019: 
%       PART 3A:    Offline noise reduction algorithm using MWF
%
close all; clear; clc;
%% Configuration:
addpath(genpath(fullfile('..')));
% Read in the audio file and retrieve the number of microphones
% [y_TD_MC, fs] = audioread('../Audio/part3a_pd.wav');     
[y_TD_MC, fs] = audioread('noisy_speech_s0_n30_multi.wav');
M = size(y_TD_MC,2);

%% Time-allign the different signals:
delays = zeros(M,1);
for i = 1:M
    delays(i) = finddelay(y_TD_MC(:,1), y_TD_MC(:,i));
    y_TD_MC(:,i) = [y_TD_MC(1:end-delays(i),i); zeros(delays(i),1)];
end

%% STFT domain: Weighted Overlap Add method
%%%%%%% WOLA parameters %%%%%%%
N_fft = 512;                        % number of FFT points
R_fft = N_fft/2;                    % shifting (50% overlap)
win = sqrt(hann(N_fft,'periodic')); % analysis window
N_half = floor(N_fft/2)+1;          % number of bins in onsided FFT 
freqs = 0:fs/N_fft:fs/2;            % one-sided set of actual frequencies

% Compute STFT 
y_STFT = calc_STFT(y_TD_MC, fs, win, N_fft, N_fft/R_fft, 'onesided');

% Compute SPP on reference microphone (first signal)
[noisePowMat, SPP] = spp_calc(y_TD_MC(:,1),N_fft,R_fft);
[N_freqs, N_frames] = size(y_STFT(:,:,1)); 

%% Define constants for MWF computation:
% Single Channel speech estimate
sig_s = zeros(N_freqs,N_frames);    % Speech estimate (both single and multi-channel)
sig_n = zeros(N_freqs,N_frames);    % noise estimate (single channel only)
Xi_min = 0.5;                       % Scaling for setting minimum value of speech power
beta_s = 0.4;                       % Forgetting factor for speech   
alpha_n = 0.9;                      % Forgetting factors for noise

n_scale = 1e-3;                     % initialization of noise correlation matrix inverse
                                
Rnn = cell(N_freqs,N_frames);  Rnn(:) = {eye(M,M)*n_scale};      % Noise Only (NO) corr. matrix. Inverse Initialize to zeros
Ryy = cell(N_freqs,N_frames);  Ryy(:) = {zeros(M)};              % Speech + Noise (SPN) corr. matrix. Inverse Initialize to zeros

lambda = 0.99;                      % Forgetting factor for computing correlation matrices - change values to observe effects on results                           
SPP_thr = 0.4;                      % Threshold for the SPP - also change values to observe effects on results

S_mc_stft = zeros(N_freqs,N_frames);% output speech estimate STFT domain 
e = zeros(M,1)';                    % Microphone Selection vector
e(1) = 1;
mvdr = zeros(M,N_freqs);
wf = zeros(N_freqs,1);

W_mc = (1/M)*ones(M,N_freqs);       % multi-channel filter
rtf_est = ones(M,N_freqs);           

%% MWF Framework: Compute the filter, filter the signal and save output 
tic
for l=2:N_frames    
    for k = 1:N_freqs % Freq index
        
        %%%%%%%%%%%%%----- SINGLE CHANNEL PROCESSING ----------%%%%%%%%%%%%
        % Use SPP thresholding to compute the noise power
        % Compute speech power using the decision directed (or any other approach) 
        if SPP(k,l) < SPP_thr % noise frame
            
              % update noise estimation based on SPP
              alpha_temp = alpha_n + (1-alpha_n)*SPP(k,l);
              sig_n(k,l) = alpha_temp*sig_n(k,l-1) +(1-alpha_temp)*(y_STFT(k,l,1)*conj(y_STFT(k,l,1))); 
        
        else   % Speech frame 
            sig_n(k,l) = sig_n(k,l-1);
            
            % Decision-Directed Approach for speech estimate
            sig_s(k,l) = max(beta_s* ( S_mc_stft(k,l-1) * conj( S_mc_stft(k,l-1) ) ) + ...
                        (1-beta_s)*((y_STFT(k,l,1)*conj(y_STFT(k,l,1)))- sig_n(k,l)), Xi_min*sig_n(k,l));
        end
        
        %%%%%%%%%%%%%----- MULTI-CHANNEL PROCESSING ----------%%%%%%%%%%%%
        
        % Use SPP thresholding to compute the Speech+Noise and Noise-Only Correlation matrices, i.e. Ryy and Rnn
        % Use either the a priori RTF vector or estimate the RTF vector
        % Compute the MVDR/MWF beamformer - you may also compute the MVDR beamformer first 
        % and then perform a single channel enhancement on the output from this.
        vec =reshape(y_STFT(k,l,:), [4 1]);
        vec_square = vec*vec';
        if SPP(k,l) >= SPP_thr
            Rnn{k,l} = Rnn{k,l-1};            
            Ryy{k,l} = lambda*Ryy{k,l-1} + (1-lambda)*vec_square;            
            rtf_est(:,k) = rtf_cov_sub(Ryy{k,l}, Rnn{k,l}, e);
        else
            if k > 1
                rtf_est(:,k) = rtf_est(:,k-1);
            end
            Rnn{k,l} = lambda*Rnn{k,l-1} + (1-lambda)*vec_square;
        end
        % Only start mvdr and wf after 10 time frames
        Rnn_inverse = Rnn{k,l}\eye(M);
        var_calc = rtf_est(:,k)' * Rnn_inverse * rtf_est(:,k);       
        mvdr(:,k) = ( Rnn_inverse *rtf_est(:,k) )/var_calc;
        wf(k) = sig_s(k,l)/(sig_s(k,l) + (1/var_calc));

        % Multi-channel coefficients
        W_mc(:,k) = mvdr(:,k)*wf(k);        
        S_mc_stft(k,l) = W_mc(:,k)'*vec; 
        
    end % end freqs
end % end time frames
toc

%% Show Multi Channel Enhancement:
times_stft = (1:N_frames);
figure;subplot(2,2,1); imagesc(times_stft,freqs,mag2db(abs(y_STFT(:,:,1))), [-65, 10]); colorbar; axis xy; set(gcf,'color','w');set(gca,'Fontsize',14); xlabel('Time (s)'), ylabel('Frequency (Hz)'), title('microphne signal, 1st mic');
subplot(2,2,2); imagesc(times_stft,freqs,SPP(:,:),[0 1]); colorbar; axis xy; set(gcf,'color','w'); set(gca,'Fontsize',14);xlabel('Time (s)'), ylabel('Frequency (Hz)'),title('SPP');
subplot(2,2,3); imagesc(times_stft,freqs,mag2db(abs(S_mc_stft(:,:))), [-65, 10]); colorbar; axis xy; set(gcf,'color','w'); set(gca,'Fontsize',14);xlabel('Time (s)'), ylabel('Frequency (Hz)'),title('Multi Channel Enhancement (SPP thr = 0.8)');

% compute the iSTFT - convert back to time domain (plot to also have a look)
time = 0:1/fs:(length(y_TD(:,1))-1)/fs;
s_mc_TD = calc_ISTFT(S_mc_stft, win, N_fft, N_fft/R_fft, 'onesided');
subplot(2,2,4); plot(time, s_mc_TD);

%% Write to audiofile
audiowrite('../audio_processed/part3_pd_enh.wav',s_mc_TD(:,1),fs);
sound(s_mc_TD(:,1),fs);
% Framework for the Multichannel Wiener Filter Algorithm
% Created:          November 19, 2019
% Last update:      December 9, 2019
%
%
% SPAI-Project 2019: 
%       PART 3A:    Offline noise reduction algorithm using MWF
%
function  [y_td_mc, y_noise_stft, SPP, noise_est, S_mc_stft, s_mc_TD, noise_est_enhanced, stopTime, times, freqs, fs] = MWF_framework(e_index, SPP_thr, Xi_min, beta_s, G_Dolby, F_Dolby)

% Configuration:
addpath(genpath(fullfile('../')));
N_fft = 512;                        % number of FFT points
% Check the number of wav files in audio directory
   audiodir = dir('../audio');
   audiodir = audiodir(3:end);
   N_Channels = size(audiodir,1);
   wav_files = strings(N_Channels,1);
   
   for i = 1:N_Channels
       wav_files(i) = "../audio/" + audiodir(i).name;
   end
   [y_sd1, fs] = audioread(wav_files(e_index));
   L = size(y_sd1,1);
   diff = N_fft - mod(L, N_fft);
   y_td_mc = zeros(L+diff, N_Channels);
   for i = 1:N_Channels
        
        if i == e_index
            y_td_mc(:,e_index) = [y_sd1 ; zeros(diff, 1)] ;
        else
            [y_current_td, fs] = audioread(wav_files(i));
            y_td_mc(:,i) = time_allign(y_td_mc(:,e_index), y_current_td);
        end
   end
M = N_Channels;
e = zeros(1,M);
e(e_index) = 1;

% STFT domain: Weighted Overlap Add method
%%%%%%% WOLA parameters %%%%%%%
R_fft = N_fft/2;                    % shifting (50% overlap)
win = sqrt(hann(N_fft,'periodic')); % analysis window
N_half = floor(N_fft/2)+1;          % number of bins in onsided FFT 
freqs = 0:fs/N_fft:fs/2;            % one-sided set of actual frequencies
times = [0:N_half/fs:L/fs L/fs];
% Compute STFT 
y_noise_stft = calc_STFT(y_td_mc, fs, win, N_fft, N_fft/R_fft, 'onesided');

% Compute SPP on reference microphone (first signal)
[noisePowMat, SPP] = spp_calc(y_td_mc(:,e_index),N_fft,R_fft);
% compute noise estimation for SNR
noise_est = calc_ISTFT(noisePowMat, win, N_fft, N_fft/R_fft, 'onesided');

[N_freqs, N_frames] = size(y_noise_stft(:,:,e_index)); 
%%% Define constants for MWF computation:
% Single Channel speech estimate
sig_s = zeros(N_freqs,N_frames);    % Speech estimate (both single and multi-channel)
sig_n = zeros(N_freqs,N_frames);    % noise estimate (single channel only) 
alpha_n = 0.9;                      % Forgetting factors for noise
n_scale = 1e-3;                     % initialization of noise correlation matrix inverse
Rnn = cell(N_freqs,N_frames);  Rnn(:) = {eye(M,M)*n_scale};      % Noise Only (NO) corr. matrix. Inverse Initialize to zeros
Ryy = cell(N_freqs,N_frames);  Ryy(:) = {zeros(M)};              % Speech + Noise (SPN) corr. matrix. Inverse Initialize to zeros

lambda = 0.99;                      % Forgetting factor for computing correlation matrices - change values to observe effects on results                           

S_mc_stft = zeros(N_freqs,N_frames);% output speech estimate STFT domain 
mvdr = zeros(M,N_freqs);
wf = zeros(N_freqs,1);

W_mc = (1/M)*ones(M,N_freqs);       % multi-channel filter
rtf_est = ones(M,N_freqs);           

%%% MWF Framework: Compute the filter, filter the signal and save output 
timerVal = tic;
for l=2:N_frames    
    for k = 1:N_freqs % Freq index
        
        %%%%%%%%%%%%%----- SINGLE CHANNEL PROCESSING ----------%%%%%%%%%%%%
        % Use SPP thresholding to compute the noise power
        % Compute speech power using the decision directed (or any other approach) 
        if SPP(k,l) < SPP_thr % noise frame
            
              % update noise estimation based on SPP
              alpha_temp = alpha_n + (1-alpha_n)*SPP(k,l);
              sig_n(k,l) = alpha_temp*sig_n(k,l-1) +(1-alpha_temp)*(y_noise_stft(k,l,e_index)*conj(y_noise_stft(k,l,e_index))); 
        
        else   % Speech frame 
            sig_n(k,l) = sig_n(k,l-1);
            
            % Decision-Directed Approach for speech estimate
            sig_s(k,l) = max((beta_s* S_mc_stft(k,l-1)) + ...
                        (1-beta_s)*((y_noise_stft(k,l,e_index)*conj(y_noise_stft(k,l,e_index)))- sig_n(k,l)), Xi_min*sig_n(k,l));
        end
        
        %%%%%%%%%%%%%----- MULTI-CHANNEL PROCESSING ----------%%%%%%%%%%%%
        
        % Use SPP thresholding to compute the Speech+Noise and Noise-Only Correlation matrices, i.e. Ryy and Rnn
        % Use either the a priori RTF vector or estimate the RTF vector
        % Compute the MVDR/MWF beamformer - you may also compute the MVDR beamformer first 
        % and then perform a single channel enhancement on the output from this.
        vec =reshape(y_noise_stft(k,l,:), [M 1]);
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
            Ryy{k,l} = Ryy{k,l-1};
        end
        % Only start mvdr and wf after 10 time frames
        Rnn_inverse = Rnn{k,l}\eye(M);
        var_calc = rtf_est(:,k)' * Rnn_inverse * rtf_est(:,k);       
        mvdr(:,k) = ( Rnn_inverse *rtf_est(:,k) )/var_calc;
        wf(k) = sig_s(k,l)/(sig_s(k,l) + (1/var_calc));
      
        % Multi-channel coefficients
        W_mc(:,k) = mvdr(:,k)*wf(k);        
        S_mc_stft(k,l) = W_mc(:,k)'*vec; 
        if k*(fs/N_fft) >= F_Dolby
            S_mc_stft(k,l) = S_mc_stft(k,l)*G_Dolby;
        end
    end % end freqs
end % end time frames

% Go back to time-domain
s_mc_TD = calc_ISTFT(S_mc_stft, win, N_fft, N_fft/R_fft, 'onesided');

% stop timer
stopTime = toc(timerVal);
timerVal = 0;

% estimate noise of enhanced signal 
[noiseMatEnh SPPEnh] = spp_calc(s_mc_TD, N_fft, R_fft);
noise_est_enhanced = calc_ISTFT(noiseMatEnh, win, N_fft, N_fft/R_fft, 'onesided');
audiowrite('part3a_pd_enh.wav', s_mc_TD, fs);
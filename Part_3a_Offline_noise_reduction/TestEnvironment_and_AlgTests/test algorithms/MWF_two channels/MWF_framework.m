function MWF_framework(color)
% Framework for the Multichannel Wiener Filter Algorithm
% Created:          November 19, 2019
% Last update:      November 19, 2019
%
%
% SPAI-Project 2019: Offline noise reduction algorithm using MWF
%
% Adapted version of NR_STFT_framework.m - Randall Ali
% close all; clear; clc;
%% Configuration:
% read one-channel signal according to the user selection
switch color
    case 1
        [y_TD1, fs] = audioread('1dB_channel3.wav');        % This is a multichannel (2D-array) audio file
        [y_TD2, fs] = audioread('1dB_channel2.wav');        % This is a multichannel (2D-array) audio file
    case 2
        [y_TD1, fs] = audioread('3dB_channel3.wav');
        [y_TD2, fs] = audioread('3dB_channel2.wav');        % This is a multichannel (2D-array) audio file
    case 3
        [y_TD1, fs] = audioread('7dB_channel3.wav'); 
        [y_TD2, fs] = audioread('7dB_channel2.wav');        % This is a multichannel (2D-array) audio file
end
y_TD=[y_TD1;y_TD2];
M = size(y_TD,2);       % Number of microphones 
% IMPORTANT: We have no information on a priori RTF 

%% STFT domain: Weighted Overlap Add method

%%%%%%% WOLA parameters %%%%%%%
N_fft = 512;                        % number of FFT points
R_fft = N_fft/2;                    % shifting (50% overlap)
win = sqrt(hann(N_fft,'periodic')); % analysis window
N_half = floor(N_fft/2)+1;          % number of bins in onsided FFT 
freqs = 0:fs/N_fft:fs/2;            % one-sided set of actual frequencies

% Computate STFT 
y_STFT = calc_STFT(y_TD, fs, win, N_fft, N_fft/R_fft, 'onesided');
% Compute SPP on reference microphone (first)
[noisePowMat, SPP] = spp_calc(y_TD(:,1),N_fft,R_fft);

[N_freqs, N_frames] = size(y_STFT(:,:,1)); % total number of freq bins + time frames

%% Define constants for MWF computation:

% Single Channel speech estimate
sig_s = zeros(N_freqs,N_frames);    % Speech estimate (both single and multi-channel)
sig_n = zeros(N_freqs,N_frames);    % noise estimate (single channel only)
Xi_min = 0.05;                  % Scaling for setting minimum value of speech power
beta_s = 0.9;                   % Forgetting factor for speech   
alpha_n = 0.9;                  % Forgetting factors for noise

n_scale = 0.5e-3;                 % initialization of noise correlation matrix inverse
                                % Initialization of speech correlation matrix inverse
Rnn = cell(N_freqs,N_frames);  Rnn(:) = {eye(M,M)*n_scale};      % Noise Only (NO) corr. matrix. Inverse Initialize to zeros
Ryy = cell(N_freqs,N_frames);  Ryy(:) = {zeros(M)};            % Speech + Noise (SPN) corr. matrix. Inverse Initialize to zeros

lambda = 0.998;
lambda_inv = 1.01;                            % Forgetting factor for computing correlation matrices - change values to observe effects on results
SPP_thr = 0.8;                                % Threshold for the SPP - also change values to observe effects on results

S_mc_stft = zeros(N_freqs,N_frames);          % output speech estimate STFT domain 
e = zeros(M,1)';                              % Microphone Selection vector
e(1) = 1;
mvdr = zeros(M,N_freqs);
wf = zeros(N_freqs,1);

W_mc = (1/M)*ones(M,N_freqs);                 % multi-channel filter
rtf_est = ones(M,N_freqs);

% Single Channel:
G_sc_stft = zeros(N_freqs,N_frames);          % Single Channel gain
S_sc_stft = zeros(N_freqs,N_frames);            

%% MWF Framework: Compute the filter, filter the signal and save output 

for l=2:N_frames % Time index - start from 2 since things would involve "l-1" - this is just a convenience for the first iteration 
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
            sig_s(k,l) = beta_s* ( S_mc_stft(k,l-1) * conj( S_mc_stft(k,l-1) ) ) + ...
                        (1-beta_s)*((y_STFT(k,l,1)*conj(y_STFT(k,l,1)))- sig_n(k,l));
            
            % Speech estimate must have minimum value in speech frame        
          
        end
        if sig_s(k,l) < Xi_min*sig_n(k,l)
                sig_s(k,l) = Xi_min*sig_n(k,l);
            end   
        
        % Single gain enhancement
        G_sc_stft(k,l) = sig_s(k,l)/(sig_s(k,l)+sig_n(k,l));
        S_sc_stft(k,l) = G_sc_stft(k,l) * y_STFT(k,l,1);
        
        %%%%%%%%%%%%%----- MULTI-CHANNEL PROCESSING ----------%%%%%%%%%%%%
        
        % Use SPP thresholding to compute the Speech+Noise and Noise-Only Correlation matrices, i.e. Ryy and Rnn
        % Use either the a priori RTF vector or estimate the RTF vector
        % Compute the MVDR/MWF beamformer - you may also compute the MVDR beamformer first 
        % and then perform a single channel enhancement on the output from this.
        
        vec =reshape(y_STFT(k,l,:), [M 1]);

        if SPP(k,l) >= SPP_thr
            Rnn{k,l} = Rnn{k,l-1};
            
            Ryy{k,l} = lambda*Ryy{k,l-1} + (1-lambda)*(vec*vec');
            % Try of Matrix inversion lemma
%             K = (lambda_inv*Ryy{k,l-1}*vec)/((1-lambda_inv)+(lambda_inv*vec'*Ryy{k,l-1}*vec));
%             Ryy{k,l} = lambda_inv*Ryy{k,l-1} - lambda_inv*K'*vec*Ryy{k,l-1};
            % update estimate_rtf in speech frame
            
            rtf_est(:,k) = rtf_cov_sub(Ryy{k,l}, Rnn{k,l}, e);
        else
            % use previous rtf_est
            if k > 1
                rtf_est(:,k) = rtf_est(:,k-1);
            end
            Rnn{k,l} = lambda*Rnn{k,l-1} + (1-lambda)*(vec*vec');
%             K = (lambda_inv*Rnn{k,l-1}*vec)/((1-lambda_inv)+(lambda_inv*vec'*Rnn{k,l-1}*vec));
%             Rnn{k,l} = lambda_inv*Rnn{k,l-1} - lambda_inv*K'*vec*Rnn{k,l-1};
        end
        % Only start mvdr and wf after 10 time frames
        Rnn_inverse = Rnn{k,l}\eye(M);
        var_calc = rtf_est(:,k)' * Rnn_inverse * rtf_est(:,k);       
        mvdr(:,k) = ( Rnn_inverse *rtf_est(:,k) )/var_calc;
        wf(k) = sig_s(k,l)/(sig_s(k,l) + (1/var_calc));

        W_mc(:,k) = mvdr(:,k)*wf(k);
        
        S_mc_stft(k,l) = W_mc(:,k)'*vec; % 1x4 * 4x1 = 1x1
        
    end % end freqs
end % end time frames

% compute the iSTFT - convert back to time domain (plot to also have a look)
time = 0:1/fs:(length(y_TD(:,1))-1)/fs;
s_mc_TD = calc_ISTFT(S_mc_stft, win, N_fft, N_fft/R_fft, 'onesided');

%% Write to audiofile
audiowrite('output.wav',s_mc_TD(:,1),fs);
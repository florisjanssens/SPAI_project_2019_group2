function demo
% demo Select a data set from the pop-up menu, then
% click one of the plot-type push buttons. Clicking the button
% plots the selected data in the axes.
    addpath(genpath(fullfile('../')));
   %  Create and then hide the UI as it is being constructed.
   f = figure('Name', 'ui_part3a', 'Visible','off','WindowState', 'maximized', 'Units', 'Normalized');
      screenSize = get(groot,'ScreenSize');
      width = screenSize(3);
      height = screenSize(4);
      drawnow;
   %  Construct the components.
   y = 0;
   fs = 0;

   % Check the number of wav files in audio directory
   audiodir = dir('../audio');
   audiodir = audiodir(3:end);
   N_Channels = size(audiodir,1);
   wav_files_names = strings(N_Channels,1);
   for i = 1:N_Channels
       wav_files_names(i) = sprintf("Receiver %d: %s",i,audiodir(i).name);
   end
   
   % Text: Select signal
   htext = uicontrol('Style','text','String','Select Receiver signal',...
           'Position',[width*0.3, height*0.7,width*0.1, height*0.1]); 
       
   % Popup: Select Signal
   selectSignal = uicontrol('Style','popupmenu', 'Tag', 'selectSignal',...
           'String',wav_files_names,...
           'Callback', @selectSignal_callback, ...
           'Position',[width*0.3,height*0.65,width*0.1,height*0.1]);
     
    index = selectSignal.Value;
   % Button: Run MWF algorithm
   run  =   uicontrol('Style', 'pushbutton', 'String', 'Run MWF', ...
                'Callback',{ @runButton_callback, f}, ...
                'Position', [width*0.7, height*0.6, width*0.2, height*0.2]);
   
   % Button: Play noisy signal
   play_noise = uicontrol('Style', 'pushbutton', 'String', 'play noisy signal',...
                'Tag', 'PlayNoisy','Callback', {@playNoiseButton_callback, f, y, fs},  ...
                'Position', [width*0.7, height*0.4, width*0.1, height*0.1]);
            
   % Button: Play enhanced signal
   play_enhanced = uicontrol('Style', 'pushbutton', 'String', 'play enhanced signal', ...
                   'Tag', 'playEnhanced', 'Callback',{@playEnhancedButton_callback, y, fs}, ...
                   'Position', [width*0.7, height*0.1, width*0.1, height*0.1]);
     
   % Plot:     
   plot_noise_stft = axes('Tag', 'stftNoise', 'Units','Pixels','Position',[width*0.1,height*0.4,width*0.2,height*0.2]); 
   plot_noise_spp = axes('Tag', 'sppNoise', 'Units','Pixels','Position',[width*0.4,height*0.4,width*0.2,height*0.2]); 
   plot_enhanced_stft = axes('Tag', 'stftEnhanced', 'Units','Pixels','Position',[width*0.1,height*0.1,width*0.2,height*0.2]); 
   plot_enhanced_time = axes('Tag', 'timeEnhanced', 'Units','Pixels','Position',[width*0.4,height*0.1,width*0.2,height*0.2]); 
   
   % Text: SNR noise
   snr_noise = uicontrol('Style', 'text', 'String', 'SNR received signal', 'Position',[width*0.8, height*0.5, width*0.1, height*0.05]);
   snr_noise_value = uicontrol('Tag', 'snrNoise','Style', 'text', 'String', 'SNR value', 'Position',[width*0.8, height*0.4, width*0.1, height*0.095]);
   % Text: SNR Enhanced
   snr_enhanced = uicontrol('Style', 'text', 'String', 'SNR enhanced signal', 'Position',[width*0.8, height*0.2, width*0.1, height*0.05]);
   snr_enhanced_value = uicontrol('Tag', 'snrEnhanced', 'Style', 'text', 'String', 'SNR value', 'Position',[width*0.8, height*0.1, width*0.1, height*0.095]);
    
   % Text: Compute Time
   compute_time = uicontrol('Style', 'text', 'String', 'Compute time: ', 'Position', [width* 0.1, height*0.75, width*0.1, height*0.05]);
   compute_time_value = uicontrol('Tag', 'ComputeTime', 'Style', 'text', 'String', 'Value ', 'Position', [width* 0.1, height*0.7, width*0.1, height*0.05]);   
   
   % Text: Set SPP Threshold (0-1)
   spp_selection = {'0.1', '0.2', '0.3', '0.4','0.5', '0.6', '0.7', '0.8', '0.9'};
   SPP_Thr_text = uicontrol('Style', 'text', 'String', 'SPP Threshold (0-1):', 'Position', [width* 0.45, height* 0.75, width*0.1, height*0.05]);
   SPP_Thr_value = uicontrol('Tag', 'sppThresh', 'Style', 'popupmenu', 'String', spp_selection, 'Position', [width* 0.55, height* 0.75, width*0.1, height*0.05]);
   
   % Text: Set Minimal Speech Xi
   Xi_selection = {'1e-7','0.01','0.05','0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9'};
   Xi_min_text = uicontrol('Style', 'text', 'String', 'Xi_min (best to keep below 1):', 'Position', [width* 0.45, height* 0.725, width*0.1, height*0.05]);
   Xi_min_value = uicontrol('Tag', 'XiMin','Style', 'popupmenu', 'String', Xi_selection, 'Position', [width* 0.55, height* 0.725, width*0.1, height*0.05]);
   
   % Text: Set forgetting factor speech
   beta_s_selection = {'0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9'};
   beta_s_text = uicontrol('Style', 'text', 'String', 'Speech Forgetting Factor(0-1):', 'Position', [width* 0.45, height* 0.7, width*0.1, height*0.05]);
   beta_s_value = uicontrol('Tag', 'betaS','Style', 'popupmenu', 'String', beta_s_selection, 'Position', [width*0.55, height* 0.7, width*0.1, height*0.05]);
   
   % Text: Set Dolby gain
   G_Dolby_selection = {'0', '1', '10', '20', '30', '40', '50'};
   G_Dolby_text = uicontrol('Style', 'text', 'String', 'Dolby Gain (dB) above 2kHz:', 'Position', [width* 0.45, height* 0.675, width*0.1, height*0.05]);
   G_dolby_value = uicontrol('Tag', 'Dolby','Style', 'popupmenu', 'String', G_Dolby_selection, 'Position', [width*0.55, height* 0.675, width*0.1, height*0.05]);
   
   % Text: Set Dolby frequency at 2kHz
   F_Dolby_selection = {'1', '2', '3', '4', '5', '6', '7', '8'};
   F_Dolby_text = uicontrol('Style', 'text', 'String', 'Frequency Gain (kHz):', 'Position', [width* 0.45, height* 0.65, width*0.1, height*0.05]);
   F_dolby_value = uicontrol('Tag', 'FDolby','Style', 'popupmenu', 'String', F_Dolby_selection, 'Position', [width*0.55, height* 0.65, width*0.1, height*0.05]);
   
   % Make the UI visible.
   f.Visible = 'on';
% --- Executes on button press in pushbutton1.
function runButton_callback(source, eventdata, f)
% hObject    handle to Run button (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

addpath(genpath(fullfile('..')));
% Process the signal in MWF_framework
index = findobj(f, 'Tag', 'selectSignal').Value;

% Get SPP value
SPP_obj = findobj(f,'Tag', 'sppThresh');
SPP_thr = str2double(SPP_obj.String(SPP_obj.Value));
% Get Xi_min value
Xi_obj =findobj(f, 'Tag', 'XiMin');
Xi_min = str2double(Xi_obj.String(Xi_obj.Value));
% Get Beta_s value
beta_s_obj = findobj(f, 'Tag', 'betaS');
beta_s = str2double(beta_s_obj.String(beta_s_obj.Value));

% Get Gain Dolby
G_Dolby_obj = findobj(f, 'Tag', 'Dolby');
G_Dolby = str2double(G_Dolby_obj.String(G_Dolby_obj.Value));
G_Dolby = 10^(G_Dolby/10);

% Get Frequency gain
F_Dolby_obj = findobj(f, 'Tag', 'FDolby');
F_Dolby = str2double(F_Dolby_obj.String(F_Dolby_obj.Value));

[y_td, y_noise_stft, SPP, noise_est_noisy, S_mc_stft, s_mc_TD, noise_est_enhanced, procesTime, times, N_freqs, fs] = MWF_framework(index, SPP_thr, Xi_min, beta_s, G_Dolby, F_Dolby);
times_stft = times(2:end-1);

% Display process time
textLabel = sprintf('%0.2f s', procesTime);
computeText = findobj(f, 'Tag', 'ComputeTime');
computeText.String = textLabel;

% SNR noisy signal in dB
SNR_noise = snr(y_td(:,index), noise_est_noisy);
textLabel = sprintf('%0.3f dB',SNR_noise);
snr_noiseBox = findobj(f, 'Tag', 'snrNoise');
snr_noiseBox.String = textLabel;

% SNR Enhanced signal in dB
SNR_enhanced = snr(s_mc_TD, noise_est_enhanced);
textLabel = sprintf('%0.3f dB',SNR_enhanced);
snr_enhancedBox = findobj(f, 'Tag', 'snrEnhanced');
snr_enhancedBox.String = textLabel;

% Plot STFT-domain noisy signal
stftNoise = findobj(f, 'Tag', 'stftNoise');
axes(stftNoise);
textlabel = sprintf("Received signal: sd%d", index);
imagesc(times_stft,N_freqs,mag2db(abs(y_noise_stft(:,:,1))), [-65, 10]); axis xy; set(gcf,'color','w');set(gca,'Fontsize',14); xlabel('Time (s)'), ylabel('Frequency (Hz)'), ...
c = colorbar;
c.Label.String = 'Amplitude (dB)';
title(textlabel);
stftNoise.Tag = 'stftNoise';

% Plot SPP of noisy signal
stftSPP = findobj(f, 'Tag', 'sppNoise');
axes(stftSPP);
imagesc(times_stft,N_freqs,SPP(:,:),[0 1]); axis xy; set(gcf,'color','w'); set(gca,'Fontsize',14);xlabel('Time (s)'), ylabel('Frequency (Hz)'),title('SPP');
c = colorbar;
c.Label.String = 'Probability of Speech';
stftSPP.Tag = 'sppNoise';

% Plot STFT-domain Enhanced signal
stftEnhanced = findobj(f, 'Tag', 'stftEnhanced');
axes(stftEnhanced);
textLabel = sprintf("STFT Receiver %d signal",index);
imagesc(times_stft,N_freqs,mag2db(abs(S_mc_stft(:,:,1))), [-65, 10]); colorbar; axis xy; set(gcf,'color','w');set(gca,'Fontsize',14); xlabel('Time (s)'), ylabel('Frequency (Hz)');
c = colorbar;
c.Label.String = 'Amplitude (dB)';
title(textLabel);
stftEnhanced.Tag = 'stftEnhanced';

% Plot the time-domain Enhanced signal
time = 0:1/fs:(length(s_mc_TD)-1)/fs;
timeEnhanced = findobj(f, 'Tag', 'timeEnhanced');
axes(timeEnhanced);
plot(time,s_mc_TD); set(gca,'Fontsize',14); xlabel('Time (s)'), ylabel('Amplitude'), title('Time-domain Enhanced signal');
timeEnhanced.Tag = 'timeEnhanced';

msgbox('Running Completed');
end

% --- Executes on button press in play_noise.
function playNoiseButton_callback(source, event, f, y, fs)
% hObject    handle to play_noise (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
select = findobj(f, 'Tag', 'selectSignal').Value;
switch select
    case 1
        [y,fs] = audioread('audio/part3a_sd1.wav');
    case 2
        [y,fs] = audioread('audio/part3a_sd2.wav');
end
player = audioplayer(y,fs);playblocking(player);
end

% --- Executes on button press in play_noise.
function playEnhancedButton_callback(source, event, y,fs)
% hObject    handle to play_noise (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[y,fs] = audioread('part3a_pd_enh.wav');
player = audioplayer(y,fs);
playblocking(player);
end
% --- Executes on selection change in selectSignal.
function selectSignal_callback(source, eventdata)
% hObject    handle to selectSignal (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    index = source.Value;
end

% --- Executes during object creation, after setting all properties.
function selectSignal_CreateFcn(hObject, eventdata, handles)
% hObject    handle to selectSignal (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
set(hObject, 'String', {'Receiver 1: audio/part3a_sd1.wav', 'Receiver 2; audio/part3a_sd2.wav'});
end
end




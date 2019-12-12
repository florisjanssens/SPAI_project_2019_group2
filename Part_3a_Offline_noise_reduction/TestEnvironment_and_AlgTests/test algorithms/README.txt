Name:
Algorithms testing for SPAI project

Description:
This folder contains three user interfaces where users could directly see the test result of the algorithms. Users don't need to read the code. The information of the test signals could be found in the folder "test environment". There are three situations to test: Spectral Subtraction algorithm, MWF with one-channel singal, two-channel signal.

Usage:

1. First go to the directory where you want to test a certain algorithm and type 'test' in command line. Then you will see the UI window.

Note: sometimes there will be some weird errors. If so, try another way: in directory find "test.fig" file. Right click and select "open in GUIDE". In the GUIDE window, type ctrl+T or just run the UI.

2. In the top left corner is the menu bar where you can choose which noise you want to add to the speech for testing. Options are 1dB SNR noise, 3dB SNR noise and 7dB SNR noise.

3. Click "Run algorithm" below the bar.

4. Wait until you see the box with "running completed!". Sometimes you have to wait for more than 20 seconds.

5. On the top there is a menu bar where you can choose which figures you want to see. Options are time domain series(of input and output signal), spectrogram (of input and output signal). Then click the "update" button. The figures will be shown below.

6. On the right part is some texts containing the information of the test result. These are STOI value, SNR and Runtime.

7. On the right part is three buttons. When you click them, you could hear the audio signals including clean speech, noisy speech and output speech.

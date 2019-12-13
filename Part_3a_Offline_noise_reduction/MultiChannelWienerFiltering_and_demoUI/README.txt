%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
README - contains an overview of the Multi-Channel Wiener Filter Algorithm for the SPAI Course 2019

Folders: 
	-	audio:              	The raw audio files will be written to this directory
	-	auxilary_fcns:		Contains 4 functions that are needed to compute the 
                            		noise reduction algorithm.
	-	framework:          	Contains the demo and MWF_framework.m to execute the offline
					noise reduction. This also contains the 
Execute:
	- 	Go in to the framework folder and write the command: demo in the console.
	- 	A UI will open which allows you to select a receiver channel based on what is 
		available in the folder audio.
	-	You can change the parameters SPP, Xi_min and beta speech to see the 
		impact on the result. Dolby gain and frequency should be left at 0 and 1 respectively.
	-	Press Run MWF to apply the noise reduction algorithm.
	-	The compute time, SNR of the noisy signal and SNR of the Enhanced signal are shown
	-	To play the audio you have two buttons available: noisy and enhanced.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
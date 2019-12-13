%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
PART3A_AUDIO	contains the audio files recorded and processed from the demo of part3a
		
	Date: 13-12-2019

	Contains:
			- part3a_sd1/sd2.wav 	: Recordings from the two secondary devices
			- part3a_pd		: Recording from the received data at primary device
			- part3a_spd_enh_sd1/sd2: Enhanced recordings using the MWF algorithm.
							  .._sd1 is the recording with the first secondary device used as the reference channel.
							  .._sd2 is the recording with the second secondary device used as the reference channel.
						  NOTE: the reference channel is the output. The other channels are purely used for the algorithm.
	Demo:		Extra information can be found in the folder MultiChannelWienerFiltering of part 3a available on git. In there you will find 
			a demo function which starts up a UI in which you can run the MWF yourself.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	

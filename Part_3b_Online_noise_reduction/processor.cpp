#include "processor.h"

#include <QDateTime>
#include <math.h>
//#include <QDebug>

Processor::Processor()
{
}

Processor::~Processor()
{
}

void Processor::processData(const std::vector<double> newData)
{
    //    qDebug() << QThread::currentThreadId() << ": Process Data";
    std::vector<std::complex<double>> fftOutput(FRAME_SIZE);
    //Perform fft on newData; size should be N_FFT samples
    std::vector<std::complex<double>> fft1 = fftTransform(newData, 1);
    std::vector<std::complex<double>> fft2 = fftTransform(newData, 2);

    fftData = std::vector<std::complex<double> >(FRAME_SIZE, 0);

    // Perform Speech Presence Probability on fftData
    spp = calculateSPP(fft2);

    // Calculate speech and noise estimates with SPP Information
    for(unsigned int k = 0; k < spp.size(); k++){
        if(spp[k] <= SPP_thr){
            sig_n[k] = calculateSig_n(k, fft1[k]);
            sig_s[k] = 0;
        }
        else {
            // NOTE: sigNoise(k) stays same as estimation from previous TIME frame
            sig_s[k] = calculateSig_s(k, fft1[k]);
        }

//        // Calculate Single Channel Enhancement
//        if (sig_s[k]+sig_n[k] == 0) {
//            qDebug() << "IF";
//        } else if (std::isnan(sig_n[k]) || std::isnan(sig_s[k]) || std::isinf(sig_n[k]) || std::isinf(sig_s[k])) {
//            qDebug() << "ELSE IF";
//        }
        GainFactor[k] = sig_s[k]/(sig_s[k]+sig_n[k]);
        fftOutput[k] = GainFactor[k] * fft1[k];
    }

    fftData = fftOutput;
    ifftTransform();

    // WHEN ALL THE PROCESSING IS DONE, PLAY THE SOUND
    QByteArray byteArray;
    for (unsigned int i = 0; i < output.size(); i++) {
        qint16 value(output[i]*pow(2, 20));
        byteArray.append(value & 0xFF);
        byteArray.append((value >> 8) & 0xFF);
    }

    //    qDebug() << QThread::currentThreadId() << ": Processed Data";
    emit readyToPlay(byteArray);
}

void Processor::initSPP(std::vector<double> noisyFrame, unsigned int noiseFrameIndex)
{
    //    qDebug() << QThread::currentThreadId() << ": Init SPP";
    noisy_dft_frame_matrix[noiseFrameIndex] = fftTransform(noisyFrame, 2);

    if (noiseFrameIndex >= NOISE_FRAMES-1) {
        for (unsigned int i = 0; i < FRAME_SIZE; i++) {
            for (unsigned int j = 0; j < NOISE_FRAMES; j++) {
                noisePowMat[i] += pow(abs(noisy_dft_frame_matrix[j][i]), 2);
            }
            noisePowMat[i] /= NOISE_FRAMES;
        }
    }
}

void Processor::convertData(QByteArray data)
{
    for (int i = 0; i < data.size(); i+=2) {
        qint16 val = qFromLittleEndian<qint16>(reinterpret_cast<const unsigned char*>(data.mid(i, 2).constData()));

        double value(val);
        value /= pow(2,20);

        buffer[static_cast<unsigned int>(i/2)] = buffer[static_cast<unsigned int>(i/2 + 256)];
        buffer[static_cast<unsigned int>(i/2 + 256)] = value;

    }

    if (bufferIndex == -1) {
        // no-op - the buffer needs to have at least two samples
        bufferIndex++;
    } else if (bufferIndex < NOISE_FRAMES) {
        initSPP(buffer, static_cast<unsigned int>(bufferIndex));
        bufferIndex++;
    } else {
        processData(buffer);
    }
}

double Processor::calculateSig_s(unsigned int k, std::complex<double> newSample)
{
    /*
     * Note: sigNoise[k] should be the noise sample at the previous time frame
     * Meaning: Since the noise tile at frequency k is only replaced with the previous noise tile value, don't worry
     */
    double sig = std::max((beta_speech*(pow(sig_s[k],2)) + (1-beta_speech)*(newSample*std::conj(newSample) - sig_n[k])).real(), xi_min*sig_n[k]);

    // qDebug() << "sig_s(" << k << "):" << sig_s[k] << " -> " << sig;

//    if (std::isinf(sig))
//    {
//        qDebug() << "Infinity";
//    }

    return sig;
}

double Processor::calculateSig_n(unsigned int k, std::complex<double> newSample)
{
    double alpha_temp = alpha_noise + (1 - alpha_noise)*spp[k];
    std::complex<double> sig = alpha_temp*sig_n[k] + (1-alpha_temp)*newSample*std::conj(newSample);
    return sig.real();
}

std::vector<double> Processor::calculateSPP(std::vector<std::complex<double> > fftData)
{
    //noise_psd_init should only be initiated at start of signalprocessor
    double alphaPSD = 0.8;

    // Constants for a posteriori SPP
    double q = 0.5; 				// a priori probability of speech presence:
    double priorFact = q/(1-q);
    double xiOptDb = 15; 			//optimal fixed a priori SNR for SPP estimation
    double xiOpt = pow(10.0 ,xiOptDb/10);
    double logGLRFact = log(1/(1+xiOpt));
    double GLRexp = xiOpt/(1+xiOpt);


    std::vector<double> noisyPer(FRAME_SIZE, 0);
    std::vector<double> snrPost1(FRAME_SIZE, 0);
    std::vector<double> GLR(FRAME_SIZE, 0);
    std::vector<double> PH1(FRAME_SIZE, 0);
    std::vector<double> estimate(FRAME_SIZE, 0); //a posteriori SNR based on old noise power estimate

    for(unsigned int i = 0; i < FRAME_SIZE; i++) {
        std::complex<double> arg = fftData[i]*std::conj(fftData[i]);
        noisyPer[i] = arg.real();
        snrPost1[i] = noisyPer[i]/noisePowMat[i];

        // Noise Power estimation
        double arg1 = logGLRFact + GLRexp*snrPost1.at(i);
        double arg2 = 200;

        GLR[i] = priorFact * exp(std::min(arg1, arg2));
        PH1[i] = GLR[i]/(1+GLR[i]); // A posteriori spp
        PH1mean[i] = alphaPH1mean*PH1mean[i] + (1-alphaPH1mean)*PH1[i];

        if(PH1mean[i] < 0.99) PH1[i] = std::min(PH1[i], 0.99);

        estimate[i] = PH1[i] * noisePowMat[i] + (1-PH1[i]) * noisyPer[i];
        noisePowMat[i] = alphaPSD*noisePowMat[i] + (1-alphaPSD)*estimate[i];
        spp[i] = PH1[i];
    }
    return spp;
}

std::vector<std::complex<double>> Processor::fftTransform(std::vector<double> newData, int hannWinExponent)
{
    std::vector<std::complex<double> > outData = std::vector<std::complex<double> > (N_FFT);
    alglib::real_1d_array xsignal;
    alglib::complex_1d_array fsignal;

    double data[N_FFT];

    for(unsigned int i = 0; i < N_FFT; i++)
    {
        data[i] = newData[i] * pow(hannWin[i], hannWinExponent);
//        if (data[i] > 1)
//            qDebug() << "data[" << i <<"] " << data[i];
    }

    xsignal.setcontent(N_FFT, data);
    fftr1d(xsignal, fsignal);

    alglib::complex *h = fsignal.getcontent();
    for(unsigned int i = 0; i<N_FFT; i++)
    {
        std::complex<double> res(h->x, h->y);
        outData[i]= res;
//        if (outData[i].real() > 1 || outData[i].imag() > 1)
//            qDebug() << "outData";
        h++;
    }

    return outData;
}

std::vector<std::complex<double> > Processor::getFftData() const
{
    return fftData;
}

void Processor::ifftTransform()
{
    output = std::vector<double>(FRAME_SIZE-1, 0);
    std::complex<double> dataArray[N_FFT];
    unsigned int i = 0;
    for(auto& data : fftData){
        dataArray[i] = data;
        dataArray[N_FFT-1-i] = std::conj(data);
        i++;
    }

    alglib::complex_1d_array fsignal;
    alglib::complex temp[N_FFT];

    for(int i = 0; i < N_FFT; i++){
        temp[i] = alglib::complex(dataArray[i].real(), dataArray[i].imag());
    }

    fsignal.setcontent(N_FFT, temp);

    alglib::real_1d_array xsignal;
    fftr1dinv(fsignal, xsignal);

    // Convert real_1d_array back to vector<double>

    double * realp = xsignal.getcontent();
    for(unsigned int i = 0; i < N_FFT; i++){
        double res = (*realp++)*hannWin[i];
        if (i < N_FFT/2)
            output[i] = res + halfFftFrame[i];
        else {
            halfFftFrame[i-N_FFT/2] = res;
        }
    }
}

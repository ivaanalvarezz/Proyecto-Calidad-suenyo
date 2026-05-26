#include "snore_detection.h"

#include <arduinoFFT.h>

#define SAMPLES 64
#define SAMPLING_FREQUENCY 16000

arduinoFFT FFT = arduinoFFT();

double vReal[SAMPLES];
double vImag[SAMPLES];

int stableCounter = 0;

bool detectSnore(int16_t samples[], int sampleCount) {

    // Convertir muestras
    for(int i = 0; i < SAMPLES; i++) {

        vReal[i] = samples[i];
        vImag[i] = 0;
    }

    // FFT
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);

    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);

    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    // Energia en graves
    double lowFreqEnergy = 0;

    for(int i = 1; i < 8; i++) {

        lowFreqEnergy += vReal[i];
    }

    // Energia total
    double totalEnergy = 0;

    for(int i = 1; i < SAMPLES / 2; i++) {

        totalEnergy += vReal[i];
    }

    // Evitar division por cero
    if(totalEnergy == 0) {

        return false;
    }

    // Porcentaje de graves
    double ratio = lowFreqEnergy / totalEnergy;

    // Detectar ronquido
    if(ratio > 0.55 && lowFreqEnergy > 50000) {

        stableCounter++;

    } else {

        stableCounter = 0;
    }

    // Necesita estabilidad temporal
    if(stableCounter >= 3) {

        stableCounter = 0;

        return true;
    }

    return false;
}
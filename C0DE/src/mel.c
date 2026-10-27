/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/mel.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Implementacion del banco de filtros Mel.
 *  Utiliza la escala HTK y filtros triangulares.
 *
 * =============================================================================
 */

#include "mel.h"

#include <math.h>
#include <stdint.h>

// =============================================================================
//  Conversion entre Hz y Mel
// =============================================================================

static float hz_to_mel(float hz) {
    return 2595.0f * log10f(1.0f + hz / 700.0f);
}

static float mel_to_hz(float mel) {
    return 700.0f * (powf(10.0f, mel / 2595.0f) - 1.0f);
}

// =============================================================================
//  Inicializacion del banco de filtros Mel
// =============================================================================

bool mel_init(Mel *mel, float *points_hz, size_t fft_size, size_t filter_count, uint32_t sample_rate, float min_hz, float max_hz) {
    // Validar punteros.
    if (mel == NULL || points_hz == NULL) {
        return false;
    }

    // Validar dimensiones.
    if (fft_size < 2 || filter_count == 0 || sample_rate == 0) {
        return false;
    }
    if (filter_count > SIZE_MAX - 2) {
        return false;
    }

    // Validar que la FFT tenga longitud potencia de dos.
    if ((fft_size & (fft_size - 1)) != 0) {
        return false;
    }

    // Validar el intervalo de frecuencias.
    float nyquist = (float)sample_rate / 2.0f;

    if (!(min_hz >= 0.0f && max_hz > min_hz && max_hz <= nyquist)) {
        return false;
    }

    // Convertir los limites a escala Mel.
    float mel_min = hz_to_mel(min_hz);
    float mel_max = hz_to_mel(max_hz);

    // Calcular las frecuencias de referencia.
    for (size_t i = 0; i < filter_count + 2; i++) {
        float position = (float)i / (float)(filter_count + 1);
        float mel_value = mel_min + position * (mel_max - mel_min);

        points_hz[i] = mel_to_hz(mel_value);

        // Evitar filtros degenerados por precision numerica.
        if (i > 0 && !(points_hz[i] > points_hz[i - 1])) {
            return false;
        }
    }

    // Configurar el banco de filtros.
    mel->fft_size = fft_size;
    mel->filter_count = filter_count;
    mel->sample_rate = sample_rate;
    mel->points_hz = points_hz;

    return true;
}

// =============================================================================
//  Aplicacion del banco de filtros Mel
// =============================================================================

bool mel_apply(const Mel *mel, const float *power, float *output) {
    // Validar punteros.
    if (mel == NULL || power == NULL || output == NULL) {
        return false;
    }

    // Validar la configuracion.
    if (mel->points_hz == NULL || mel->filter_count == 0 || mel->fft_size < 2 || mel->sample_rate == 0) {
        return false;
    }

    if ((mel->fft_size & (mel->fft_size - 1)) != 0) {
        return false;
    }

    // Numero de componentes espectrales no redundantes.
    size_t bins = mel->fft_size / 2 + 1;

    // Separacion en Hz entre componentes de la FFT.
    float bin_hz = (float)mel->sample_rate / (float)mel->fft_size;

    // Recorrer los filtros Mel.
    for (size_t m = 0; m < mel->filter_count; m++) {
        float left = mel->points_hz[m];
        float center = mel->points_hz[m + 1];
        float right = mel->points_hz[m + 2];

        // Validar los limites del filtro.
        if (!(left < center && center < right)) {
            return false;
        }

        float rise = 1.0f / (center - left);
        float fall = 1.0f / (right - center);

        float energy = 0.0f;

        // Aplicar la ponderacion triangular.
        for (size_t k = 0; k < bins; k++) {
            float frequency = (float)k * bin_hz;
            float weight = 0.0f;

            if (frequency > left && frequency <= center) {
                weight = (frequency - left) * rise;
            }
            else if (frequency > center && frequency < right) {
                weight = (right - frequency) * fall;
            }

            energy += power[k] * weight;
        }

        output[m] = energy;
    }

    return true;
}

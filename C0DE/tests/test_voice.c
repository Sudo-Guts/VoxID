/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_voice.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Regresiones de PCM, seleccion por energia y reinicio entre tomas.
 *
 * =============================================================================
 */

#include "voxid_voice.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

static VoxIDVoiceStats procesar_toma(unsigned caso) {
    uint16_t bloque[640];
    VoxIDVoiceStats resultado;
    assert(voxid_voice_begin());

    for (uint32_t b = 0U; b < 75U; ++b) {
        for (uint32_t i = 0U; i < 640U; ++i) {
            uint32_t n = b * 640U + i;
            float amplitud = caso == 2U ? 1.0f : 900.0f;
            bool senal_activa = caso >= 2U && (caso != 4U || (n >= 16000U && n < 32000U));
            float valor = caso == 1U ? 2500.0f : 2048.0f;

            if (senal_activa) {
                valor += amplitud * sinf(2.0f * 3.14159265358979323846f * 440.0f * (float)n / 16000.0f);
            }

            bloque[i] = (uint16_t)valor;
        }

        assert(voxid_voice_process_block(bloque, 640U));
    }

    assert(voxid_voice_finish());
    assert(voxid_voice_read_stats(&resultado));
    assert(resultado.samples == 48000U && resultado.frames == 298U);

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        assert(isfinite(resultado.last_mfcc[k]));
        assert(isfinite(resultado.mean_mfcc[k]));
    }

    return resultado;
}

int main(void) {
    for (unsigned caso = 0U; caso < 3U; ++caso) {
        VoxIDVoiceStats resultado = procesar_toma(caso);
        assert(resultado.voiced_frames == 0U);

        if (caso < 2U) {
            assert(resultado.pcm_peak == 0U);
        }
    }

    VoxIDVoiceStats tono = procesar_toma(3U);
    assert(tono.voiced_frames == 298U);
    VoxIDVoiceStats intermitente = procesar_toma(4U);
    assert(intermitente.voiced_frames > 0U && intermitente.voiced_frames < 298U);
    VoxIDVoiceStats repetida = procesar_toma(4U);
    assert(repetida.voiced_frames == intermitente.voiced_frames);

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        assert(fabsf(repetida.mean_mfcc[k] - intermitente.mean_mfcc[k]) < 1e-5f);
    }

    uint16_t invalida = 4096U;
    assert(voxid_voice_begin());
    assert(!voxid_voice_process_block(&invalida, 1U));
    assert(!voxid_voice_finish());
    assert(!voxid_voice_read_stats(&tono));

    assert(voxid_voice_begin());
    assert(!voxid_voice_process_block(NULL, 640U));
    assert(!voxid_voice_finish());
    (void)procesar_toma(3U);
    voxid_voice_cancel();
    assert(!voxid_voice_read_stats(&tono));

    puts("OK: silencio, DC sin transitorio, umbral, 298 frames, repeticion y cancelacion.");
    return 0;
}

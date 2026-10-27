/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_identify.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Identificacion con distancias analiticas, rechazo y perfiles ambiguos.
 *
 * =============================================================================
 */

#include "voxid_identify.h"
#include "voxid_enroll.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

#define TEST_PI 3.14159265358979323846f

static VoxIDVoiceStats crear_toma(float angulo) {
    VoxIDVoiceStats toma = {
        .samples = VOXID_VOICE_EXPECTED_SAMPLES,
        .frames = VOXID_VOICE_EXPECTED_FRAMES,
        .voiced_frames = VOXID_ENROLL_MIN_VOICED_FRAMES,
        .pcm_peak = 10000U,
        .rms_max = 0.1f
    };
    toma.mean_mfcc[0] = 100.0f;
    toma.mean_mfcc[1] = cosf(angulo);
    toma.mean_mfcc[2] = sinf(angulo);
    return toma;
}

static VoxIDSpeakerProfile crear_perfil(float angulo) {
    VoxIDSpeakerProfile perfil = {.completo = true};

    for (uint8_t p = 0U; p < VOXID_PROTOTYPES_PER_SPEAKER; ++p) {
        perfil.frames_con_voz[p] = VOXID_ENROLL_MIN_VOICED_FRAMES;
        perfil.mfcc[p][0] = 50.0f + (float)p;
        perfil.mfcc[p][1] = cosf(angulo);
        perfil.mfcc[p][2] = sinf(angulo);
    }

    return perfil;
}

static void guardar_perfil(uint8_t locutor, float angulo) {
    VoxIDSpeakerProfile perfil = crear_perfil(angulo);
    assert(voxid_enroll_restore(locutor, &perfil));
}

int main(void) {
    VoxIDVoiceStats toma = crear_toma(0.0f);
    VoxIDIdentifyResult resultado;
    assert(!voxid_identify_run(NULL, &resultado));
    assert(!voxid_identify_run(&toma, NULL));
    voxid_enroll_reset();
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_NO_PROFILES && resultado.locutor == UINT8_MAX);

    guardar_perfil(2U, 0.0f);
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_MATCH && resultado.locutor == 2U);
    assert(resultado.distancia == 0.0f && isinf(resultado.margen) && resultado.perfiles == 1U);
    toma.mean_mfcc[0] = -99999.0f;
    toma.mean_mfcc[1] = FLT_MAX;
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_MATCH && resultado.distancia == 0.0f);
    toma.mean_mfcc[1] = 1e-5f;
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_MATCH && resultado.distancia == 0.0f);

    toma = crear_toma(TEST_PI / 2.0f);
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_UNKNOWN && resultado.locutor == UINT8_MAX);
    assert(fabsf(resultado.distancia - sqrtf(2.0f)) < 1e-6f);
    toma = crear_toma(TEST_PI);
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_UNKNOWN && fabsf(resultado.distancia - 2.0f) < 1e-6f);

    voxid_enroll_reset();
    guardar_perfil(0U, 0.0f);
    guardar_perfil(1U, TEST_PI / 2.0f);
    toma = crear_toma(0.0f);
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_MATCH && resultado.locutor == 0U && resultado.margen > 1.4f);
    guardar_perfil(3U, 0.0f);
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_AMBIGUOUS && resultado.locutor == UINT8_MAX && resultado.margen == 0.0f);

    for (uint8_t caso = 0U; caso < 2U; ++caso) {
        float distancia = VOXID_IDENTIFY_MAX_DISTANCE + (caso == 0U ? -0.01f : 0.01f);
        voxid_enroll_reset();
        guardar_perfil(0U, 2.0f * asinf(distancia / 2.0f));
        assert(voxid_identify_run(&toma, &resultado));
        assert(fabsf(resultado.distancia - distancia) < 1e-6f);
        assert(resultado.status == (caso == 0U ? VOXID_IDENTIFY_MATCH : VOXID_IDENTIFY_UNKNOWN));
    }

    for (uint8_t caso = 0U; caso < 2U; ++caso) {
        float margen = VOXID_IDENTIFY_MIN_MARGIN + (caso == 0U ? -0.01f : 0.01f);
        voxid_enroll_reset();
        guardar_perfil(0U, 0.0f);
        guardar_perfil(1U, 2.0f * asinf(margen / 2.0f));
        assert(voxid_identify_run(&toma, &resultado));
        assert(fabsf(resultado.margen - margen) < 1e-6f);
        assert(resultado.status == (caso == 0U ? VOXID_IDENTIFY_AMBIGUOUS : VOXID_IDENTIFY_MATCH));
    }

    voxid_enroll_reset();
    VoxIDSpeakerProfile perfil = crear_perfil(0.0f);
    perfil.mfcc[1][1] = cosf(0.1f);
    perfil.mfcc[1][2] = sinf(0.1f);
    perfil.mfcc[2][1] = cosf(0.2f);
    perfil.mfcc[2][2] = sinf(0.2f);
    assert(voxid_enroll_restore(0U, &perfil));
    assert(voxid_identify_run(&toma, &resultado));
    float esperada = (2.0f * sinf(0.05f) + 2.0f * sinf(0.1f)) / 3.0f;
    assert(fabsf(resultado.distancia - esperada) < 1e-6f);

    // Un unico prototipo cercano no compensa dos tomas incompatibles.
    perfil.mfcc[1][1] = 0.0f;
    perfil.mfcc[1][2] = 1.0f;
    perfil.mfcc[2][1] = 0.0f;
    perfil.mfcc[2][2] = -1.0f;
    assert(voxid_enroll_restore(0U, &perfil));
    assert(voxid_identify_run(&toma, &resultado));
    assert(resultado.status == VOXID_IDENTIFY_UNKNOWN);

    voxid_enroll_reset();

    for (uint8_t locutor = 0U; locutor < VOXID_SPEAKER_COUNT; ++locutor) {
        guardar_perfil(locutor, (float)locutor * TEST_PI / 2.0f);
    }

    for (uint8_t locutor = 0U; locutor < VOXID_SPEAKER_COUNT; ++locutor) {
        toma = crear_toma((float)locutor * TEST_PI / 2.0f + 0.02f);
        assert(voxid_identify_run(&toma, &resultado));
        assert(resultado.status == VOXID_IDENTIFY_MATCH && resultado.locutor == locutor && resultado.perfiles == 4U);
    }

    for (uint8_t error = 0U; error < 6U; ++error) {
        toma = crear_toma(0.0f);

        switch (error) {
            case 0U:
                --toma.samples;
                break;
            case 1U:
                toma.voiced_frames = 0U;
                break;
            case 2U:
                toma.mean_mfcc[4] = NAN;
                break;
            case 3U:
                toma.rms_max = INFINITY;
                break;
            case 4U:
                toma.pcm_peak = 0U;
                break;
            default:
                toma.mean_mfcc[1] = 0.0f;
                break;
        }

        assert(voxid_identify_run(&toma, &resultado));
        assert(resultado.status == VOXID_IDENTIFY_INVALID_VOICE && resultado.locutor == UINT8_MAX);
    }

    puts("OK: distancia analitica, C00 excluido, tres prototipos, umbral, margen y cuatro locutores.");
    return 0;
}

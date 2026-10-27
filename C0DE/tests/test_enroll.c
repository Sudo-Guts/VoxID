/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_enroll.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Registro en RAM: tres prototipos, rechazo, cancelacion y reemplazo completo.
 *
 * =============================================================================
 */

#include "voxid_enroll.h"
#include "voxid_storage.h"
#include "mock_eeprom.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

static VoxIDVoiceStats crear_toma(unsigned numero) {
    VoxIDVoiceStats toma = {
        .samples = VOXID_VOICE_EXPECTED_SAMPLES,
        .frames = VOXID_VOICE_EXPECTED_FRAMES,
        .voiced_frames = VOXID_ENROLL_MIN_VOICED_FRAMES,
        .pcm_peak = 10000U,
        .rms_max = 0.1f
    };

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        toma.mean_mfcc[k] = (float)numero + (float)k * 0.25f;
    }

    return toma;
}

static void comprobar_perfil(uint8_t locutor, unsigned primera_toma) {
    const VoxIDSpeakerProfile *perfil = voxid_enroll_profile(locutor);
    assert(perfil != NULL && perfil->completo);

    for (uint32_t i = 0U; i < VOXID_PROTOTYPES_PER_SPEAKER; ++i) {
        VoxIDVoiceStats esperada = crear_toma(primera_toma + i);
        assert(perfil->frames_con_voz[i] == VOXID_ENROLL_MIN_VOICED_FRAMES);

        for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
            assert(perfil->mfcc[i][k] == esperada.mean_mfcc[k]);
        }
    }
}

static void completar_registro(uint8_t locutor, unsigned primera_toma) {
    assert(voxid_enroll_begin(locutor));

    for (uint32_t i = 0U; i < VOXID_PROTOTYPES_PER_SPEAKER; ++i) {
        VoxIDVoiceStats toma = crear_toma(primera_toma + i);
        VoxIDEnrollResult esperado = i + 1U == VOXID_PROTOTYPES_PER_SPEAKER ? VOXID_ENROLL_COMPLETE : VOXID_ENROLL_ACCEPTED;
        assert(voxid_enroll_add(&toma) == esperado);
        assert(voxid_enroll_count() == i + 1U);
        toma.mean_mfcc[0] = -999.0f;
    }

    comprobar_perfil(locutor, primera_toma);
}

int main(void) {
    mock_hal_reset();
    mock_eeprom_reset();
    assert(voxid_storage_init(&hi2c1));

    for (uint8_t i = 0U; i < VOXID_SPEAKER_COUNT; ++i) {
        assert(voxid_enroll_profile(i) == NULL);
    }

    VoxIDVoiceStats toma = crear_toma(1U);
    assert(!voxid_enroll_begin(VOXID_SPEAKER_COUNT));
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_REJECTED);
    assert(!voxid_enroll_take_valid(NULL));
    assert(voxid_enroll_begin(0U));
    assert(voxid_enroll_add(NULL) == VOXID_ENROLL_REJECTED);

    for (unsigned error = 0U; error < 9U; ++error) {
        VoxIDVoiceStats invalida = toma;

        switch (error) {
            case 0U:
                --invalida.samples;
                break;
            case 1U:
                --invalida.frames;
                break;
            case 2U:
                invalida.voiced_frames = VOXID_ENROLL_MIN_VOICED_FRAMES - 1U;
                break;
            case 3U:
                invalida.voiced_frames = invalida.frames + 1U;
                break;
            case 4U:
                invalida.mean_mfcc[3] = NAN;
                break;
            case 5U:
                invalida.mean_mfcc[4] = INFINITY;
                break;
            case 6U:
                invalida.rms_max = NAN;
                break;
            case 7U:
                invalida.pcm_peak = 0U;
                break;
            default:
                for (uint8_t k = 1U; k < VOXID_MFCC_COUNT; ++k) {
                    invalida.mean_mfcc[k] = 0.0f;
                }
                break;
        }

        assert(!voxid_enroll_take_valid(&invalida));
        assert(voxid_enroll_add(&invalida) == VOXID_ENROLL_REJECTED);
        assert(voxid_enroll_count() == 0U && voxid_enroll_profile(0U) == NULL);
    }

    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_ACCEPTED);
    assert(voxid_enroll_profile(0U) == NULL);
    voxid_enroll_cancel();
    assert(voxid_enroll_count() == 0U && voxid_enroll_profile(0U) == NULL);

    completar_registro(0U, 10U);
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_REJECTED);
    comprobar_perfil(0U, 10U);

    assert(voxid_enroll_begin(0U));
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_ACCEPTED);
    comprobar_perfil(0U, 10U);
    voxid_enroll_cancel();
    comprobar_perfil(0U, 10U);

    assert(voxid_enroll_begin(1U));
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_ACCEPTED);
    assert(voxid_enroll_begin(2U));
    assert(voxid_enroll_count() == 0U && voxid_enroll_profile(1U) == NULL);
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_ACCEPTED);
    assert(!voxid_enroll_begin(VOXID_SPEAKER_COUNT));
    assert(voxid_enroll_count() == 1U);
    voxid_enroll_cancel();

    for (uint8_t i = 0U; i < VOXID_SPEAKER_COUNT; ++i) {
        completar_registro(i, 20U + i * 10U);
    }

    for (uint8_t i = 0U; i < VOXID_SPEAKER_COUNT; ++i) {
        comprobar_perfil(i, 20U + i * 10U);
    }

    assert(voxid_enroll_profile(VOXID_SPEAKER_COUNT) == NULL);
    assert(!voxid_enroll_profile_valid(NULL));
    assert(!voxid_enroll_restore(0U, NULL));
    assert(!voxid_enroll_restore(VOXID_SPEAKER_COUNT, voxid_enroll_profile(0U)));

    assert(voxid_enroll_begin(0U));
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_ACCEPTED);
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_ACCEPTED);
    mock_eeprom_protected = true;
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_STORAGE_ERROR);
    assert(voxid_enroll_count() == 2U);
    comprobar_perfil(0U, 20U);
    mock_eeprom_protected = false;
    assert(voxid_enroll_add(&toma) == VOXID_ENROLL_COMPLETE);
    assert(voxid_enroll_count() == 3U);
    assert(voxid_storage_init(&hi2c1));
    const VoxIDSpeakerProfile *recuperado = voxid_enroll_profile(0U);
    assert(recuperado != NULL && recuperado->mfcc[2][5] == toma.mean_mfcc[5]);
    comprobar_perfil(1U, 30U);
    puts("OK: tres prototipos, rechazo, cancelacion, EEPROM y reintento de guardado.");
    return 0;
}

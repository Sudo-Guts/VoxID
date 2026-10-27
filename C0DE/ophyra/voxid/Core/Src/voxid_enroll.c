/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_enroll.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Banco de perfiles en RAM y sesion de registro con tres tomas.
 *  El perfil anterior permanece disponible hasta completar su reemplazo.
 *
 * =============================================================================
 */

#include "voxid_enroll.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

_Static_assert(VOXID_SPEAKER_COUNT > 0U && VOXID_SPEAKER_COUNT <= UINT8_MAX, "Cantidad de locutores fuera de rango");
_Static_assert(VOXID_PROTOTYPES_PER_SPEAKER == 3U, "La etapa 3 requiere tres prototipos por locutor");
_Static_assert(VOXID_ENROLL_MIN_VOICED_FRAMES > 0U && VOXID_ENROLL_MIN_VOICED_FRAMES <= VOXID_VOICE_EXPECTED_FRAMES, "Minimo de frames fuera de rango");
_Static_assert(VOXID_VOICE_EXPECTED_FRAMES <= UINT16_MAX, "Conteo de frames no representable");

// =============================================================================
//  Perfiles completos y registro pendiente
// =============================================================================

static VoxIDSpeakerProfile perfiles[VOXID_SPEAKER_COUNT];
static VoxIDSpeakerProfile registro_pendiente;
static uint8_t locutor_actual;
static uint8_t tomas_aceptadas;
static bool registro_activo;

bool voxid_enroll_begin(uint8_t locutor) {
    if (locutor >= VOXID_SPEAKER_COUNT) {
        return false;
    }

    voxid_enroll_cancel();
    locutor_actual = locutor;
    registro_activo = true;
    return true;
}

bool voxid_enroll_take_valid(const VoxIDVoiceStats *toma) {
    if (toma == NULL || toma->samples != VOXID_VOICE_EXPECTED_SAMPLES || toma->frames != VOXID_VOICE_EXPECTED_FRAMES) {
        return false;
    }

    if (toma->voiced_frames < VOXID_ENROLL_MIN_VOICED_FRAMES || toma->voiced_frames > toma->frames) {
        return false;
    }

    if (toma->pcm_peak == 0U || toma->pcm_peak > 32768U || !isfinite(toma->rms_max) || toma->rms_max < VOXID_VOICE_MIN_RMS || toma->rms_max > 1.0f) {
        return false;
    }

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        if (!isfinite(toma->mean_mfcc[k])) {
            return false;
        }
    }

    return true;
}

VoxIDEnrollResult voxid_enroll_add(const VoxIDVoiceStats *toma) {
    if (!registro_activo || tomas_aceptadas >= VOXID_PROTOTYPES_PER_SPEAKER || !voxid_enroll_take_valid(toma)) {
        return VOXID_ENROLL_REJECTED;
    }

    (void)memcpy(registro_pendiente.mfcc[tomas_aceptadas], toma->mean_mfcc, sizeof(registro_pendiente.mfcc[0]));
    registro_pendiente.frames_con_voz[tomas_aceptadas] = (uint16_t)toma->voiced_frames;
    ++tomas_aceptadas;

    if (tomas_aceptadas < VOXID_PROTOTYPES_PER_SPEAKER) {
        return VOXID_ENROLL_ACCEPTED;
    }

    registro_pendiente.completo = true;
    perfiles[locutor_actual] = registro_pendiente;
    registro_activo = false;
    return VOXID_ENROLL_COMPLETE;
}

uint8_t voxid_enroll_count(void) {
    return tomas_aceptadas;
}

const VoxIDSpeakerProfile *voxid_enroll_profile(uint8_t locutor) {
    if (locutor >= VOXID_SPEAKER_COUNT || !perfiles[locutor].completo) {
        return NULL;
    }

    return &perfiles[locutor];
}

void voxid_enroll_cancel(void) {
    registro_activo = false;
    tomas_aceptadas = 0U;
    (void)memset(&registro_pendiente, 0, sizeof(registro_pendiente));
}

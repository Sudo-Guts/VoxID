/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_storage.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Registros binarios little-endian de 256 bytes con CRC32 y confirmacion
 *  en una pagina separada. Cada locutor dispone de dos copias alternadas.
 *
 * =============================================================================
 */

#include "voxid_storage.h"

#include <float.h>
#include <stddef.h>
#include <string.h>

#define STORAGE_RECORD_SIZE   256U
#define STORAGE_HEADER_SIZE    32U
#define STORAGE_CRC_OFFSET    220U
#define STORAGE_COMMIT_OFFSET 224U
#define STORAGE_FORMAT_VERSION  1U
#define STORAGE_MAGIC         0x44495856U
#define STORAGE_COMMIT        0xC04D17EDU
#define STORAGE_PAYLOAD_SIZE  (VOXID_PROTOTYPES_PER_SPEAKER * (VOXID_MFCC_COUNT * 4U + 2U))

_Static_assert(sizeof(float) == 4U && FLT_RADIX == 2 && FLT_MANT_DIG == 24 && FLT_MAX_EXP == 128, "EEPROM requiere float IEEE 754 de 32 bits");
_Static_assert(VOXID_SPEAKER_COUNT * 2U * STORAGE_RECORD_SIZE <= M24C32_CAPACITY, "Los perfiles no caben en EEPROM");
_Static_assert(STORAGE_HEADER_SIZE + STORAGE_PAYLOAD_SIZE <= STORAGE_CRC_OFFSET, "El perfil excede el registro");
_Static_assert(STORAGE_COMMIT_OFFSET % M24C32_PAGE_SIZE == 0U, "La confirmacion requiere una pagina propia");

static uint8_t registros[2][STORAGE_RECORD_SIZE];
static VoxIDSpeakerProfile perfiles_leidos[2];
static uint32_t secuencias[2];
static bool validos[2];
static bool bus_vinculado;
static VoxIDStorageStatus estado_memoria = VOXID_STORAGE_NOT_INITIALIZED;

// -----------------------------------------------------------------------------
//  Formato independiente del relleno y alineacion de estructuras C
// -----------------------------------------------------------------------------

static void storage_put_u16(uint8_t *destino, uint16_t valor) {
    destino[0] = (uint8_t)valor;
    destino[1] = (uint8_t)(valor >> 8U);
}

static uint16_t storage_get_u16(const uint8_t *origen) {
    return (uint16_t)((uint16_t)origen[0] | (uint16_t)((uint16_t)origen[1] << 8U));
}

static void storage_put_u32(uint8_t *destino, uint32_t valor) {
    for (uint8_t i = 0U; i < 4U; ++i) {
        destino[i] = (uint8_t)(valor >> (i * 8U));
    }
}

static uint32_t storage_get_u32(const uint8_t *origen) {
    uint32_t valor = 0U;

    for (uint8_t i = 0U; i < 4U; ++i) {
        valor |= (uint32_t)origen[i] << (i * 8U);
    }

    return valor;
}

/**
 * @brief Calcula CRC32/ISO-HDLC sin tablas ni perifericos dedicados.
 * @param datos Bytes de cabecera, prototipos y relleno.
 * @param longitud Cantidad que se debe proteger.
 * @return CRC con polinomio reflejado 0xEDB88320 e inicio/XOR final 0xFFFFFFFF.
 */
static uint32_t storage_crc32(const uint8_t *datos, uint16_t longitud) {
    uint32_t crc = UINT32_MAX;

    for (uint16_t i = 0U; i < longitud; ++i) {
        crc ^= datos[i];

        for (uint8_t bit = 0U; bit < 8U; ++bit) {
            crc = (crc >> 1U) ^ ((crc & 1U) != 0U ? 0xEDB88320U : 0U);
        }
    }

    return ~crc;
}

static uint16_t storage_address(uint8_t locutor, uint8_t copia) {
    return (uint16_t)(((uint16_t)locutor * 2U + copia) * STORAGE_RECORD_SIZE);
}

static bool storage_profile_equal(const VoxIDSpeakerProfile *a, const VoxIDSpeakerProfile *b) {
    return memcmp(a->mfcc, b->mfcc, sizeof(a->mfcc)) == 0 && memcmp(a->frames_con_voz, b->frames_con_voz, sizeof(a->frames_con_voz)) == 0;
}

/**
 * @brief Serializa un perfil sin depender del layout de su estructura C.
 * @param registro Destino de STORAGE_RECORD_SIZE bytes.
 * @param locutor Indice del espacio que debe validar el lector.
 * @param secuencia Generacion del registro, con desbordamiento uint32 permitido.
 * @param perfil Perfil completo previamente validado.
 */
static void storage_encode(uint8_t *registro, uint8_t locutor, uint32_t secuencia, const VoxIDSpeakerProfile *perfil) {
    (void)memset(registro, 0, STORAGE_RECORD_SIZE);
    storage_put_u32(registro, STORAGE_MAGIC);
    storage_put_u16(&registro[4], STORAGE_FORMAT_VERSION);
    registro[6] = locutor;
    registro[7] = VOXID_MFCC_COUNT;
    storage_put_u32(&registro[8], secuencia);
    registro[12] = VOXID_PROTOTYPES_PER_SPEAKER;
    registro[13] = VOXID_PROFILE_DSP_REVISION;
    storage_put_u16(&registro[14], VOXID_VOICE_EXPECTED_FRAMES);
    storage_put_u32(&registro[16], VOXID_SAMPLE_RATE);
    storage_put_u16(&registro[20], VOXID_FRAME_SIZE);
    storage_put_u16(&registro[22], VOXID_HOP_SIZE);
    storage_put_u16(&registro[24], VOXID_FFT_SIZE);
    storage_put_u16(&registro[26], VOXID_MEL_FILTERS);
    storage_put_u16(&registro[28], STORAGE_PAYLOAD_SIZE);
    uint16_t posicion = STORAGE_HEADER_SIZE;

    for (uint8_t p = 0U; p < VOXID_PROTOTYPES_PER_SPEAKER; ++p) {
        for (uint8_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
            uint32_t bits;
            (void)memcpy(&bits, &perfil->mfcc[p][k], sizeof(bits));
            storage_put_u32(&registro[posicion], bits);
            posicion += 4U;
        }

        storage_put_u16(&registro[posicion], perfil->frames_con_voz[p]);
        posicion += 2U;
    }

    storage_put_u32(&registro[STORAGE_CRC_OFFSET], storage_crc32(registro, STORAGE_CRC_OFFSET));
    storage_put_u32(&registro[STORAGE_COMMIT_OFFSET], STORAGE_COMMIT);
}

/**
 * @brief Rechaza registros incompletos, incompatibles, corruptos o no numericos.
 * @param registro Bytes completos obtenidos de la memoria.
 * @param locutor Indice esperado del perfil.
 * @param perfil Destino que solo debe utilizarse si la funcion devuelve true.
 * @param secuencia Destino de la generacion validada.
 * @return true si la copia puede usarse como perfil del firmware actual.
 */
static bool storage_decode(const uint8_t *registro, uint8_t locutor, VoxIDSpeakerProfile *perfil, uint32_t *secuencia) {
    if (storage_get_u32(&registro[STORAGE_COMMIT_OFFSET]) != STORAGE_COMMIT || storage_get_u32(registro) != STORAGE_MAGIC || storage_get_u16(&registro[4]) != STORAGE_FORMAT_VERSION) {
        return false;
    }

    if (registro[6] != locutor || registro[7] != VOXID_MFCC_COUNT || registro[12] != VOXID_PROTOTYPES_PER_SPEAKER || registro[13] != VOXID_PROFILE_DSP_REVISION || storage_get_u16(&registro[14]) != VOXID_VOICE_EXPECTED_FRAMES) {
        return false;
    }

    if (storage_get_u32(&registro[16]) != VOXID_SAMPLE_RATE || storage_get_u16(&registro[20]) != VOXID_FRAME_SIZE || storage_get_u16(&registro[22]) != VOXID_HOP_SIZE || storage_get_u16(&registro[24]) != VOXID_FFT_SIZE || storage_get_u16(&registro[26]) != VOXID_MEL_FILTERS || storage_get_u16(&registro[28]) != STORAGE_PAYLOAD_SIZE) {
        return false;
    }

    if (storage_get_u32(&registro[STORAGE_CRC_OFFSET]) != storage_crc32(registro, STORAGE_CRC_OFFSET)) {
        return false;
    }

    (void)memset(perfil, 0, sizeof(*perfil));
    uint16_t posicion = STORAGE_HEADER_SIZE;

    for (uint8_t p = 0U; p < VOXID_PROTOTYPES_PER_SPEAKER; ++p) {
        for (uint8_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
            uint32_t bits = storage_get_u32(&registro[posicion]);
            (void)memcpy(&perfil->mfcc[p][k], &bits, sizeof(bits));
            posicion += 4U;
        }

        perfil->frames_con_voz[p] = storage_get_u16(&registro[posicion]);
        posicion += 2U;
    }

    perfil->completo = true;
    *secuencia = storage_get_u32(&registro[8]);
    return voxid_enroll_profile_valid(perfil);
}

/**
 * @brief Lee ambas copias y elige la mas reciente con aritmetica modular.
 * @param locutor Espacio que se desea consultar.
 * @param activa Recibe cero/uno o -1 si ninguna copia es valida.
 * @return false ante un error de bus; nunca decide con una lectura incompleta.
 */
static bool storage_read_pair(uint8_t locutor, int *activa) {
    *activa = -1;

    for (uint8_t copia = 0U; copia < 2U; ++copia) {
        if (!m24c32_read(storage_address(locutor, copia), registros[copia], STORAGE_RECORD_SIZE)) {
            return false;
        }

        validos[copia] = storage_decode(registros[copia], locutor, &perfiles_leidos[copia], &secuencias[copia]);
    }

    if (validos[0]) {
        *activa = 0;
    }

    uint32_t diferencia = secuencias[1] - secuencias[0];

    if (validos[1] && (!validos[0] || (diferencia > 0U && diferencia < 0x80000000U))) {
        *activa = 1;
    }

    return true;
}

static bool storage_pair_erased(void) {
    for (uint8_t copia = 0U; copia < 2U; ++copia) {
        for (uint16_t i = 0U; i < STORAGE_RECORD_SIZE; ++i) {
            if (registros[copia][i] != 0xFFU) {
                return false;
            }
        }
    }

    return true;
}

// =============================================================================
//  Carga al arrancar y reemplazo verificado
// =============================================================================

bool voxid_storage_init(I2C_HandleTypeDef *bus) {
    voxid_enroll_reset();
    bus_vinculado = bus != NULL;
    estado_memoria = VOXID_STORAGE_IO_ERROR;

    if (!m24c32_init(bus)) {
        return false;
    }

    bool fallo_io = false;
    bool hay_danados = false;
    uint8_t cargados = 0U;

    for (uint8_t locutor = 0U; locutor < VOXID_SPEAKER_COUNT; ++locutor) {
        int activa;

        if (!storage_read_pair(locutor, &activa)) {
            fallo_io = true;
        } else if (activa >= 0) {
            if (voxid_enroll_restore(locutor, &perfiles_leidos[activa])) {
                ++cargados;
            }
        } else if (!storage_pair_erased()) {
            hay_danados = true;
        }
    }

    estado_memoria = fallo_io ? VOXID_STORAGE_IO_ERROR : hay_danados ? VOXID_STORAGE_CORRUPT : cargados > 0U ? VOXID_STORAGE_READY : VOXID_STORAGE_EMPTY;
    return !fallo_io;
}

bool voxid_storage_save(uint8_t locutor, const VoxIDSpeakerProfile *perfil) {
    if (locutor >= VOXID_SPEAKER_COUNT || !voxid_enroll_profile_valid(perfil) || !bus_vinculado) {
        return false;
    }

    int activa;

    if (!storage_read_pair(locutor, &activa)) {
        estado_memoria = VOXID_STORAGE_IO_ERROR;
        return false;
    }

    // Tambien resuelve un reintento cuyo commit anterior si llego a escribirse.
    if (activa >= 0 && storage_profile_equal(perfil, &perfiles_leidos[activa])) {
        estado_memoria = VOXID_STORAGE_READY;
        return true;
    }

    uint8_t destino = activa == 0 ? 1U : 0U;
    uint8_t verificacion = (uint8_t)(1U - destino);
    uint32_t secuencia = activa >= 0 ? secuencias[activa] + 1U : 1U;
    uint16_t direccion = storage_address(locutor, destino);
    uint8_t invalidacion[4] = {0U, 0U, 0U, 0U};
    storage_encode(registros[destino], locutor, secuencia, perfil);
    estado_memoria = VOXID_STORAGE_IO_ERROR;

    // Invalidar y comprobar primero: una copia antigua no debe parecer nueva
    // mientras se escriben su cabecera, prototipos y CRC.
    if (!m24c32_write((uint16_t)(direccion + STORAGE_COMMIT_OFFSET), invalidacion, sizeof(invalidacion)) || !m24c32_read((uint16_t)(direccion + STORAGE_COMMIT_OFFSET), registros[verificacion], sizeof(invalidacion)) || memcmp(registros[verificacion], invalidacion, sizeof(invalidacion)) != 0) {
        return false;
    }

    if (!m24c32_write(direccion, registros[destino], STORAGE_COMMIT_OFFSET) || !m24c32_read(direccion, registros[verificacion], STORAGE_COMMIT_OFFSET) || memcmp(registros[destino], registros[verificacion], STORAGE_COMMIT_OFFSET) != 0) {
        return false;
    }

    // La confirmacion ocupa otra pagina y solo se escribe tras verificar datos.
    if (!m24c32_write((uint16_t)(direccion + STORAGE_COMMIT_OFFSET), &registros[destino][STORAGE_COMMIT_OFFSET], 4U) || !m24c32_read(direccion, registros[verificacion], STORAGE_RECORD_SIZE)) {
        return false;
    }

    if (!storage_decode(registros[verificacion], locutor, &perfiles_leidos[verificacion], &secuencias[verificacion]) || secuencias[verificacion] != secuencia || !storage_profile_equal(perfil, &perfiles_leidos[verificacion])) {
        return false;
    }

    estado_memoria = VOXID_STORAGE_READY;
    return true;
}

VoxIDStorageStatus voxid_storage_status(void) {
    return estado_memoria;
}

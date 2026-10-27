/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_storage.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  EEPROM: limites, paginas, formato, CRC, reinicio y escrituras interrumpidas.
 *
 * =============================================================================
 */

#include "m24c32.h"
#include "voxid_storage.h"
#include "mock_eeprom.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static VoxIDSpeakerProfile crear_perfil(float base) {
    VoxIDSpeakerProfile perfil = {.completo = true};

    for (uint8_t p = 0U; p < VOXID_PROTOTYPES_PER_SPEAKER; ++p) {
        perfil.frames_con_voz[p] = (uint16_t)(VOXID_ENROLL_MIN_VOICED_FRAMES + p);

        for (uint8_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
            perfil.mfcc[p][k] = base + (float)p * 0.5f - (float)k * 0.25f;
        }
    }

    return perfil;
}

static void comprobar_perfil(uint8_t locutor, const VoxIDSpeakerProfile *esperado) {
    const VoxIDSpeakerProfile *actual = voxid_enroll_profile(locutor);
    assert(actual != NULL && actual->completo);
    assert(memcmp(actual->mfcc, esperado->mfcc, sizeof(actual->mfcc)) == 0);
    assert(memcmp(actual->frames_con_voz, esperado->frames_con_voz, sizeof(actual->frames_con_voz)) == 0);
}

/* Permite construir registros con CRC correcto y metadatos deliberadamente
 * incompatibles; el rechazo no puede depender solo de un CRC incorrecto. */
static void actualizar_crc(uint8_t *registro) {
    uint32_t crc = UINT32_MAX;

    for (uint16_t i = 0U; i < 220U; ++i) {
        crc ^= registro[i];

        for (uint8_t bit = 0U; bit < 8U; ++bit) {
            uint32_t mascara = 0U - (crc & 1U);
            crc = (crc >> 1U) ^ (0xEDB88320U & mascara);
        }
    }

    crc = ~crc;

    for (uint8_t i = 0U; i < 4U; ++i) {
        registro[220U + i] = (uint8_t)(crc >> (i * 8U));
    }
}

static void comprobar_controlador(void) {
    uint8_t datos[90];
    uint8_t lectura[90];

    for (uint8_t i = 0U; i < sizeof(datos); ++i) {
        datos[i] = (uint8_t)(i * 3U);
    }

    assert(!m24c32_init(NULL));
    assert(!m24c32_read(0U, lectura, 1U));
    assert(!m24c32_write(0U, datos, 1U));
    assert(m24c32_init(&hi2c1));
    assert(!m24c32_read(0U, NULL, 1U));
    assert(!m24c32_write(0U, NULL, 1U));
    assert(!m24c32_write(0U, datos, 0U));
    assert(!m24c32_read(0U, lectura, 0U));
    assert(!m24c32_write(4095U, datos, 2U));
    assert(!m24c32_read(4095U, lectura, 2U));
    assert(!m24c32_write(4096U, datos, 1U));
    assert(!m24c32_read(UINT16_MAX, lectura, UINT16_MAX));
    assert(mock_eeprom_write_calls == 0U && mock_eeprom_read_calls == 0U);
    assert(m24c32_write(31U, datos, sizeof(datos)));
    assert(mock_eeprom_write_calls == 4U);
    assert(m24c32_read(31U, lectura, sizeof(lectura)));
    assert(memcmp(datos, lectura, sizeof(datos)) == 0);
    assert(m24c32_write(4095U, datos, 1U));
    assert(m24c32_read(4095U, lectura, 1U) && lectura[0] == datos[0]);

    mock_eeprom_busy_ms = 100U;
    uint32_t inicio = mock_tick;
    assert(!m24c32_write(128U, datos, 1U));
    assert((uint32_t)(mock_tick - inicio) == VOXID_EEPROM_WRITE_WAIT_MS);
    mock_eeprom_reboot();
    mock_eeprom_present = false;
    inicio = mock_tick;
    assert(!m24c32_init(&hi2c1));
    assert((uint32_t)(mock_tick - inicio) == VOXID_EEPROM_WRITE_WAIT_MS);
    mock_eeprom_reset();
}

static void comprobar_formato(void) {
    VoxIDSpeakerProfile perfil = crear_perfil(1.0f);
    assert(voxid_storage_init(&hi2c1));
    assert(voxid_storage_status() == VOXID_STORAGE_EMPTY);
    assert(mock_eeprom_write_calls == 0U);
    assert(!voxid_storage_save(VOXID_SPEAKER_COUNT, &perfil));
    assert(!voxid_storage_save(0U, NULL));
    VoxIDSpeakerProfile invalido = perfil;
    invalido.mfcc[1][3] = NAN;
    assert(!voxid_storage_save(0U, &invalido));
    assert(voxid_storage_save(0U, &perfil));
    assert(mock_eeprom_write_calls == 9U);
    assert(mock_eeprom_data[0] == 'V' && mock_eeprom_data[1] == 'X' && mock_eeprom_data[2] == 'I' && mock_eeprom_data[3] == 'D');
    assert(mock_eeprom_data[32] == 0U && mock_eeprom_data[33] == 0U && mock_eeprom_data[34] == 0x80U && mock_eeprom_data[35] == 0x3FU);
    uint32_t escrituras = mock_eeprom_write_calls;
    assert(voxid_storage_save(0U, &perfil));
    assert(mock_eeprom_write_calls == escrituras);
    assert(voxid_storage_init(&hi2c1));
    comprobar_perfil(0U, &perfil);
    assert(voxid_storage_status() == VOXID_STORAGE_READY);

    uint8_t registro_valido[256];
    (void)memcpy(registro_valido, mock_eeprom_data, sizeof(registro_valido));
    const uint16_t posiciones[] = {4U, 6U, 7U, 12U, 13U, 14U, 16U, 20U, 22U, 24U, 26U, 28U};

    for (uint8_t i = 0U; i < sizeof(posiciones) / sizeof(posiciones[0]); ++i) {
        (void)memcpy(mock_eeprom_data, registro_valido, sizeof(registro_valido));
        mock_eeprom_data[posiciones[i]] ^= 0x01U;
        actualizar_crc(mock_eeprom_data);
        assert(voxid_storage_init(&hi2c1));
        assert(voxid_enroll_profile(0U) == NULL);
        assert(voxid_storage_status() == VOXID_STORAGE_CORRUPT);
    }

    (void)memcpy(mock_eeprom_data, registro_valido, sizeof(registro_valido));
    mock_eeprom_data[40] ^= 0x80U;
    assert(voxid_storage_init(&hi2c1));
    assert(voxid_enroll_profile(0U) == NULL);
    (void)memcpy(mock_eeprom_data, registro_valido, sizeof(registro_valido));
    // NaN con CRC valido: el formato tambien valida el contenido numerico.
    mock_eeprom_data[34] = 0xC0U;
    mock_eeprom_data[35] = 0x7FU;
    actualizar_crc(mock_eeprom_data);
    assert(voxid_storage_init(&hi2c1));
    assert(voxid_enroll_profile(0U) == NULL);
    assert(mock_eeprom_write_calls == escrituras);
    mock_eeprom_reset();
}

static void comprobar_reemplazos(void) {
    VoxIDSpeakerProfile viejo = crear_perfil(2.0f);
    VoxIDSpeakerProfile vigente = crear_perfil(4.0f);
    VoxIDSpeakerProfile nuevo = crear_perfil(6.0f);
    uint8_t semilla[4096];
    assert(voxid_storage_init(&hi2c1));
    assert(voxid_storage_save(0U, &viejo));
    assert(voxid_storage_save(0U, &vigente));

    for (uint8_t locutor = 1U; locutor < VOXID_SPEAKER_COUNT; ++locutor) {
        assert(voxid_storage_save(locutor, &viejo));
    }

    (void)memcpy(semilla, mock_eeprom_data, sizeof(semilla));

    // Se interrumpe antes o despues de cada byte transferido: invalidacion
    // (4), cuerpo (224) y confirmacion (4), sobre una copia ya utilizada.
    for (int32_t corte = 0; corte <= 232; ++corte) {
        (void)memcpy(mock_eeprom_data, semilla, sizeof(semilla));
        mock_eeprom_reboot();
        assert(voxid_storage_init(&hi2c1));
        mock_eeprom_write_budget = corte;
        assert(!voxid_storage_save(0U, &nuevo));
        comprobar_perfil(0U, &vigente);
        mock_eeprom_reboot();
        assert(voxid_storage_init(&hi2c1));
        comprobar_perfil(0U, corte == 232 ? &nuevo : &vigente);
        comprobar_perfil(1U, &viejo);
        assert(memcmp(&mock_eeprom_data[512], &semilla[512], sizeof(semilla) - 512U) == 0);
    }

    // Fallos en cada lectura del guardado, incluyendo la verificacion final.
    for (uint32_t lectura = 1U; lectura <= 32U; ++lectura) {
        (void)memcpy(mock_eeprom_data, semilla, sizeof(semilla));
        mock_eeprom_reboot();
        assert(voxid_storage_init(&hi2c1));
        mock_eeprom_fail_read_call = mock_eeprom_read_calls + lectura;
        assert(!voxid_storage_save(0U, &nuevo));
        comprobar_perfil(0U, &vigente);
        mock_eeprom_reboot();
        assert(voxid_storage_init(&hi2c1));
        comprobar_perfil(0U, lectura >= 25U ? &nuevo : &vigente);
    }

    (void)memcpy(mock_eeprom_data, semilla, sizeof(semilla));
    mock_eeprom_reboot();
    assert(voxid_storage_init(&hi2c1));
    mock_eeprom_fail_read_call = mock_eeprom_read_calls + 25U;
    assert(!voxid_storage_save(0U, &nuevo));
    uint32_t escrituras = mock_eeprom_write_calls;
    assert(voxid_storage_save(0U, &nuevo));
    assert(mock_eeprom_write_calls == escrituras);

    // La copia nueva danada recupera la anterior; ambas invalidas vacian solo
    // ese espacio. La inicializacion nunca formatea ni toca otros locutores.
    (void)memcpy(mock_eeprom_data, semilla, sizeof(semilla));
    mock_eeprom_reboot();
    mock_eeprom_data[256U + 50U] ^= 1U;
    assert(voxid_storage_init(&hi2c1));
    comprobar_perfil(0U, &viejo);
    mock_eeprom_data[50U] ^= 1U;
    assert(voxid_storage_init(&hi2c1));
    assert(voxid_storage_status() == VOXID_STORAGE_CORRUPT);
    assert(voxid_enroll_profile(0U) == NULL);
    comprobar_perfil(3U, &viejo);
    assert(mock_eeprom_write_calls == 0U);

    (void)memcpy(mock_eeprom_data, semilla, sizeof(semilla));
    mock_eeprom_reboot();
    assert(voxid_storage_init(&hi2c1));
    mock_eeprom_protected = true;
    assert(!voxid_storage_save(0U, &nuevo));
    comprobar_perfil(0U, &vigente);
    mock_eeprom_protected = false;
    mock_eeprom_corrupt_write = true;
    assert(!voxid_storage_save(0U, &nuevo));
    comprobar_perfil(0U, &vigente);

    (void)memcpy(mock_eeprom_data, semilla, sizeof(semilla));
    mock_eeprom_reboot();
    mock_eeprom_fail_read_call = 1U;
    assert(!voxid_storage_init(&hi2c1));
    assert(voxid_storage_status() == VOXID_STORAGE_IO_ERROR);
    assert(voxid_enroll_profile(0U) == NULL);
    comprobar_perfil(1U, &viejo);

    // La secuencia pasa de UINT32_MAX a cero sin elegir la copia antigua.
    (void)memcpy(mock_eeprom_data, semilla, sizeof(semilla));
    (void)memset(&mock_eeprom_data[8], 0xFF, 4U);
    actualizar_crc(mock_eeprom_data);
    (void)memset(&mock_eeprom_data[256], 0xFF, 256U);
    mock_eeprom_reboot();
    assert(voxid_storage_init(&hi2c1));
    comprobar_perfil(0U, &viejo);
    assert(voxid_storage_save(0U, &nuevo));
    assert(mock_eeprom_data[256U + 8U] == 0U);
    assert(voxid_storage_init(&hi2c1));
    comprobar_perfil(0U, &nuevo);
}

int main(void) {
    mock_hal_reset();
    mock_eeprom_reset();
    VoxIDSpeakerProfile perfil = crear_perfil(1.0f);
    assert(voxid_storage_status() == VOXID_STORAGE_NOT_INITIALIZED);
    assert(!voxid_storage_save(0U, &perfil));
    comprobar_controlador();
    comprobar_formato();
    comprobar_reemplazos();
    assert(!voxid_storage_init(NULL));
    assert(!voxid_storage_save(0U, &perfil));
    puts("OK: paginas, limites, CRC, formato, cuatro perfiles, 233 cortes y 32 fallos de lectura.");
    return 0;
}

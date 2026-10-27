function probar_preparacion_audio()
% =============================================================================
%  Proyecto : VoxID
%  Archivo  : SIM/tests/probar_preparacion_audio.m
%  Autor    : Gustavo Ruiz Luis
% -----------------------------------------------------------------------------
%
%  Ensayos funcionales con WAV sinteticos en un directorio temporal.
%
% =============================================================================
%
% @brief Comprueba preparacion, errores, exportacion y conservacion del original.
% @note Ejecutar en MATLAB. La prueba de remuestreo usa Signal Processing Toolbox.
% @note Estos ensayos no validan codecs MP4 ni identificacion de hablantes.

    raiz_sim = fileparts(fileparts(mfilename('fullpath')));
    ruta_anterior = path;
    restaurar_ruta = onCleanup(@() path(ruta_anterior)); %#ok<NASGU>
    addpath(raiz_sim, fullfile(raiz_sim, 'funciones'));

    temporal = tempname;
    mkdir(temporal);
    limpiar_temporal = onCleanup(@() rmdir(temporal, 's')); %#ok<NASGU>
    config = configuracion();
    config.rutas.originales = fullfile(temporal, 'originales');
    config.rutas.preparados = fullfile(temporal, 'preparados');
    config.rutas.resultados = fullfile(temporal, 'resultados');
    mkdir(config.rutas.originales);

    frecuencia = config.audio.frecuencia_hz;
    tiempo = (0:frecuencia - 1)' / frecuencia;
    senal = 0.2 * sin(2 * pi * 1000 * tiempo);
    mono = fullfile(temporal, 'mono.wav');
    audiowrite(mono, senal + 0.15, frecuencia, 'BitsPerSample', 24);
    bytes_originales = leer_bytes(mono);

    % Media retirada, duracion y amplitud sin elevar una senal normal.
    toma = preparar_audio(mono, config);
    assert(toma.frecuencia_hz == frecuencia);
    assert(numel(toma.senal) == frecuencia);
    assert(abs(toma.media_retirada - 0.15) < 1e-6);
    assert(abs(mean(toma.senal)) < 1e-12);
    assert(max(abs(toma.senal - senal)) < 1e-6);
    assert(toma.ganancia_aplicada == 1);
    assert(isequal(bytes_originales, leer_bytes(mono)));

    % Dos canales de fase opuesta: seleccionar uno no cancela la voz.
    estereo = fullfile(temporal, 'estereo.wav');
    audiowrite(estereo, [senal, -senal], frecuencia, 'BitsPerSample', 24);
    toma = preparar_audio(estereo, config);
    assert(toma.canales_originales == 2 && toma.canal_seleccionado == 1);
    assert(max(abs(toma.senal - senal)) < 1e-6);
    config.audio.canal = 2;
    toma = preparar_audio(estereo, config);
    assert(max(abs(toma.senal + senal)) < 1e-6);
    esperar_error(@() preparar_audio(mono, config), 'VoxID:SIM:CanalInvalido');
    config.audio.canal = 1;

    % Recorte manual reproducible; no hay relleno a tres segundos.
    config.audio.intervalo_s = [0.1, 0.4];
    toma = preparar_audio(mono, config);
    assert(numel(toma.senal) == 4800);
    assert(max(abs(toma.intervalo_original_s - [0.1, 0.4])) < 1e-12);
    config.audio.intervalo_s = [0.1, 1.1];
    esperar_error(@() preparar_audio(mono, config), 'VoxID:SIM:IntervaloInvalido');
    config.audio.intervalo_s = [];

    % El silencio se conserva y se marca para revision; no se amplifica.
    silencio = fullfile(temporal, 'silencio.wav');
    audiowrite(silencio, zeros(frecuencia, 1), frecuencia);
    toma = preparar_audio(silencio, config);
    assert(all(toma.senal == 0) && toma.nivel_bajo);
    assert(toma.ganancia_aplicada == 1);

    % Restar media puede exceder escala completa: atenuar sin recortar.
    asimetrica = fullfile(temporal, 'asimetrica.wav');
    senal_asimetrica = 0.9 * ones(frecuencia, 1);
    senal_asimetrica(1:1600) = -0.9;
    audiowrite(asimetrica, senal_asimetrica, frecuencia, 'BitsPerSample', 24);
    toma = preparar_audio(asimetrica, config);
    assert(toma.ganancia_aplicada > 0 && toma.ganancia_aplicada < 1);
    assert(toma.pico_preparado <= config.audio.pico_maximo + eps);

    corta = fullfile(temporal, 'corta.wav');
    audiowrite(corta, zeros(config.dsp.frame_muestras - 1, 1), frecuencia);
    esperar_error(@() preparar_audio(corta, config), 'VoxID:SIM:TomaCorta');
    esperar_error(@() preparar_audio(fullfile(temporal, 'ausente.mp4'), config), 'VoxID:SIM:ArchivoNoExiste');

    % Comprobar rechazo de banda que se plegaria al reducir 48 a 16 kHz.
    if exist('resample', 'file') ~= 0
        tiempo_48k = (0:47999)' / 48000;
        entrada = 0.2 * sin(2 * pi * 1000 * tiempo_48k) + 0.2 * sin(2 * pi * 12000 * tiempo_48k);
        remuestreo = fullfile(temporal, 'remuestreo.wav');
        audiowrite(remuestreo, entrada, 48000, 'BitsPerSample', 24);
        toma = preparar_audio(remuestreo, config);
        assert(toma.remuestreado && toma.frecuencia_original_hz == 48000);
        assert(abs(numel(toma.senal) - 16000) <= 1);
        segmento = toma.senal(1601:14400);
        espectro = abs(fft(segmento)) * 2 / numel(segmento);
        assert(abs(espectro(801) - 0.2) < 0.01);
        assert(espectro(3201) < 0.01);
    else
        fprintf('OMITIDO: remuestreo, resample no esta disponible.\n');
    end

    % Una entrada corrupta no invalida las otras ni sobrescribe sus originales.
    buena = fullfile(config.rutas.originales, '01_buena.wav');
    copyfile(mono, buena);
    rota = fullfile(config.rutas.originales, '02_rota.mp4');
    identificador = fopen(rota, 'w');
    assert(identificador ~= -1);
    fprintf(identificador, 'Archivo sin pista de audio.');
    fclose(identificador);

    ruta_antes_main = path;
    [resumen, tomas] = main(config.rutas.originales, config);
    assert(height(resumen) == 2);
    assert(sum(strcmp(resumen.estado, 'ERROR')) == 1);
    assert(~isempty(tomas{1}) && isempty(tomas{2}));
    assert(isfile(tomas{1}.ruta_wav));
    assert(strcmp(path, ruta_antes_main));
    assert(isequal(bytes_originales, leer_bytes(buena)));
    [exportada, frecuencia_exportada] = audioread(tomas{1}.ruta_wav);
    assert(frecuencia_exportada == 16000);
    assert(max(abs(exportada - tomas{1}.senal)) <= 2^-22);

    main(buena, config);
    salidas_mat = dir(fullfile(config.rutas.resultados, '*', 'preparacion_audio.mat'));
    assert(numel(salidas_mat) == 2);
    sin_archivos = fullfile(temporal, 'vacia');
    mkdir(sin_archivos);
    [resumen, tomas] = main(sin_archivos, config);
    assert(isempty(resumen) && isempty(tomas));
    fprintf('OK: preparacion de audio y exportacion verificadas con WAV sinteticos.\n');
end

function esperar_error(accion, identificador_esperado)
% @brief Exige un identificador de error concreto, sin ocultar errores distintos.
% @param accion Funcion sin argumentos que debe fallar.
% @param identificador_esperado Identificador exigido para la excepcion.
    try
        accion();
    catch fallo
        assert(strcmp(fallo.identifier, identificador_esperado), 'Error inesperado: %s', fallo.identifier);
        return;
    end
    error('VoxID:SIM:PruebaFallida', 'No se produjo el error esperado: %s', identificador_esperado);
end

function bytes = leer_bytes(ruta)
% @brief Lee bytes para comprobar que el original permanece intacto.
% @param ruta Archivo que se inspeccionara sin modificarlo.
% @return Vector de bytes del archivo.
    identificador = fopen(ruta, 'rb');
    assert(identificador ~= -1);
    cerrar_archivo = onCleanup(@() fclose(identificador)); %#ok<NASGU>
    bytes = fread(identificador, Inf, '*uint8');
end

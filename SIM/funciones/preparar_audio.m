function toma = preparar_audio(ruta_archivo, config)
% =============================================================================
%  Proyecto : VoxID
%  Archivo  : SIM/funciones/preparar_audio.m
%  Autor    : Gustavo Ruiz Luis
% -----------------------------------------------------------------------------
%
%  Lectura, seleccion de canal y preparacion reproducible de una grabacion.
%  Devuelve audio y metadatos; no escribe ni modifica el archivo original.
%
% =============================================================================
%
% @brief Prepara una toma para el analisis exploratorio de VoxID.
% @param ruta_archivo Ruta de un archivo de audio o video compatible con audioread.
% @param config Estructura devuelta por configuracion().
% @return Estructura con senal mono, frecuencia_hz y metadatos de transformacion.
% @note intervalo_s, cuando se especifica, representa [inicio, fin) en segundos.
% @note Remuestrear requiere resample de Signal Processing Toolbox.
% @note La sustraccion de media no reproduce el filtro DC del firmware.

    % -------------------------------------------------------------------------
    %  Validacion de parametros
    % -------------------------------------------------------------------------

    if isstring(ruta_archivo) && isscalar(ruta_archivo)
        ruta_archivo = char(ruta_archivo);
    end
    validateattributes(ruta_archivo, {'char'}, {'row', 'nonempty'}, mfilename, 'ruta_archivo');
    validateattributes(config.audio.frecuencia_hz, {'numeric'}, {'scalar', 'integer', 'positive', 'finite'}, mfilename, 'frecuencia_hz');
    validateattributes(config.audio.canal, {'numeric'}, {'scalar', 'integer', 'positive', 'finite'}, mfilename, 'canal');
    validateattributes(config.audio.retirar_media, {'logical'}, {'scalar'}, mfilename, 'retirar_media');
    validateattributes(config.audio.pico_maximo, {'numeric'}, {'scalar', 'positive', '<=', 1, 'finite'}, mfilename, 'pico_maximo');
    validateattributes(config.audio.umbral_nivel_bajo, {'numeric'}, {'scalar', 'nonnegative', 'finite'}, mfilename, 'umbral_nivel_bajo');
    validateattributes(config.audio.umbral_cerca_riel, {'numeric'}, {'scalar', 'positive', '<=', 1, 'finite'}, mfilename, 'umbral_cerca_riel');
    validateattributes(config.dsp.frame_muestras, {'numeric'}, {'scalar', 'integer', 'positive', 'finite'}, mfilename, 'frame_muestras');

    if ~isfile(ruta_archivo)
        error('VoxID:SIM:ArchivoNoExiste', 'No existe el archivo: %s', ruta_archivo);
    end

    % -------------------------------------------------------------------------
    %  Lectura y seleccion explicita del canal
    % -------------------------------------------------------------------------

    try
        [muestras, frecuencia_original] = audioread(ruta_archivo);
    catch fallo
        error('VoxID:SIM:LecturaAudio', 'No se pudo leer %s. Revise el codec o convierta una copia a WAV. Detalle: %s', ruta_archivo, fallo.message);
    end

    if isempty(muestras) || ~ismatrix(muestras) || ~isreal(muestras) || any(~isfinite(muestras(:)))
        error('VoxID:SIM:AudioInvalido', 'El archivo no contiene muestras reales y finitas: %s', ruta_archivo);
    end
    validateattributes(frecuencia_original, {'numeric'}, {'scalar', 'integer', 'positive', 'finite'}, mfilename, 'frecuencia_original');

    canales_originales = size(muestras, 2);
    if config.audio.canal > canales_originales
        error('VoxID:SIM:CanalInvalido', 'Se solicito el canal %d, pero %s tiene %d canal(es).', config.audio.canal, ruta_archivo, canales_originales);
    end

    % Elegir un canal evita cancelaciones al promediar canales de fase opuesta.
    senal = muestras(:, config.audio.canal);
    cantidad_original = size(muestras, 1);
    duracion_original = cantidad_original / frecuencia_original;
    intervalo = config.audio.intervalo_s;
    primera_muestra = 1;
    ultima_muestra = cantidad_original;

    if ~isempty(intervalo)
        validateattributes(intervalo, {'numeric'}, {'vector', 'numel', 2, 'finite', 'nonnegative'}, mfilename, 'intervalo_s');
        if intervalo(2) <= intervalo(1) || intervalo(2) > duracion_original
            error('VoxID:SIM:IntervaloInvalido', 'El intervalo debe cumplir 0 <= inicio < fin <= %.6f s.', duracion_original);
        end
        primera_muestra = floor(intervalo(1) * frecuencia_original) + 1;
        ultima_muestra = min(ceil(intervalo(2) * frecuencia_original), cantidad_original);
        senal = senal(primera_muestra:ultima_muestra);
    end

    pico_entrada = max(abs(senal));
    rms_entrada = sqrt(mean(senal.^2));
    fraccion_cerca_riel = mean(abs(senal) >= config.audio.umbral_cerca_riel);

    % -------------------------------------------------------------------------
    %  Retiro de media y conversion de frecuencia
    % -------------------------------------------------------------------------

    media_retirada = 0;
    if config.audio.retirar_media
        media_retirada = mean(senal);
        senal = senal - media_retirada;
    end

    frecuencia_objetivo = config.audio.frecuencia_hz;
    if frecuencia_original ~= frecuencia_objetivo
        if exist('resample', 'file') == 0
            error('VoxID:SIM:FaltaResample', 'Convertir de %d a %d Hz requiere resample de Signal Processing Toolbox.', frecuencia_original, frecuencia_objetivo);
        end
        divisor = gcd(frecuencia_original, frecuencia_objetivo);
        senal = resample(senal, frecuencia_objetivo / divisor, frecuencia_original / divisor);
    end

    if numel(senal) < config.dsp.frame_muestras
        error('VoxID:SIM:TomaCorta', 'El segmento preparado tiene %d muestras; se requieren al menos %d para un frame.', numel(senal), config.dsp.frame_muestras);
    end
    if any(~isfinite(senal))
        error('VoxID:SIM:AudioInvalido', 'El procesamiento produjo muestras no finitas: %s', ruta_archivo);
    end

    % -------------------------------------------------------------------------
    %  Margen de exportacion y trazabilidad
    % -------------------------------------------------------------------------

    pico_previo = max(abs(senal));
    ganancia_aplicada = 1;
    if pico_previo > config.audio.pico_maximo
        ganancia_aplicada = config.audio.pico_maximo / pico_previo;
        senal = senal * ganancia_aplicada;
    end

    toma.ruta_original = ruta_archivo;
    toma.frecuencia_original_hz = frecuencia_original;
    toma.canales_originales = canales_originales;
    toma.canal_seleccionado = config.audio.canal;
    toma.muestras_originales = cantidad_original;
    toma.duracion_original_s = duracion_original;
    toma.intervalo_original_s = [(primera_muestra - 1) / frecuencia_original, ultima_muestra / frecuencia_original];
    toma.media_retirada = media_retirada;
    toma.ganancia_aplicada = ganancia_aplicada;
    toma.remuestreado = frecuencia_original ~= frecuencia_objetivo;
    toma.pico_entrada = pico_entrada;
    toma.rms_entrada = rms_entrada;
    toma.fraccion_cerca_riel = fraccion_cerca_riel;
    toma.senal = senal(:);
    toma.frecuencia_hz = frecuencia_objetivo;
    toma.duracion_preparada_s = numel(senal) / frecuencia_objetivo;
    toma.pico_preparado = max(abs(senal));
    toma.rms_preparado = sqrt(mean(senal.^2));
    toma.nivel_bajo = toma.rms_preparado < config.audio.umbral_nivel_bajo;
end

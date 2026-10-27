function [resumen, tomas] = main(ruta_entrada, config)
% =============================================================================
%  Proyecto : VoxID
%  Archivo  : SIM/main.m
%  Autor    : Gustavo Ruiz Luis
% -----------------------------------------------------------------------------
%
%  Entrada unica para revisar y preparar las grabaciones de MARIPOSA.
%  Exporta WAV mono, resumen CSV y datos MAT en una ejecucion independiente.
%
% =============================================================================
%
% @brief Prepara un archivo o los archivos de una carpeta, sin buscar subcarpetas.
% @param ruta_entrada Archivo o carpeta; por defecto, datos/originales.
% @param config Configuracion opcional; por defecto, configuracion().
% @return resumen Tabla con un estado y metadatos por archivo.
% @return tomas Celda de estructuras; una celda vacia indica un archivo fallido.
% @note No calcula MFCC, identifica hablantes ni estima exactitud.

    % -------------------------------------------------------------------------
    %  Inicializacion y archivos de entrada
    % -------------------------------------------------------------------------

    if nargin < 2 || isempty(config)
        config = configuracion();
    end
    if nargin < 1 || isempty(ruta_entrada)
        ruta_entrada = config.rutas.originales;
    end
    if isstring(ruta_entrada) && isscalar(ruta_entrada)
        ruta_entrada = char(ruta_entrada);
    end
    validateattributes(ruta_entrada, {'char'}, {'row', 'nonempty'}, mfilename, 'ruta_entrada');

    ruta_anterior = path;
    restaurar_ruta = onCleanup(@() path(ruta_anterior)); %#ok<NASGU>
    addpath(fullfile(fileparts(mfilename('fullpath')), 'funciones'));

    columnas = {'archivo', 'estado', 'frecuencia_original_hz', 'canales', 'canal_seleccionado', 'duracion_original_s', 'inicio_s', 'fin_s', 'duracion_preparada_s', 'pico_entrada', 'rms_preparado', 'fraccion_cerca_riel', 'media_retirada', 'ganancia_aplicada', 'wav_preparado', 'detalle'};
    resumen = cell2table(cell(0, numel(columnas)), 'VariableNames', columnas);
    tomas = cell(0, 1);

    if ~isfile(ruta_entrada) && ~isfolder(ruta_entrada)
        error('VoxID:SIM:RutaNoExiste', 'No existe la ruta de entrada: %s', ruta_entrada);
    end
    [valido, atributos] = fileattrib(ruta_entrada);
    if ~valido
        error('VoxID:SIM:RutaInaccesible', 'No se pudo resolver la ruta: %s', ruta_entrada);
    end
    ruta_entrada = atributos.Name;

    if isfile(ruta_entrada)
        archivos = {ruta_entrada};
    else
        entradas = dir(ruta_entrada);
        entradas = entradas(~[entradas.isdir]);
        [~, orden] = sort({entradas.name});
        entradas = entradas(orden);
        archivos = cell(0, 1);
        for indice = 1:numel(entradas)
            [~, ~, extension] = fileparts(entradas(indice).name);
            if any(strcmpi(extension, config.audio.extensiones))
                archivos{end + 1, 1} = fullfile(ruta_entrada, entradas(indice).name);
            end
        end
    end

    if isempty(archivos)
        fprintf('No hay grabaciones compatibles en: %s\n', ruta_entrada);
        fprintf('Coloque los MP4 en datos/originales o ejecute main(''ruta/al/archivo.mp4'').\n');
        return;
    end

    % -------------------------------------------------------------------------
    %  Directorios unicos para no reemplazar ejecuciones anteriores
    % -------------------------------------------------------------------------

    asegurar_carpeta(config.rutas.resultados);
    asegurar_carpeta(config.rutas.preparados);
    [~, identificador] = fileparts(tempname(config.rutas.resultados));
    ruta_resultados = fullfile(config.rutas.resultados, identificador);
    ruta_preparados = fullfile(config.rutas.preparados, identificador);
    asegurar_carpeta(ruta_resultados);
    asegurar_carpeta(ruta_preparados);

    registros = cell(numel(archivos), numel(columnas));
    tomas = cell(numel(archivos), 1);

    % -------------------------------------------------------------------------
    %  Preparacion independiente de cada archivo
    % -------------------------------------------------------------------------

    for indice = 1:numel(archivos)
        archivo = archivos{indice};
        registros(indice, :) = {archivo, 'ERROR', NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, NaN, '', ''};
        try
            toma = preparar_audio(archivo, config);
            [~, nombre] = fileparts(archivo);
            destino_wav = fullfile(ruta_preparados, sprintf('%03d_%s_preparado.wav', indice, nombre));
            audiowrite(destino_wav, toma.senal, toma.frecuencia_hz, 'BitsPerSample', config.audio.bits_wav);
            toma.ruta_wav = destino_wav;

            estado = 'OK';
            detalle = '';
            if toma.nivel_bajo || toma.fraccion_cerca_riel > 0
                estado = 'REVISAR';
                detalle = 'Nivel bajo o muestras cercanas al riel; no demuestra ruido ni recorte.';
            end

            tomas{indice} = toma;
            registros(indice, :) = {archivo, estado, toma.frecuencia_original_hz, toma.canales_originales, toma.canal_seleccionado, toma.duracion_original_s, toma.intervalo_original_s(1), toma.intervalo_original_s(2), toma.duracion_preparada_s, toma.pico_entrada, toma.rms_preparado, toma.fraccion_cerca_riel, toma.media_retirada, toma.ganancia_aplicada, destino_wav, detalle};
            fprintf('[%s] %s: %d -> %d Hz, %.3f s\n', estado, nombre, toma.frecuencia_original_hz, toma.frecuencia_hz, toma.duracion_preparada_s);
        catch fallo
            registros{indice, end} = sprintf('%s: %s', fallo.identifier, fallo.message);
            fprintf('[ERROR] %s: %s\n', archivo, fallo.message);
        end
    end

    resumen = cell2table(registros, 'VariableNames', columnas);
    writetable(resumen, fullfile(ruta_resultados, 'resumen_preparacion.csv'));
    save(fullfile(ruta_resultados, 'preparacion_audio.mat'), 'config', 'resumen', 'tomas');
    fprintf('Resumen y datos MAT: %s\n', ruta_resultados);
    fprintf('Audio preparado: %s\n', ruta_preparados);
end

% -----------------------------------------------------------------------------
%  Funciones auxiliares
% -----------------------------------------------------------------------------

function asegurar_carpeta(ruta)
% @brief Crea una carpeta de salida o informa por que no pudo crearla.
% @param ruta Ruta del directorio de salida.
    [creada, mensaje] = mkdir(ruta);
    if ~creada
        error('VoxID:SIM:CarpetaNoDisponible', 'No se pudo crear %s: %s', ruta, mensaje);
    end
end

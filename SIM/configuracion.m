function config = configuracion()
% =============================================================================
%  Proyecto : VoxID
%  Archivo  : SIM/configuracion.m
%  Autor    : Gustavo Ruiz Luis
% -----------------------------------------------------------------------------
%
%  Parametros y rutas del analisis de grabaciones. No modifica archivos.
%
% =============================================================================
%
% @brief Construye la configuracion inicial de SIM.
% @return Estructura con rutas, preparacion de audio y parametros DSP previstos.
% @note Las rutas se resuelven desde este archivo, no desde el directorio actual.

    raiz_sim = fileparts(mfilename('fullpath'));

    % -------------------------------------------------------------------------
    %  Rutas de entrada y salida
    % -------------------------------------------------------------------------

    config.rutas.originales = fullfile(raiz_sim, 'datos', 'originales');
    config.rutas.preparados = fullfile(raiz_sim, 'datos', 'preparados');
    config.rutas.resultados = fullfile(raiz_sim, 'resultados');

    % -------------------------------------------------------------------------
    %  Preparacion de la grabacion
    % -------------------------------------------------------------------------

    config.audio.extensiones = {'.mp4', '.m4a', '.wav', '.flac'};
    config.audio.frecuencia_hz = 16000;
    config.audio.canal = 1;
    config.audio.intervalo_s = [];
    config.audio.retirar_media = true;
    config.audio.bits_wav = 24;

    % Solo se atenua si hace falta para exportar sin recorte. No se eleva el nivel.
    config.audio.pico_maximo = 1 - 2^(1 - config.audio.bits_wav);

    % Indicadores exploratorios; no son un detector de voz ni umbrales calibrados.
    config.audio.umbral_nivel_bajo = 0.01;
    config.audio.umbral_cerca_riel = 0.999;

    % -------------------------------------------------------------------------
    %  Parametros para la siguiente etapa; aqui no se calculan MFCC
    % -------------------------------------------------------------------------

    config.dsp.frame_muestras = 400;
    config.dsp.avance_muestras = 160;
    config.dsp.fft_puntos = 512;
    config.dsp.filtros_mel = 26;
    config.dsp.coeficientes_mfcc = 13;
end

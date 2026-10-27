# =============================================================================
#  Proyecto : VoxID
#  Archivo  : SIM/tests/comprobar_valores.py
#  Autor    : Gustavo Ruiz Luis
# -----------------------------------------------------------------------------
#  Comprueba valores nominales entre KiCad y los experimentos SPICE vigentes.
# =============================================================================

import json
import math
from pathlib import Path
import re


def parsear(texto):
    """Lee la estructura de parentesis del esquematico sin librerias externas."""
    tokens = re.findall(r'\(|\)|"(?:\\.|[^"\\])*"|[^\s()]+', texto)
    pila = []
    raiz = None
    for token in tokens:
        if token == "(":
            nodo = []
            if pila:
                pila[-1].append(nodo)
            else:
                raiz = nodo
            pila.append(nodo)
        elif token == ")":
            pila.pop()
        else:
            pila[-1].append(json.loads(token) if token.startswith('"') else token)
    if pila or raiz is None:
        raise ValueError("Esquematico incompleto")
    return raiz


def valor_si(texto):
    """Convierte los valores R/C usados en estos archivos a unidades SI."""
    partes = re.fullmatch(r"([0-9]+(?:\.[0-9]+)?)([kKmunpM]?)(?:[fF])?", texto.strip())
    if partes is None:
        raise ValueError("Valor no reconocido: " + texto)
    factores = {"": 1, "k": 1e3, "K": 1e3, "M": 1e6, "m": 1e-3, "u": 1e-6, "n": 1e-9, "p": 1e-12}
    return float(partes[1]) * factores[partes[2]]


def valores_kicad(ruta):
    """Extrae componentes instanciados, sin confundirlos con simbolos de biblioteca."""
    valores = {}
    for nodo in parsear(ruta.read_text(encoding="utf-8")):
        if not isinstance(nodo, list) or not nodo or nodo[0] != "symbol":
            continue
        propiedades = {p[1]: p[2] for p in nodo if isinstance(p, list) and p and p[0] == "property"}
        referencia = propiedades.get("Reference", "")
        if referencia.startswith(("R", "C")) and referencia[1:].isdigit():
            valores[referencia] = valor_si(propiedades["Value"])
    return valores


def valores_spice(ruta):
    """Extrae el valor nominal, incluido el primer argumento de mc()."""
    valores = {}
    for linea in ruta.read_text(encoding="utf-8").splitlines():
        campos = linea.split()
        if not campos or not campos[0].startswith(("R_R", "C_C")):
            continue
        referencia = campos[0].split("_", 1)[1]
        expresion = campos[3]
        if expresion.startswith("{mc("):
            expresion = expresion[4:].split(",", 1)[0]
        if referencia in valores:
            raise ValueError("Componente duplicado: " + referencia)
        valores[referencia] = valor_si(expresion)
    return valores


def main():
    """Compara los dieciseis componentes de las cuatro secciones."""
    raiz = Path(__file__).resolve().parents[2]
    pcb = valores_kicad(raiz / "PCB/voxid.kicad_sch")
    esperados = {"R1", "R2", "R3", "R4", "R5", "R6", "R10", "R14", "C1", "C2", "C3", "C4", "C6", "C7", "C8", "C9"}
    for nombre in ["Filtro_PCB_AC.cir", "Filtro_Real_MonteCarlo.cir", "Filtro_Real_Ruido.cir"]:
        simulacion = valores_spice(raiz / "SIM/spice" / nombre)
        if set(simulacion) != esperados:
            raise ValueError("Conjunto de componentes distinto en " + nombre)
        for referencia, valor in simulacion.items():
            if referencia not in pcb or not math.isclose(valor, pcb[referencia], rel_tol=1e-12):
                raise ValueError("Valor distinto: " + nombre + " / " + referencia)
        print("OK:", nombre, "/ 16 valores coinciden con KiCad")


if __name__ == "__main__":
    main()

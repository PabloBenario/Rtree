#pragma once
#include <string>
#include <vector>
#include "Rectangulo.h"
/**
 * @brief Lee y muestra rectángulos de un archivo binario.
 *
 * @param nombreArchivo Nombre del archivo desde donde se leerán los rectángulos.
 */
void leerYImprimirRectangulos(const std::string& nombreArchivo);

/**
 * @brief Escribe una cantidad específica de rectángulos aleatorios en un archivo binario.
 *
 * @param n Cantidad de rectángulos a generar y escribir.
 * @param nombreArchivo Nombre del archivo donde se escribirán los rectángulos.
 */
void escribirRectangulos(unsigned int n, const std::string& nombreArchivo);


/**
 * @brief Ordena rectángulos por su centro X y escribe el resultado en un archivo binario.
 *
 * @param archivoEntrada Nombre del archivo binario de entrada con rectángulos.
 * @param archivoSalida Nombre del archivo binario de salida para los rectángulos ordenados.
 */
void ordenarRectangulosPorCentro(const std::string& archivoEntrada, const std::string& archivoSalida);

/**
 * @brief Verifica si los rectángulos en un archivo binario están ordenados por su centro X.
 *
 * @param nombreArchivo Nombre del archivo binario a verificar.
 * @return true si los rectángulos están ordenados, false en caso contrario.
 */
bool estanOrdenados(const std::string& nombreArchivo);






std::vector<Rectangulo> leerRectangulos(const std::string& nombreArchivo);



std::vector<std::vector<Rectangulo>> agruparRectangulos(const std::vector<Rectangulo>& rectangulos, size_t M);



void ordenarPorCentroX(std::vector<Rectangulo>& rectangulos);



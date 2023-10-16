#pragma once
#include <string>
#include <vector>
#include "Rectangulo.h"
/**
 * @brief Lee y muestra rect�ngulos de un archivo binario.
 *
 * @param nombreArchivo Nombre del archivo desde donde se leer�n los rect�ngulos.
 */
void leerYImprimirRectangulos(const std::string& nombreArchivo);

/**
 * @brief Escribe una cantidad espec�fica de rect�ngulos aleatorios en un archivo binario.
 *
 * @param n Cantidad de rect�ngulos a generar y escribir.
 * @param nombreArchivo Nombre del archivo donde se escribir�n los rect�ngulos.
 */
void escribirRectangulos(unsigned int n, const std::string& nombreArchivo);


/**
 * @brief Ordena rect�ngulos por su centro X y escribe el resultado en un archivo binario.
 *
 * @param archivoEntrada Nombre del archivo binario de entrada con rect�ngulos.
 * @param archivoSalida Nombre del archivo binario de salida para los rect�ngulos ordenados.
 */
void ordenarRectangulosPorCentro(const std::string& archivoEntrada, const std::string& archivoSalida);

/**
 * @brief Verifica si los rect�ngulos en un archivo binario est�n ordenados por su centro X.
 *
 * @param nombreArchivo Nombre del archivo binario a verificar.
 * @return true si los rect�ngulos est�n ordenados, false en caso contrario.
 */
bool estanOrdenados(const std::string& nombreArchivo);






std::vector<Rectangulo> leerRectangulos(const std::string& nombreArchivo);



std::vector<std::vector<Rectangulo>> agruparRectangulos(const std::vector<Rectangulo>& rectangulos, size_t M);



void ordenarPorCentroX(std::vector<Rectangulo>& rectangulos);



#include "ArchivosRectangulo.h"
#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>

/**
 * @brief Escribe una cantidad específica de rectángulos aleatorios en un archivo binario.
 *
 * @param n Cantidad de rectángulos a generar y escribir.
 * @param nombreArchivo Nombre del archivo donde se escribirán los rectángulos.
 *
 * Crea `n` rectángulos con coordenadas y dimensiones aleatorias. Las coordenadas
 * están distribuidas uniformemente entre [0.0, 500000.0], mientras que las dimensiones
 * están distribuidas uniformemente entre [0.0, 100.0]. Todos los rectángulos generados
 * se escriben en un archivo binario con el nombre proporcionado.
 */
void escribirRectangulos(unsigned int n, const std::string& nombreArchivo) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> coord_dist(0.0, 500000.0);
    std::uniform_real_distribution<> size_dist(0.0, 100.0);

    std::ofstream outFile(nombreArchivo, std::ios::binary);

    for (unsigned int i = 0; i < n; ++i) {
        double x1 = coord_dist(gen);
        double y1 = coord_dist(gen);
        double width = size_dist(gen);
        double height = size_dist(gen);

        Rectangulo rect(x1, y1, width, height);
        outFile.write(reinterpret_cast<char*>(&rect), sizeof(Rectangulo));
    }
    outFile.close();
}

/**
 * @brief Lee y muestra rectángulos de un archivo binario.
 *
 * @param nombreArchivo Nombre del archivo desde donde se leerán los rectángulos.
 *
 * Abre el archivo binario especificado, lee los rectángulos uno por uno y
 * los muestra en la consola utilizando el método `imprimir()` de la clase Rectangulo.
 */
void leerYImprimirRectangulos(const std::string& nombreArchivo) {
    std::ifstream inFile(nombreArchivo, std::ios::binary);

    if (!inFile) {
        std::cerr << "Error al abrir el archivo para lectura." << std::endl;
        return;
    }

    Rectangulo rect;
    while (inFile.read(reinterpret_cast<char*>(&rect), sizeof(Rectangulo))) {
        rect.imprimir();
    }

    inFile.close();
}



/**
 * @brief Toma un archivo binario de rectángulos y crea un nuevo archivo con los rectángulos ordenados por el centro.
 *
 * @param archivoEntrada Nombre del archivo binario de entrada con rectángulos.
 * @param archivoSalida Nombre del archivo binario de salida para los rectángulos ordenados.
 */
void ordenarRectangulosPorCentro(const std::string& archivoEntrada, const std::string& archivoSalida) {
    std::ifstream inFile(archivoEntrada, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error al abrir el archivo de entrada." << std::endl;
        return;
    }

    // Leer todos los rectángulos en un vector
    std::vector<Rectangulo> rectangulos;
    Rectangulo rect;
    while (inFile.read(reinterpret_cast<char*>(&rect), sizeof(Rectangulo))) {
        rectangulos.push_back(rect);
    }

    inFile.close();

    // Ordenar el vector por la coordenada X del centro
    std::sort(rectangulos.begin(), rectangulos.end(), [](const Rectangulo& a, const Rectangulo& b) {
        return a.getCenterX() < b.getCenterX();
        });

    // Escribir el vector ordenado en el archivo de salida
    std::ofstream outFile(archivoSalida, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return;
    }

    for (const Rectangulo& rect : rectangulos) {
        outFile.write(reinterpret_cast<const char*>(&rect), sizeof(Rectangulo));
    }

    outFile.close();
}


bool estanOrdenados(const std::string& nombreArchivo) {
    std::ifstream inFile(nombreArchivo, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error al abrir el archivo para lectura." << std::endl;
        return false;
    }

    Rectangulo rectActual, rectAnterior;

    // Si el archivo está vacío o tiene un solo rectángulo, consideramos que está "ordenado"
    if (!inFile.read(reinterpret_cast<char*>(&rectAnterior), sizeof(Rectangulo))) {
        return true;
    }

    while (inFile.read(reinterpret_cast<char*>(&rectActual), sizeof(Rectangulo))) {
        if (rectActual.getCenterX() < rectAnterior.getCenterX()) {
            return false; // Detectamos un rectángulo fuera de orden
        }
        rectAnterior = rectActual; // Actualizamos el rectángulo anterior
    }
    inFile.close();
    return true; // Si llegamos hasta aquí, todos los rectángulos están en orden
}




std::vector<Rectangulo> leerRectangulos(const std::string& nombreArchivo) {
    std::ifstream inFile(nombreArchivo, std::ios::binary);

    if (!inFile.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
        return {};
    }

    inFile.seekg(0, std::ios::end);
    size_t numRectangles = inFile.tellg() / sizeof(Rectangulo);
    inFile.seekg(0, std::ios::beg);

    std::vector<Rectangulo> rectangles(numRectangles);
    inFile.read(reinterpret_cast<char*>(rectangles.data()), sizeof(Rectangulo) * numRectangles);

    inFile.close();

    return rectangles;
}



void ordenarPorCentroX(std::vector<Rectangulo>& rectangulos) {
    std::sort(rectangulos.begin(), rectangulos.end(), [](const Rectangulo& a, const Rectangulo& b) {
        return a.getCenterX() < b.getCenterX();
        });
}

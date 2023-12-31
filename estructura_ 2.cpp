#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include "Rectangulo/Rectangulo.h"
#include <iomanip>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>

// Estructura para representar un punto en el plano
struct Point {
    int x, y;
    Point(int x, int y) : x(x), y(y) {}
};

// Función para generar la curva de Hilbert
std::vector<Point> generateHilbertCurve(int order) {

    std::vector<Point> path;  // Vector que almacenará los puntos de la curva

    // Conjunto de puntos de referencia para la curva de Hilbert
    const Point points[] = {
        Point(0, 0),
        Point(0, 1),
        Point(1, 1),
        Point(1, 0)
    };

    int N = std::pow(2, order);  // Número total de puntos en la curva
    int total = N * N;


    for (int i = 0; i < total; i++) {

        int original_i = i;

        int index = i & 3;   // Determina el punto de referencia actual
        Point v = points[index];  // Inicializa el punto actual

        for (int j = 1; j < order; j++) {

            i = i >> 2;       // Desplazamiento binario para recorrer los niveles
            index = i & 3;    // Nuevo punto de referencia
            int len = std::pow(2, j);  // Longitud del paso en el nivel actual

            // Transformaciones para generar la curva de Hilbert
            if (index == 0) {
                int temp = v.x;
                v.x = v.y;
                v.y = temp;
            }
            else if (index == 1) {
                v.y += len;
            }
            else if (index == 2) {
                v.x += len;
                v.y += len;
            }
            else if (index == 3) {
                int temp = len - 1 - v.x;
                v.x = len - 1 - v.y;
                v.y = temp;
                v.x += len;
            }

            path.push_back(v);  // Agrega el punto actual al vector de la curva
        }
        i = original_i;
    }

    return path;  // Devuelve la curva de Hilbert generada
}

void ordenarRectangulosPorHilbert(const std::string& archivoEntrada, const std::string& archivoSalida) {

    std::ifstream inFile(archivoEntrada, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error al abrir el archivo de entrada." << std::endl;
        return;
    }

    // Leer todos los rect�ngulos en un vector
    std::vector<Rectangulo> rectangulos;
    Rectangulo rect;
    while (inFile.read(reinterpret_cast<char*>(&rect), sizeof(Rectangulo))) {
        rectangulos.push_back(rect);
    }

    inFile.close();

    // Genera la curva de Hilbert
    std::vector<Point> hilbertCurve = generateHilbertCurve(19);

    // Crea un mapa que asocia cada punto de la curva de Hilbert con su índice
    std::map<Point, int> hilbertMap;
    for (int i = 0; i < hilbertCurve.size(); i++) {
        hilbertMap[hilbertCurve[i]] = i;
    }

    // Ordena los rectángulos en función de la posición de su centro en la curva de Hilbert
    std::sort(rectangulos.begin(), rectangulos.end(), [&hilbertMap](const Rectangulo& a, const Rectangulo& b) {
        Point centroA(a.getCenterX(), a.getCenterY());
        Point centroB(b.getCenterX(), b.getCenterY());
        return hilbertMap[centroA] < hilbertMap[centroB];
    });

    // Escribir los rectángulos ordenados en el archivo de salida
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
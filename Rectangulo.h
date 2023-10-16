#pragma once
#include <iostream>

/**
 * @class Rectangulo
 *
 * @brief Representa un rectángulo en un sistema de coordenadas 2D.
 *
 * El rectángulo está definido por dos puntos: el punto inferior izquierdo y el punto superior derecho.
 * Provee funciones auxiliares para obtener características como el ancho, alto, centro y área.
 */
class Rectangulo {
public:
    double x1, y1; ///< Coordenadas del punto inferior izquierdo.
    double x2, y2; ///< Coordenadas del punto superior derecho.

    Rectangulo(); // Constructor por defecto
    Rectangulo(double x1, double y1, double width, double height); // Otro constructor

    double getWidth() const;
    double getHeight() const;
    double getCenterX() const;
    double getCenterY() const;
    double getArea() const;
    void imprimir() const;
    bool intersecta(const Rectangulo& otro) const;
};

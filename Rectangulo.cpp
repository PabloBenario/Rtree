#include "Rectangulo.h"
#include <iomanip>

Rectangulo::Rectangulo() : x1(0), y1(0), x2(0), y2(0) {}

Rectangulo::Rectangulo(double x1, double y1, double width, double height)
    : x1(x1), y1(y1), x2(x1 + width), y2(y1 + height) {}

double Rectangulo::getWidth() const {
    return x2 - x1;
}

double Rectangulo::getHeight() const {
    return y2 - y1;
}

double Rectangulo::getCenterX() const {
    return x1 + getWidth() / 2.0;
}

double Rectangulo::getCenterY() const {
    return y1 + getHeight() / 2.0;
}

double Rectangulo::getArea() const {
    return getWidth() * getHeight();
}

void Rectangulo::imprimir() const {
    double centroX = (x1 + x2) / 2.0;
    std::cout << "[x1: " << x1 << ", y1: " << y1 << ", x2: " << x2 << ", y2: " << y2 << ", centroX: " << centroX << "]" << std::endl;
}


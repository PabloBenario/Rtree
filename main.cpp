
#include "ArchivosRectangulo.h"
#include "Rectangulo.h"
#include "AuxFunctions.h"

const unsigned int M = 2;  // Capacidad máxima de HIJOS de un nodo.
const unsigned int N = 6; 



Nodo* construirRtree(const std::vector<Rectangulo>& rectangulosOrdenados) {
    if (rectangulosOrdenados.empty()) return nullptr;

    // Convertir los rectángulos en nodos hoja.
    std::vector<Nodo*> nodos;
    for (const auto& rect : rectangulosOrdenados) {
        Nodo* nodo = new Nodo();
        nodo->entradas.push_back(Entrada(rect));
        nodos.push_back(nodo);
    }

    // Si el número de nodos (o rectángulos) es menor a M, entonces simplemente deberíamos tener un nodo raíz que apunte a todos ellos.
    if (nodos.size() <= M) {
        Nodo* raiz = new Nodo();
        for (Nodo* nodo : nodos) {
            raiz->entradas.push_back(Entrada(nodo->calcularMBR(), nodo));
        }
        return raiz;
    }

    // Agrupamos los nodos en grupos de tamaño M y creamos nuevos nodos hasta llegar a la raíz.
    while (nodos.size() > M) {
        std::vector<Nodo*> nodosNuevos;

        for (size_t i = 0; i < nodos.size(); i += M) {
            Nodo* padre = new Nodo();

            // Por cada nodo en este grupo, añadimos una entrada al nodo padre.
            for (size_t j = i; j < i + M && j < nodos.size(); ++j) {
                Nodo* hijo = nodos[j];
                Rectangulo mbr = hijo->calcularMBR();
                padre->entradas.push_back(Entrada(mbr, hijo));
            }

            nodosNuevos.push_back(padre);
        }

        nodos = nodosNuevos;
    }

    // Finalmente, el único nodo que queda es la raíz.
    return nodos[0];
}



void imprimirRtree(Nodo* nodo, int nivel = 0) {
    if (!nodo) return;

    // Sangría para visualización jerárquica.
    for (int i = 0; i < nivel; ++i) {
        std::cout << "       "; // Usamos dos espacios por nivel.
    }

    // Imprime información del nodo actual.
    Rectangulo mbrNodo = nodo->calcularMBR();
    std::cout << "Nodo (MBR: [" << mbrNodo.x1 << ", " << mbrNodo.y1 << ", " << mbrNodo.x2 << ", " << mbrNodo.y2 << "])" << std::endl;

    // Si es un nodo hoja, imprime directamente las entradas.
    if (nodo->entradas[0].hijo == nullptr) {
        for (const auto& entrada : nodo->entradas) {
            for (int i = 0; i < nivel + 1; ++i) {
                std::cout << "  ";
            }
            std::cout << "Entrada (MBR: [" << entrada.mbr.x1 << ", " << entrada.mbr.y1 << ", " << entrada.mbr.x2 << ", " << entrada.mbr.y2 << "])" << std::endl;
        }
    }
    else {
        // Si no es una hoja, imprimimos recursivamente los hijos.
        for (const auto& entrada : nodo->entradas) {
            imprimirRtree(entrada.hijo, nivel + 1);
        }
    }
}



int main() {

    // Generar y escribir rectángulos
    escribirRectangulos(N, "rectangles.bin");  // --------------------------------------------------------------------------------------------------------------
    std::cout << "Se escribieron " << N << " rectangulos en rectangles.bin\n";

    std::string filename = "ordered-rectangles.bin";
    // Ordenar rectángulos y verificar
    ordenarRectangulosPorCentro("rectangles.bin", filename);//----------------------------------------------------------------------------------------------------
    if (estanOrdenados("ordered-rectangles.bin")) {
        std::cout << "Los rectángulos están correctamente ordenados.\n" << "Se pusieron ordenados en ordered-rectangles.bin\n";
    }
    else {
        std::cerr << "Los rectángulos no están ordenados.\n";
    }

    std::vector < Rectangulo > rectangles = leerRectangulos(filename); //-------------------------------------------------------------------------------------------
    std::cout << "Se leyeron los rectangulos de ordered-bin y se guardaron en un array\n";

 
    // Imprimir rectángulos ordenados
    std::cout << "\nAhora los imprimimos ordenados\n";
    for (const Rectangulo& rect : rectangles) {
        rect.imprimir();
    }

    std::cout << "\nComenzamos la construccion delR-tree\n";
    Nodo* raiz = construirRtree(rectangles);
    std::cout << "\nConstruido!\n";
    std::cout << "\nLo imprimios!\n";
    imprimirRtree(raiz, 0);
    std::cout << "-----------------" << std::endl;



    // Crear y mostrar grupos de rectángulos
    std::cout << "\nAhora los agrupamos en grupos de tamano" << M << " y los imprimimos : \n";
    std::vector < std::vector < Rectangulo >> grupos = agruparRectangulos(rectangles, M);
    int numeroGrupo = 1; // Iniciar contador de grupos
    for (const auto& grupo : grupos) {
        std::cout << "\n---- Grupo " << numeroGrupo << " ----\n";
        for (const Rectangulo& rect : grupo) {
            rect.imprimir();
        }
        std::cout << "--------------\n";
        numeroGrupo++;
    }
    std::cout << "\n--------------------------" << "\n--------------------------\n";



    std::cout << "\n111111111111111111\n";
    // Después de haber creado los grupos:
    std::vector<Nodo> nodos = crearNodosDesdeGrupos(grupos);
    std::cout << "Aqui los grupos que creamos antes los transformamos en nodos!\n";
    std::cout << "Se crearon " << nodos.size() << " nodos." << std::endl;
    std::cout << "\n--------------------------\n";
    
    std::cout << "\nprinteamos los hijos de cada unos de estos nodos\n";
    for (size_t i = 0; i < nodos.size(); ++i) {
        std::cout << "Nodo " << (i + 1) << " tiene los siguientes hijos:\n";
        for (const Entrada& entrada : nodos[i].entradas) {
            if (entrada.hijo) {
                // Si tienes alguna forma de identificar cada nodo, la puedes imprimir aquí.
                // Por simplicidad, aquí solo indicamos que hay un hijo.
                std::cout << "  - Tiene un hijo\n";
              

            }
            else {
                // No hay hijo, solo es una entrada de hoja.
                entrada.mbr.imprimir();
            }
        }
        std::cout << "------------------------\n";
    }


    //Aqui calculamos el MBR de cada uno de los nodos que creamos antes y los ordenamos, obviamente
    std::cout << "\nAqui calculamos el MBR de cada uno de los nodos que creamos antes y los ordenamos, obviamente\n";
    std::vector<Rectangulo> rectangulosMBRS = calcularMBRsDeNodos(nodos); //ESTAN DESORDENADOS.
    ordenarPorCentroX(rectangulosMBRS);
    // Imprimir rectángulos MBRS de 
    std::cout << "\nAhora los imprimimos \n" << "--------------------------\n";
    for (const Rectangulo& rect : rectangulosMBRS) {
        rect.imprimir();
    }
    std::cout << "--------------------------\n";




    std::cout << "Esta ultima parte es mas que nada para comprobar\nque se estaba haciendo bien el procedimiento\n";

    imprimirMBRsDeNodos(nodos);

    std::cout << "--------------------------\n";

    return 0;
}


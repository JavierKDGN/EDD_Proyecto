#pragma once

// Referencia: https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/

/*
* Uso del codigo en archivos:
* Se lee un archivo de texto con la funcion de lectura de InputOutput.h
* luego se codifica llamando a la funcion codificar que acepta una string
* y devuelve un vector de unsigned char que se puede escribir en un archivo binario
* con la funcion de escritura en binario de InputOutput.h
* para descomprimir se lee el archivo binario con la funcion de lectura en binario
* de InputOutput.h y se llama a la funcion decodificar que acepta un vector de unsigned char
* y devuelve un string que se puede escribir en un archivo de texto con la funcion de escritura
*/

#include "Nodo.h"
#include <iostream>
#include <queue>
#include <map>
#include <bitset>


class Huffman {
private:
    std::string str_original;   // String original a codificar
    std::string str_codificada; // String codificado usando Huffman
    std::map<char, std::string> tabla; // Tabla de codigos Huffman
    Nodo* raiz; // Raiz del arbol de Huffman

    // Funcion recursiva para obtener los codigos Huffman
    void _obtenerCodigos(Nodo* raiz, std::string str) {
        if (!raiz) {
            return;
        }

        if (raiz->dato != '$') {
            tabla[raiz->dato] = str;
        }

        _obtenerCodigos(raiz->izq, str + "0");
        _obtenerCodigos(raiz->der, str + "1");
    }

    // Funcion para codificar un string usando la tabla de codigos Huffman
    std::string _codificarString(Nodo* raiz, const std::string& str) {
        std::string str_codificada;
        for (char c : str) {
            str_codificada += tabla[c];
        }
        return str_codificada;
    }

    //Funcion auxiliar para convertir un string a un vector de bytes
    std::vector<unsigned char> _toBinary(const std::string& str) {
        std::vector<unsigned char> binary_data;
        unsigned char current_byte = 0;
        int bit_count = 0;

        for (char c : str) {
            current_byte = (current_byte << 1) | (c - '0'); // Desplaza el byte actual a la izquierda y añade el bit actual
            bit_count++;
            if (bit_count == 8) { // Si hemos acumulado 8 bits
                binary_data.push_back(current_byte); // Añadir el byte al vector
                current_byte = 0; // Reiniciar el byte actual
                bit_count = 0; // Reiniciar el contador de bits
            }
        }

        if (bit_count > 0) { // Si quedan bits que no llenan un byte completo
            current_byte <<= (8 - bit_count); // Desplazar el byte a la izquierda para llenar con ceros
            binary_data.push_back(current_byte); // Añadir el byte final al vector
        }

        return binary_data;
    }

    //Funcion auxiliar para convertir un vector de bytes a un string
    std::string _fromBinary(const std::vector<unsigned char>& binary_data) {
        std::string str;
        for (unsigned char byte : binary_data) {
            std::bitset<8> bits(byte);
            str += bits.to_string();
        }
        return str;
    }

public:
    // Funcion principal para codificar un string utilizando Huffman
    std::vector<unsigned char> codificar(const std::string& str) {
        // Cola de prioridad de nodos que representan los caracteres del string
        std::priority_queue<Nodo*, std::vector<Nodo*>, compararNodos> cola;

        // Conteo de frecuencias de cada caracter en el string
        std::map<char, int> freq;
        for (char c : str) {
            freq[c]++;
        }

        // Creacion de nodos y colocacion en la cola de prioridad
        for (auto i = freq.begin(); i != freq.end(); i++) {
            cola.push(new Nodo(i->first, i->second));
        }

        Nodo* izq, * der, * top;

        // Construccion del arbol de Huffman combinando nodos de menor frecuencia
        while (cola.size() != 1) {
            // Sacamos los dos nodos con menor frecuencia
            izq = cola.top();
            cola.pop();

            der = cola.top();
            cola.pop();

            // Creamos un nuevo nodo interno con la suma de las frecuencias de los dos nodos sacados
            // Los nodos sacados se convierten en hijos de este nuevo nodo
            // El caracter '$' representa un nodo interno
            top = new Nodo('$', izq->freq + der->freq);
            top->izq = izq;
            top->der = der;

            cola.push(top);
        }

        raiz = cola.top(); // La raiz del arbol es el unico nodo restante en la cola
        _obtenerCodigos(raiz, ""); // Obtenemos los codigos Huffman
        str_codificada = _codificarString(raiz, str); // Codificamos el string original
        return _toBinary(str_codificada);
    }

    // Funcion para decodificar un string codificado usando el arbol de Huffman
    std::string decodificar(const std::vector<unsigned char>& binary_data) {
        std::string str_decodificada;
        Nodo* temp = raiz;

        std::string str = _fromBinary(binary_data);

        // Recorremos el string codificado caracter por caracter
        for (char c : str) {
            if (c == '0') {
                temp = temp->izq; // Movemos hacia el hijo izquierdo si encontramos '0'
            }
            else {
                temp = temp->der; // Movemos hacia el hijo derecho si encontramos '1'
            }

            // Si llegamos a una hoja, agregamos el caracter correspondiente a la cadena decodificada
            if (temp->isHoja()) {
                str_decodificada += temp->dato;
                temp = raiz; // Reiniciamos el recorrido desde la raiz
            }
        }

        return str_decodificada;
    }

    // Funcion para obtener el string codificado
    std::string getStringCodificada() {
        return str_codificada;
    }

    // Funcion para obtener la tabla de codigos Huffman
    std::map<char, std::string> getTabla() {
        return tabla;
    }
};
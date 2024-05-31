#include "AdministradorDeTexturas.h"
#include <iostream>

bool AdministradorDeTexturas::cargarTextura(const std::string& nombre, const std::string& rutaArchivo) {
    std::shared_ptr<sf::Texture> textura = std::make_shared<sf::Texture>();
    if (!textura->loadFromFile(rutaArchivo)) {
        std::cerr << "Error al cargar la textura: " << rutaArchivo << std::endl;
        return false;
    }
    texturas[nombre] = textura;
    return true;
}

std::shared_ptr<sf::Texture> AdministradorDeTexturas::obtenerTextura(const std::string& nombre) {
    return texturas[nombre];
}

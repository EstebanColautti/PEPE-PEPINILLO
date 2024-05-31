#ifndef ADMINISTRADOR_DE_TEXTURAS_H
#define ADMINISTRADOR_DE_TEXTURAS_H

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <string>

class AdministradorDeTexturas {
private:
    std::map<std::string, std::shared_ptr<sf::Texture>> texturas;

public:
    bool cargarTextura(const std::string& nombre, const std::string& rutaArchivo);
    std::shared_ptr<sf::Texture> obtenerTextura(const std::string& nombre);
};

#endif // ADMINISTRADOR_DE_TEXTURAS_H

#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>

class Menu {
private:
    int seleccionado;
    sf::Font fuente;
    std::vector<sf::Text> menuPrincipal;

public:
    Menu(float ancho, float alto, const std::string& rutaFuente);
    void dibujar(sf::RenderWindow& ventana);
    void moverArriba();
    void moverAbajo();
    int obtenerSeleccionado() const;
};

#endif // MENU_H

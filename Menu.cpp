#include "Menu.h"

Menu::Menu(float ancho, float alto, const std::string& rutaFuente) : seleccionado(1) {
    fuente.loadFromFile(rutaFuente);

    std::vector<std::string> opcionesMenu = { "PEPE EL PEPINILLO", "Jugar", "Salir" };
    std::vector<int> tamanosCaracter = { 65, 45, 45, 45 };
    std::vector<sf::Vector2f> posiciones = {
        {ancho / 28, alto / 4},
        {ancho / 8, alto / 4 + 225},
        {ancho / 8, alto / 4 + 325},
        {ancho / 8, alto / 4 + 425}
    };

    for (size_t i = 0; i < opcionesMenu.size(); ++i) {
        sf::Text texto;
        texto.setFont(fuente);
        texto.setString(opcionesMenu[i]);
        texto.setCharacterSize(tamanosCaracter[i]);
        texto.setPosition(posiciones[i]);
        texto.setFillColor(i == seleccionado ? sf::Color(65, 119, 27) : sf::Color(0, 0, 0));
        menuPrincipal.push_back(texto);
    }
}

void Menu::dibujar(sf::RenderWindow& ventana) {
    for (const auto& item : menuPrincipal) {
        ventana.draw(item);
    }
}

void Menu::moverArriba() {
    if (seleccionado > 1) {
        menuPrincipal[seleccionado].setFillColor(sf::Color(0, 0, 0));
        seleccionado--;
        menuPrincipal[seleccionado].setFillColor(sf::Color(65, 119, 27));
    }
}

void Menu::moverAbajo() {
    if (seleccionado < menuPrincipal.size() - 1) {
        menuPrincipal[seleccionado].setFillColor(sf::Color(0, 0, 0));
        seleccionado++;
        menuPrincipal[seleccionado].setFillColor(sf::Color(65, 119, 27));
    }
}

int Menu::obtenerSeleccionado() const {
    return seleccionado;
}

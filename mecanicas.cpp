#include <SFML/Graphics.hpp>
#include <iostream>


class Mecanicas {
private:
    float velocidad = .5f;
    float velocidadY = 0.0f;
    float gravity;

public:
    //Constructor con valor de gravedad como entrada
    Mecanicas(float gravity) : gravity(gravity) {}

    //Funcion de movimientos basicos
    //Utiliza sf::Keyboard::isKeyPressed(sf::Keyboard::key)) para poder mover a pepe, determinando la direccion a base de las tecas wasd
    //Utiliza laa funcion Sprite.move(x,y) de SFML para mover el  objeto deseado

    void movimientoBasico(sf::Sprite& sprite) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            sprite.move(0.f, -velocidad);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            sprite.move(-velocidad, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            sprite.move(0.f, velocidad);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            sprite.move(velocidad, 0.f);
    }

    //Funcion para aplicar gravedad
    void gravedad(sf::Sprite& sprite, sf::RenderWindow& window) {
        velocidadY += gravity;
        sprite.move(0.f, velocidadY);

        //getPosition para determinar posicion de objeto en eje y
        //if para determinarque el sprite este tocando la parte baja de la ventana en eje y para cambiar la velocidad en Y a 0 cuando toque el piso
        if (sprite.getPosition().y + sprite.getLocalBounds().height >= window.getSize().y) {
            velocidadY = 0.0f;
            sprite.setPosition(sprite.getPosition().x, window.getSize().y - sprite.getLocalBounds().height);
        }
    }

};


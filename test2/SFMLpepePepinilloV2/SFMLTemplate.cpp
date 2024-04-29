
#include <iostream>
#include "WindowClass.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <string>
#include <random>
#include <iomanip>
#include <vector>

using namespace std;
using namespace sf;


void mostrarMenu() {
    std::cout << "Bienvenido al juego Pepe Pepinillo!" << std::endl;
    std::cout << "1. Empezar juego" << std::endl;
    std::cout << "2. Salir" << std::endl;
}

int obtenerOpcion() {
    int opcion;
    std::cout << "Seleccione una opcion: ";
    std::cin >> opcion;
    return opcion;
}

class Mecanicas {
private:
    float velocidad = 0.3f;
    float velocidadY = 0.f;
    float velocidadJump = 0.6f;
    float gravity;

public:
    Mecanicas(float gravity) : gravity(gravity) {}

    void movimientoBasico(sf::Sprite& sprite) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            sprite.move(0.f, -velocidadJump);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            sprite.move(-velocidad, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            sprite.move(0.f, velocidad);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            sprite.move(velocidad, 0.f);
    }

    void gravedad(sf::Sprite& sprite, sf::RenderWindow& window) {
        velocidadY += gravity;
        sprite.move(0.f, velocidadY);

        if (sprite.getPosition().y + sprite.getLocalBounds().height >= window.getSize().y) {
            velocidadY = 0.0f;
            sprite.setPosition(sprite.getPosition().x, window.getSize().y - sprite.getLocalBounds().height);
        }
    }

    //Agregado por Esteban
    void resetearVelocidadY() {
        velocidadY = 0.0f;
    }


};

class Objeto {
public:
    sf::Sprite sprite;
    sf::Texture textura;
    Mecanicas* mecanicas;
    bool movible;

    Objeto(const std::string& imagePath, Mecanicas* movimientos, bool movible) : mecanicas(movimientos), movible(movible) {
        if (!textura.loadFromFile(imagePath)) {
            std::cerr << "Error cargando la textura del objeto" << std::endl;
           
            exit(1);
        }
        sprite.setTexture(textura);
        //Agregado por Esteban
        //No funciona
        //float scaleFactor = 3.0f;
        //sprite.setScale(scaleFactor, scaleFactor);
    }

    void ejecutarMovimiento(sf::RenderWindow& window) {
        if (movible) {
            mecanicas->movimientoBasico(sprite);
            mecanicas->gravedad(sprite, window);

            sf::Vector2u windowSize = window.getSize();

            if (sprite.getPosition().x < 0)
                sprite.setPosition(0, sprite.getPosition().y);
            else if (sprite.getPosition().x + sprite.getGlobalBounds().width > windowSize.x)
                sprite.setPosition(windowSize.x - sprite.getGlobalBounds().width, sprite.getPosition().y);

            if (sprite.getPosition().y < 0)
                sprite.setPosition(sprite.getPosition().x, 0);
            else if (sprite.getPosition().y + sprite.getGlobalBounds().height > windowSize.y)
                sprite.setPosition(sprite.getPosition().x, windowSize.y - sprite.getGlobalBounds().height);
        }
    }

    void dibujar(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Personaje : public Objeto {
public:
    //enElSuelo agrgado por Esteban
    bool enElSuelo;

    Personaje(const std::string& imagePath, Mecanicas* movimientos, bool movible) : Objeto(imagePath, movimientos, movible) {
        sprite.setPosition(5.f, 600.f);
    }

    void setEnElSuelo(bool value) {
        enElSuelo = value;
    }

};

class Enemigo : public Personaje {
private:
    bool moveRight = true;
    float speed = 0.05f;
    float initialX;
    float groundY; // Altura del suelo

public:
    Enemigo(const std::string& imagePath, Mecanicas* movimientos, bool movible, float initialXPos, float groundHeight)
        : Personaje(imagePath, movimientos, movible), initialX(initialXPos), groundY(groundHeight) {
        sprite.setPosition(initialX, groundY - sprite.getLocalBounds().height);
    }

    void mover() {
        // Movimirnyo de lado a lado
        if (moveRight) {
            sprite.move(speed, 0.f);
            if (sprite.getPosition().x >= initialX + 100.f) { // Distancia la cual se movera de lado a lado
                moveRight = false;
            }
        }
        else {
            sprite.move(-speed, 0.f);
            if (sprite.getPosition().x <= initialX) {
                moveRight = true;
            }
        }
    }
};

// Clase Bloque para representar el piso

class Bloque{
public:
    sf::Sprite sprite;
    sf::Texture texture;

    Bloque(const std::string& imagePath, float x, float y) {
        if (!texture.loadFromFile(imagePath)) {
            std::cerr << "Error cargando la textura del bloque" << std::endl;
            exit(1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void dibujar(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::FloatRect getGlobalBounds() {
        return sprite.getGlobalBounds();
    }
};


class Nivel {
private:
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    sf::RenderWindow& window;
    std::vector<Bloque> bloques;
    
    //Agregue una camara para el nivel (La vista del juego en la ventana)
    View& camara;

public:
    Nivel(const std::string& backgroundPath, sf::RenderWindow& window, View& camara) : window(window), camara(camara) {
        if (!bgTexture.loadFromFile(backgroundPath)) {
            std::cerr << "Error cargando la textura del fondo del nivel" << std::endl;
            exit(1);
        }

        sf::Vector2u textureSize = bgTexture.getSize();
        float scaleX = window.getSize().x / static_cast<float>(textureSize.x);
        float scaleY = window.getSize().y / static_cast<float>(textureSize.y);

        bgSprite.setScale(scaleX, scaleY);
        bgSprite.setTexture(bgTexture);

    }

    void dibujarFondo(sf::RenderWindow& window) {
        window.draw(bgSprite);
    }

    void dibujarBloques(sf::RenderWindow& window) {
        for (auto& bloque : bloques) {
            bloque.dibujar(window);
        }
    }

    std::vector<Bloque>& getBloques() {
        return bloques;
    }

    void crearBloques() {
        // Crear bloques para el piso
        for (int i = 0; i < 10; ++i) {
            float bloquePosX = i * (640.0f / 10.0f); // Distribuir los bloques uniformemente en el eje x
            Bloque bloque("x64/Debug/imagenes/piso.png", bloquePosX, 480.0f - 50.0f); // Colocar los bloques en la parte inferior de la ventana
            bloques.push_back(bloque);
        }
    }
};

class Juego {
private:
    //Creacion de camara (Vista dentro del nivel)
    View camara;
public:
    //Inicializacion de camara desde el constructor de juego
    //Nota camara(Vector2f(posicionInicioX,posicionInicioY), Vector2f(tamanoX, tamanoY))
    Juego() : window(sf::VideoMode(1200.f, 367.f), "Pepe pepinillo"), camara(Vector2f(300.f, 185.f), Vector2f(600.f, 367.f)) {}

    sf::RenderWindow window;

    void ejecutar() {
        mostrarMenu();
        int opcion = obtenerOpcion();

        switch (opcion) {
        case 1:
            iniciarJuego();
            break;
        case 2:
            std::cout << "Saliendo del juego" << std::endl;
            break;
        default:
            std::cout << "Opcion invalida. Intente de nuevo." << std::endl;
            ejecutar();
            break;
        }
    }

    void iniciarJuego() {
        //Incluye camara
        Nivel nivel("x64/Debug/imagenes/backgroundTestMove.png", window, camara);

        nivel.crearBloques();

        Mecanicas movimientos(0.001f);

        Personaje pepe("x64/Debug/imagenes/pepe.png", &movimientos, true);

        // Crear enemigos
        float groundHeight = 367.f;
        Enemigo enemigo1("x64/Debug/imagenes/malo.png", &movimientos, true, 200.f, groundHeight);
        Enemigo enemigo2("x64/Debug/imagenes/malo.png", &movimientos, true, 400.f, groundHeight);

        while (window.isOpen()) {
            sf::Event evento;
            while (window.pollEvent(evento)) {

                //Eventos para cerrar juego
                if (evento.type == Event::Closed)
                    window.close();
                //ESC para cerrar juego
                if (evento.key.code == Keyboard::Escape && evento.type == Event::KeyPressed)
                    window.close();
            }
            

          //Para que la posicion de la camara siga la posicion del personaje
            Vector2f pepePosicion = pepe.sprite.getPosition();
            if (pepePosicion.x > 300.f && pepePosicion.x < 900.f) {
                camara.setCenter(pepePosicion.x, camara.getCenter().y);
                
            }

           
    
           



            window.clear();

            //Para asignar camara a ventana
            window.setView(camara);

            nivel.dibujarFondo(window);
           
            nivel.dibujarBloques(window); // Dibujar bloques del piso
            pepe.ejecutarMovimiento(window);
            pepe.dibujar(window);

            // Movimiento de los enemigos
            enemigo1.mover();
            enemigo2.mover();

            // Colisión contra los enemigos
            if (pepe.sprite.getGlobalBounds().intersects(enemigo1.sprite.getGlobalBounds()) ||
                pepe.sprite.getGlobalBounds().intersects(enemigo2.sprite.getGlobalBounds())) {
                std::cout << "¡Has perdido!" << std::endl;

                // Reiniciar posición de Pepe
                pepe.sprite.setPosition(5.f, groundHeight - pepe.sprite.getLocalBounds().height);
             
            }


            enemigo1.dibujar(window);
            enemigo2.dibujar(window);

            checkCollisions(nivel.getBloques(), pepe); // Comprobar colisiones con los bloques del piso

            window.display();
        }
    }
    void checkCollisions(std::vector<Bloque>& bloques, Personaje& pepe) {
        for (auto& bloque : bloques) {
            if (pepe.sprite.getGlobalBounds().intersects(bloque.getGlobalBounds())) {
                // Si hay colisión, detener el movimiento vertical del personaje
                pepe.setEnElSuelo(true);
                pepe.sprite.setPosition(pepe.sprite.getPosition().x, bloque.sprite.getPosition().y - pepe.sprite.getGlobalBounds().height * pepe.sprite.getScale().y);
                pepe.mecanicas->gravedad(pepe.sprite, window);
                pepe.mecanicas->resetearVelocidadY();
            }
        }
    }
};

int main() {
    Juego juego;
    juego.ejecutar();

    return 0;
}
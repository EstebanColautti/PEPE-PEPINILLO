//Version1 Pepe pepinillo
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



#include <iostream>
#include <string>

//Menu basico en lo que integramos menu que creo Diego
//Lo creo Ivan
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

//Clase Mecanicas de movimiento
//Lo creo Ivan
class Mecanicas {
private:
    float velocidad = 0.3f;
    float velocidadY = 0.f;
    float velocidadJump = 0.6f;
    float gravity;

public:
    //Constructor con valor de gravedad como entrada
    Mecanicas(float gravity) : gravity(gravity) {}

    //Funcion de movimientos basicos
    //Utiliza sf::Keyboard::isKeyPressed(sf::Keyboard::key)) para poder mover a pepe, determinando la direccion a base de las tecas wasd
    //Utiliza laa funcion Sprite.move(x,y) de SFML para mover el  objeto deseado

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

//Clase Objeto
//Lo creo Jose, Excepto los limetes de el personaje
class Objeto {
public:

    sf::Sprite sprite;
    sf::Texture textura;
    Mecanicas* mecanicas;
    bool movible;

    //Constructor de objeto para poder insertar cualquier imagen, agregar mecanicas, y buleano movible se utilizara en el futuro para cuando integremos objetos estaticos
    //como ladrillos
    Objeto(const std::string& imagePath, Mecanicas* movimientos, bool movible) : mecanicas(movimientos), movible(movible) {
        if (!textura.loadFromFile(imagePath)) {
            std::cerr << "Error cargando la textura del objeto" << std::endl;
            exit(1);
        }
        sprite.setTexture(textura);
    }

    //Funcion para ejecutar mecanicas de movimiento y gravedad a sprite
    void ejecutarMovimiento(sf::RenderWindow& window) {
        if (movible) {
            mecanicas->movimientoBasico(sprite);
            mecanicas->gravedad(sprite, window);

            // Obtiene las dimensiones de la ventana, creando un objeto
            sf::Vector2u windowSize = window.getSize();

            // Esto Verifica los límites de la ventana en X
            if (sprite.getPosition().x < 0)
                sprite.setPosition(0, sprite.getPosition().y); // Ajustar si el personaje está más allá del límite izquierdo
            else if (sprite.getPosition().x + sprite.getGlobalBounds().width > windowSize.x)
                sprite.setPosition(windowSize.x - sprite.getGlobalBounds().width, sprite.getPosition().y); // Ajusta si el personaje está más allá del límite derecho

            // Verificar los límites de la ventana en Y
            if (sprite.getPosition().y < 0)
                sprite.setPosition(sprite.getPosition().x, 0); // Ajustar si el personaje está más allá del límite superior
            else if (sprite.getPosition().y + sprite.getGlobalBounds().height > windowSize.y)
                sprite.setPosition(sprite.getPosition().x, windowSize.y - sprite.getGlobalBounds().height); // Ajusta si el personaje está más allá del límite inferior
        }
    }
    //Funcion para desplegar sprite en ventana
    void dibujar(sf::RenderWindow& window) {
        window.draw(sprite);


    }




};
//La creo Jose
//Clase personaje (Pepe pepinillo: Clase hija de objeto)
class Personaje : public Objeto {
public:
    //Clase constructora Personaje y objeto
    //Determina posicion de Pepe 
    //Toma como entrada el path de la imagen de pepe, las mecanicas y si el objeto es movible
    Personaje(const std::string& imagePath, Mecanicas* movimientos, bool movible) : Objeto(imagePath, movimientos, movible) {
        sprite.setPosition(5.f, 600.f);
    }
};

///Clase Nivel
//La creo Alex, Excepto el reajuste de la ventana(Esteban)
//Encargada de darle un background al nivel
class Nivel {
private:
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    sf::RenderWindow& window;

public:
    Nivel(const std::string& backgroundPath, sf::RenderWindow& window) : window(window) {
        if (!bgTexture.loadFromFile(backgroundPath)) {
            std::cerr << "Error cargando la textura del fondo del nivel" << std::endl;
            exit(1);
        }

        // Calcular el tamaño de la imagen para que se ajuste a la ventana
        sf::Vector2u textureSize = bgTexture.getSize();
        float scaleX = window.getSize().x / static_cast<float>(textureSize.x);
        float scaleY = window.getSize().y / static_cast<float>(textureSize.y);

        // Ajustar la escala de la imagen para que se ajuste a la ventana
        bgSprite.setScale(scaleX, scaleY);
        bgSprite.setTexture(bgTexture);
    }

    void dibujarFondo(sf::RenderWindow& window) {
        window.draw(bgSprite);
    }

};



//Clase Juego
// La hizo Esteban
//Encargada del bucle del juego
class Juego {
public:
    //Constructor para crear ventanta principal del juego
    Juego() : window(sf::VideoMode(612, 367), "Pepe pepinillo") {}






    //Funcion ejecutar, para que corra el Menu
    void ejecutar() {
        mostrarMenu();
        int opcion = obtenerOpcion();

        switch (opcion) {
        case 1:
            //Ejecuta funcion iniciar juego para
            iniciarJuego();
            break;
        case 2:
            std::cout << "Saliendo del juego" << std::endl;
            break;
        default:
            std::cout << "Opcion invalida. Intente de nuevo." << std::endl;
            ejecutar(); // Reiniciar el menú si la opción no es válida
            break;
        }
    }

    //RenderWindow de SFML para cargar ventana
    sf::RenderWindow window;

    //Funcion para empezar el juego
    //creando una instancia de Nivel, Mecanicas y personajes
    void iniciarJuego() {
        Nivel nivel("C:/imagenes/background.jpg", window);

        Mecanicas movimientos(0.001f);

        Personaje pepe("C:/imagenes/pepe.png", &movimientos, true);

        //Bucle de juego
        while (window.isOpen()) {
            sf::Event evento;
            while (window.pollEvent(evento)) {
                if (evento.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            nivel.dibujarFondo(window);
            pepe.ejecutarMovimiento(window);
            pepe.dibujar(window);
            window.display();
        }
    }
};

int main() {
    Juego juego;
    juego.ejecutar();

    return 0;
}

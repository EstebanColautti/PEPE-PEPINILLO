#include <iostream>
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
#include <windows.h>
#include <map>
#include <memory>
#include "Menu.h"
#include "AdministradorDeTexturas.h"

using namespace std;
using namespace sf;

// Variables globales
int numPagina = 1000;
int nivelActual = 1;  // Variable para rastrear el nivel actual

// Clase que manejaba las mecánicas del juego
class Mecanicas {
private:
    float velocidad = 200.0f; // Velocidad caminando
    float velocidadY = 0.f;
    float velocidadSalto = 400.0f; // Velocidad de salto
    float gravedad = 980.0f; // Gravedad
    float aceleracion = 2000.0f; // Aceleración reducida
    float friccion = 2000.0f; // Fricción reducida
    bool saltando = false; // Indicador de salto

public:
    bool debajoDeBloque = false; // Indicador de colisión con bloque superior
    float velocidadX = 0.0f; // Velocidad horizontal

    float obtenerVelocidadY() const {
        return velocidadY;
    }

    float obtenerVelocidad() const {
        return velocidad;
    }

    Mecanicas(float gravedad) : gravedad(gravedad) {}

    // Método que creamos para controlar el movimiento básico del personaje
    void movimientoBasico(sf::Sprite& sprite, float deltaTime) {
        int direccionDeseada = 0;

        // Detectamos si se está presionando una tecla para moverse a la izquierda o derecha
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            direccionDeseada = -1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            direccionDeseada = 1;
        }

        // Aceleramos el movimiento en la dirección deseada
        if (direccionDeseada != 0) {
            velocidadX += direccionDeseada * aceleracion * deltaTime;
            if (velocidadX > velocidad) velocidadX = velocidad;
            if (velocidadX < -velocidad) velocidadX = -velocidad;
        }
        else {
            // Aplicamos fricción si no se está moviendo
            if (velocidadX > 0) {
                velocidadX -= friccion * deltaTime;
                if (velocidadX < 0) velocidadX = 0;
            }
            else if (velocidadX < 0) {
                velocidadX += friccion * deltaTime;
                if (velocidadX > 0) velocidadX = 0;
            }
        }

        // Movemos el sprite horizontalmente
        sprite.move(velocidadX * deltaTime, 0.f);

        // Detectamos si se está presionando la tecla para saltar
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && !saltando) {
            saltar();
            saltando = true;
        }

        // Aplicamos gravedad si no estamos debajo de un bloque
        if (!debajoDeBloque) {
            velocidadY += gravedad * deltaTime;
        }
        else {
            velocidadY = 0.f;
        }

        // Movemos el sprite verticalmente
        sprite.move(0.f, velocidadY * deltaTime);
    }

    // Método para resetear la velocidad vertical
    void resetearVelocidadY() {
        velocidadY = 0.0f;
    }

    // Método para realizar el salto
    void saltar() {
        velocidadY = -velocidadSalto;
    }

    // Método para establecer si el personaje está saltando
    void setSaltando(bool value) {
        saltando = value;
    }
};

// Clase base para todos los objetos en el juego
class Objeto {
public:
    sf::Sprite sprite;
    std::unique_ptr<Mecanicas> mecanicas;
    bool movible;

    // Constructor de la clase Objeto
    Objeto(const std::string& rutaImagen, std::unique_ptr<Mecanicas> movimientos, bool movible, AdministradorDeTexturas& administradorDeTexturas)
        : mecanicas(std::move(movimientos)), movible(movible) {
        sprite.setTexture(*administradorDeTexturas.obtenerTextura(rutaImagen));
    }

    // Constructor alternativo de la clase Objeto
    Objeto(sf::Texture& textura, std::unique_ptr<Mecanicas> movimientos, bool movible)
        : mecanicas(std::move(movimientos)), movible(movible) {
        sprite.setTexture(textura);
    }

    // Método para ejecutar el movimiento del objeto
    virtual void ejecutarMovimiento(sf::RenderWindow& ventana, float deltaTime) {
        if (movible && mecanicas != nullptr) {
            mecanicas->movimientoBasico(sprite, deltaTime);

            sf::Vector2u tamanoVentana = ventana.getSize();

            // Evitamos que el sprite se salga de los límites de la ventana
            if (sprite.getPosition().x < 0)
                sprite.setPosition(0, sprite.getPosition().y);
            else if (sprite.getPosition().x + sprite.getGlobalBounds().width > tamanoVentana.x * 3) // Ajustamos el tamaño de la ventana
                sprite.setPosition(tamanoVentana.x * 3 - sprite.getGlobalBounds().width, sprite.getPosition().y);

            if (sprite.getPosition().y < 0)
                sprite.setPosition(sprite.getPosition().x, 0);
            else if (sprite.getPosition().y + sprite.getGlobalBounds().height > tamanoVentana.y)
                sprite.setPosition(sprite.getPosition().x, tamanoVentana.y - sprite.getGlobalBounds().height);
        }
    }

    // Método para dibujar el objeto en la ventana
    virtual void dibujar(sf::RenderWindow& ventana) {
        ventana.draw(sprite);
    }
};

// Declaración adelantada de la clase Boss
class Boss;

// Clase Personaje que hereda de Objeto
class Personaje : public Objeto {
public:
    bool enElSuelo; // Indicador de si el personaje está en el suelo

    // Constructor de la clase Personaje
    Personaje(const std::string& rutaImagen, std::unique_ptr<Mecanicas> movimientos, bool movible, AdministradorDeTexturas& administradorDeTexturas)
        : Objeto(rutaImagen, std::move(movimientos), movible, administradorDeTexturas), enElSuelo(false) {
        sprite.setPosition(50.f, 350.f);
    }

    // Constructor alternativo de la clase Personaje
    Personaje(sf::Texture& textura, std::unique_ptr<Mecanicas> movimientos, bool movible)
        : Objeto(textura, std::move(movimientos), movible), enElSuelo(false) {
        sprite.setPosition(50.f, 350.f);
    }

    // Método para establecer si el personaje está en el suelo
    void setEnElSuelo(bool value) {
        enElSuelo = value;
        if (value) {
            mecanicas->resetearVelocidadY();
            mecanicas->setSaltando(false);
        }
    }

    // Método para ejecutar el movimiento del personaje
    void ejecutarMovimiento(sf::RenderWindow& ventana, float deltaTime) override {
        mecanicas->movimientoBasico(sprite, deltaTime);

        sf::Vector2u tamanoVentana = ventana.getSize();

        // Evitamos que el sprite se salga de los límites de la ventana
        if (sprite.getPosition().x < 0)
            sprite.setPosition(0, sprite.getPosition().y);
        else if (sprite.getPosition().x + sprite.getGlobalBounds().width > tamanoVentana.x * 3) // Ajuste de nivel más largo
            sprite.setPosition(tamanoVentana.x * 3 - sprite.getGlobalBounds().width, sprite.getPosition().y);

        if (sprite.getPosition().y < 0)
            sprite.setPosition(sprite.getPosition().x, 0);
        else if (sprite.getPosition().y + sprite.getGlobalBounds().height > tamanoVentana.y)
            sprite.setPosition(sprite.getPosition().x, tamanoVentana.y - sprite.getGlobalBounds().height);
    }

    // Método para verificar colisiones con el jefe
    void verificarColisionesConJefe(Boss& jefe);
};

// Clase Enemigo que hereda de Personaje
class Enemigo : public Personaje {
protected:
    float velocidad; // Velocidad del enemigo
    float posicionInicialX;
    float posicionInicialY;
    bool eliminado = false; // Indicador de si el enemigo ha sido eliminado

public:
    // Constructor de la clase Enemigo
    Enemigo(const std::string& rutaImagen, std::unique_ptr<Mecanicas> movimientos, bool movible, float posicionInicialX, float posicionInicialY, AdministradorDeTexturas& administradorDeTexturas, float velocidad)
        : Personaje(rutaImagen, std::move(movimientos), movible, administradorDeTexturas), velocidad(velocidad), posicionInicialX(posicionInicialX), posicionInicialY(posicionInicialY) {
        sprite.setPosition(posicionInicialX, posicionInicialY);
    }

    // Método para marcar al enemigo como eliminado
    void marcarEliminado() {
        eliminado = true;
    }

    // Método para verificar si el enemigo está eliminado
    bool estaEliminado() const {
        return eliminado;
    }

    // Método virtual puro para mover al enemigo
    virtual void mover(float deltaTime) = 0;

    // Método para establecer la escala del sprite del enemigo
    void setEscala(float escalaX, float escalaY) {
        sprite.setScale(escalaX, escalaY);
    }
};

// Clase EnemigoTerrestre que hereda de Enemigo
class EnemigoTerrestre : public Enemigo {
private:
    bool moverDerecha = true; // Indicador de dirección de movimiento

public:
    // Constructor de la clase EnemigoTerrestre
    EnemigoTerrestre(const std::string& rutaImagen, std::unique_ptr<Mecanicas> movimientos, bool movible, float posicionInicialX, float posicionInicialY, AdministradorDeTexturas& administradorDeTexturas, float velocidad)
        : Enemigo(rutaImagen, std::move(movimientos), movible, posicionInicialX, posicionInicialY, administradorDeTexturas, velocidad) {}

    // Método para mover al enemigo terrestre
    void mover(float deltaTime) override {
        if (moverDerecha) {
            sprite.move(velocidad * deltaTime, 0.f);
            if (sprite.getPosition().x >= posicionInicialX + 100.f) {
                moverDerecha = false;
            }
        }
        else {
            sprite.move(-velocidad * deltaTime, 0.f);
            if (sprite.getPosition().x <= posicionInicialX) {
                moverDerecha = true;
            }
        }
    }
};

// Clase EnemigoSaltarin que hereda de Enemigo
class EnemigoSaltarin : public Enemigo {
private:
    bool moverArriba = true; // Indicador de dirección de movimiento vertical

public:
    // Constructor de la clase EnemigoSaltarin
    EnemigoSaltarin(const std::string& rutaImagen, std::unique_ptr<Mecanicas> movimientos, bool movible, float posicionInicialX, float posicionInicialY, AdministradorDeTexturas& administradorDeTexturas, float velocidad)
        : Enemigo(rutaImagen, std::move(movimientos), movible, posicionInicialX, posicionInicialY, administradorDeTexturas, velocidad) {}

    // Método para mover al enemigo saltarín
    void mover(float deltaTime) override {
        if (moverArriba) {
            sprite.move(0.f, -velocidad * deltaTime);
            if (sprite.getPosition().y <= posicionInicialY - 50.f) {
                moverArriba = false;
            }
        }
        else {
            sprite.move(0.f, velocidad * deltaTime);
            if (sprite.getPosition().y >= posicionInicialY + 50.f) {
                moverArriba = true;
            }
        }
    }
};

// Clase EnemigoVolador que hereda de Enemigo
class EnemigoVolador : public Enemigo {
private:
    bool moverDerecha = true;
    bool moverArriba = true;

public:
    // Constructor de la clase EnemigoVolador
    EnemigoVolador(const std::string& rutaImagen, std::unique_ptr<Mecanicas> movimientos, bool movible, float posicionInicialX, float posicionInicialY, AdministradorDeTexturas& administradorDeTexturas, float velocidad)
        : Enemigo(rutaImagen, std::move(movimientos), movible, posicionInicialX, posicionInicialY, administradorDeTexturas, velocidad) {}

    // Método para mover al enemigo volador
    void mover(float deltaTime) override {
        if (moverDerecha) {
            sprite.move(velocidad * deltaTime, 0.f);
            if (sprite.getPosition().x >= posicionInicialX + 100.f) {
                moverDerecha = false;
            }
        }
        else {
            sprite.move(-velocidad * deltaTime, 0.f);
            if (sprite.getPosition().x <= posicionInicialX) {
                moverDerecha = true;
            }
        }

        if (moverArriba) {
            sprite.move(0.f, -velocidad * deltaTime);
            if (sprite.getPosition().y <= posicionInicialY - 50.f) {
                moverArriba = false;
            }
        }
        else {
            sprite.move(0.f, velocidad * deltaTime);
            if (sprite.getPosition().y >= posicionInicialY + 50.f) {
                moverArriba = true;
            }
        }
    }
};

// Clase Boss
class Boss {
private:
    sf::Clock clock;
    float speed;
    int attackPattern;
    bool isImmobilized;
    sf::Clock immobilizedClock;
    int health;
    float limiteEscalerasIzq;
    float limiteEscalerasDer;
    bool moveRight = true;
    bool jumping = false;
    float initialPosY;
    float rangoVision;
    bool eliminado;
    sf::Sprite spriteperro;

public:
    // Constructor de la clase Boss
    Boss(const std::string& nombre, const std::string& rutaImagen, float posX, float posY, AdministradorDeTexturas& administradorDeTexturas, float limiteIzq, float limiteDer)
        : limiteEscalerasIzq(limiteIzq), limiteEscalerasDer(limiteDer), initialPosY(posY), rangoVision(300.0f), health(6), eliminado(false) {
        if (!administradorDeTexturas.cargarTextura(nombre, rutaImagen)) {
            std::cerr << "Error al cargar la textura del jefe: " << rutaImagen << std::endl;
            exit(1);
        }
        spriteperro.setTexture(*administradorDeTexturas.obtenerTextura(nombre));
        spriteperro.setPosition(posX, posY);
        spriteperro.setScale(0.7f, 0.7f); // Ajustar la escala del jefe para hacerlo más pequeño
        speed = 110.0f;
        attackPattern = 0;
        isImmobilized = false;
        health = 6;
    }

    // Método para actualizar la posición del jefe
    void update(float deltaTime, sf::Vector2f playerPos) {
        if (isImmobilized) {
            if (immobilizedClock.getElapsedTime().asSeconds() > 3.0f) {
                isImmobilized = false;
                attackPattern++;
                if (attackPattern >= 6) {
                    attackPattern = 0;
                }
            }
            return;
        }

        sf::Vector2f direction = playerPos - spriteperro.getPosition();
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        direction /= length;

        switch (attackPattern % 3) {
        case 0:
            if (length < rangoVision) {
                spriteperro.move(direction.x * speed * deltaTime, 0.f);
            }
            else {
                if (moveRight) {
                    spriteperro.move(speed * deltaTime, 0.f);
                    if (spriteperro.getPosition().x > limiteEscalerasDer) moveRight = false;
                }
                else {
                    spriteperro.move(-speed * deltaTime, 0.f);
                    if (spriteperro.getPosition().x < limiteEscalerasIzq) moveRight = true;
                }
            }
            break;
        case 1:
            if (!jumping) {
                spriteperro.move(direction.x * speed * deltaTime, -speed * 2 * deltaTime);
                if (spriteperro.getPosition().y <= initialPosY - 100.f) {
                    jumping = true;
                }
            }
            else {
                spriteperro.move(direction.x * speed * deltaTime, speed * 2 * deltaTime);
                if (spriteperro.getPosition().y >= initialPosY) {
                    jumping = false;
                }
            }
            break;
        case 2:
            spriteperro.move(direction * speed * deltaTime * 1.5f);
            break;
        }

        if (spriteperro.getGlobalBounds().intersects(sf::FloatRect(playerPos, sf::Vector2f(50.0f, 50.0f)))) {
            if (playerPos.y + 50.0f <= spriteperro.getPosition().y + 10.0f) {
                perderVida();
                isImmobilized = true;
                immobilizedClock.restart();
                if (health <= 0) {
                    spriteperro.setPosition(-100, -100);
                }
            }
        }
    }

    // Método para dibujar al jefe en la ventana
    void draw(sf::RenderWindow& window) {
        window.draw(spriteperro);
    }

    // Método para resetear la posición del jefe
    void reset() {
        spriteperro.setPosition(2500.0f, 300.0f); // Reemplaza estos valores con la posición inicial adecuada
        health = 6;
        isImmobilized = false;
        attackPattern = 0;
    }

    // Método para reducir la vida del jefe
    void perderVida() {
        health--;
    }

    // Método para obtener la cantidad de vidas del jefe
    int getVidas() {
        return health;
    }

    // Método para verificar si el jefe está eliminado
    bool estaEliminado() const {
        return health <= 0;
    }

    // Método para manejar colisiones con el personaje
    void manejarColisiones(Personaje& personaje) {
        if (spriteperro.getGlobalBounds().intersects(personaje.sprite.getGlobalBounds())) {
            // Colisión desde arriba
            if (personaje.sprite.getPosition().y + personaje.sprite.getGlobalBounds().height <= spriteperro.getPosition().y + 10.f) {
                personaje.mecanicas->saltar();
                personaje.setEnElSuelo(false);
                perderVida();
                isImmobilized = true;
                immobilizedClock.restart();
                if (health <= 0) {
                    spriteperro.setPosition(-100, -100); // Jefe eliminado
                }
            }
            else {
                // Reiniciar la posición del personaje
                personaje.sprite.setPosition(50.f, 316.f); // Ajustar la posición de reinicio de Pepe
                // Reiniciar la posición del jefe y el patrón de ataque
                reiniciarPosicion();
                attackPattern = 0; // Reiniciar el patrón de ataque
                isImmobilized = false; // Reiniciar la inmovilización
                immobilizedClock.restart(); // Reiniciar el reloj de inmovilización
            }
        }
    }

    // Método para reiniciar la posición del jefe
    void reiniciarPosicion() {
        spriteperro.setPosition(2500.0f, 300.0f); // Reemplaza estos valores con la posición inicial adecuada
    }
};

// Clase Bloque que hereda de Objeto
class Bloque : public Objeto {
public:
    // Constructor de la clase Bloque
    Bloque(const std::string& rutaImagen, float x, float y, bool movible, AdministradorDeTexturas& administradorDeTexturas)
        : Objeto(rutaImagen, nullptr, movible, administradorDeTexturas) {
        sprite.setPosition(x, y);
        sprite.setScale(34.f / administradorDeTexturas.obtenerTextura(rutaImagen)->getSize().x, 34.f / administradorDeTexturas.obtenerTextura(rutaImagen)->getSize().y);
    }

    // Constructor de movimiento
    Bloque(Bloque&&) = default;
    Bloque& operator=(Bloque&&) = default;

    // Método para obtener los límites globales del bloque
    sf::FloatRect obtenerLimitesGlobales() {
        return sprite.getGlobalBounds();
    }
};

// Clase PlataformaMovil que hereda de Bloque
class PlataformaMovil : public Bloque {
private:
    float velocidad; // Velocidad de la plataforma
    bool moverDerecha = true;
    float limiteIzquierdo, limiteDerecho;

public:
    // Constructor de la clase PlataformaMovil
    PlataformaMovil(const std::string& rutaImagen, float x, float y, float velocidad, float limiteIzquierdo, float limiteDerecho, AdministradorDeTexturas& administradorDeTexturas)
        : Bloque(rutaImagen, x, y, false, administradorDeTexturas), velocidad(velocidad), limiteIzquierdo(limiteIzquierdo), limiteDerecho(limiteDerecho) {}

    // Método para mover la plataforma
    void mover(float deltaTime) {
        if (moverDerecha) {
            sprite.move(velocidad * deltaTime, 0.f);
            if (sprite.getPosition().x >= limiteDerecho) {
                moverDerecha = false;
            }
        }
        else {
            sprite.move(-velocidad * deltaTime, 0.f);
            if (sprite.getPosition().x <= limiteIzquierdo) {
                moverDerecha = true;
            }
        }
    }

    // Método para obtener la velocidad de la plataforma
    sf::Vector2f obtenerVelocidad() const {
        return moverDerecha ? sf::Vector2f(velocidad, 0.f) : sf::Vector2f(-velocidad, 0.f);
    }
};

// Clase Nivel que maneja múltiples niveles del juego
class Nivel {
private:
    sf::Texture texturaFondo;
    sf::Sprite spriteFondo;
    sf::RenderWindow& ventana;
    std::vector<std::unique_ptr<Bloque>> bloques; // Vector de unique_ptr para los bloques
    std::vector<std::unique_ptr<PlataformaMovil>> plataformasMoviles; // Vector para las plataformas móviles
    View& camara;

    std::unique_ptr<Boss> jefeFinal; // Uso de unique_ptr para el jefe final

public:
    // Constructor de la clase Nivel
    Nivel(const std::string& rutaImagenFondo, sf::RenderWindow& ventana, View& camara, AdministradorDeTexturas& administradorDeTexturas)
        : ventana(ventana), camara(camara) {

        if (!texturaFondo.loadFromFile(rutaImagenFondo)) {
            std::cerr << "Error cargando la textura del fondo desde: " << rutaImagenFondo << std::endl;
            exit(1);
        }

        spriteFondo.setTexture(texturaFondo);

        sf::Vector2u tamanoTextura = texturaFondo.getSize();
        sf::Vector2u tamanoVentana = ventana.getSize();

        float escalaX = static_cast<float>(tamanoVentana.x * 3) / tamanoTextura.x; // Ajuste de nivel más largo
        float escalaY = static_cast<float>(tamanoVentana.y) / tamanoTextura.y;

        float escala = std::max(escalaX, escalaY);
        spriteFondo.setScale(escala, escala);

        // Inicializamos el jefe final
        jefeFinal = std::make_unique<Boss>("Boss1", "C:/PepePepinillo/jefe.png", 2500.0f, 300.0f, administradorDeTexturas, 1900.0f, 2600.0f);
    }

    // Método para dibujar el fondo
    void dibujarFondo(sf::RenderWindow& ventana) {
        ventana.draw(spriteFondo);
    }

    // Método para dibujar los bloques
    void dibujarBloques(sf::RenderWindow& ventana) {
        for (auto& bloque : bloques) {
            bloque->dibujar(ventana);
        }
    }

    // Método para dibujar las plataformas móviles
    void dibujarPlataformasMoviles(sf::RenderWindow& ventana) {
        for (auto& plataforma : plataformasMoviles) {
            plataforma->dibujar(ventana);
        }
    }

    // Método para mover las plataformas móviles
    void moverPlataformasMoviles(float deltaTime) {
        for (auto& plataforma : plataformasMoviles) {
            plataforma->mover(deltaTime);
        }
    }

    // Método para dibujar el jefe final
    void dibujarJefeFinal(sf::RenderWindow& ventana);
    // Método para mover el jefe final
    void moverJefeFinal(float deltaTime, sf::Vector2f posicionJugador);


    std::vector<std::unique_ptr<Bloque>>& obtenerBloques() {
        return bloques;
    }

    std::vector<std::unique_ptr<PlataformaMovil>>& obtenerPlataformasMoviles() {
        return plataformasMoviles;
    }

    std::unique_ptr<Boss>& obtenerJefeFinal() {
        return jefeFinal;
    }

    // Método para crear los bloques del nivel
    void crearBloques(AdministradorDeTexturas& administradorDeTexturas, int nivel, std::vector<std::unique_ptr<Enemigo>>& enemigos) {
        bloques.clear();  // Limpiamos bloques existentes
        plataformasMoviles.clear(); // Limpiamos plataformas móviles existentes
        enemigos.clear(); // Limpiamos enemigos existentes

        // Creamos los bloques para el primer nivel
        switch (nivel) {
        case 1: {
            float anchoBloque = 34.0f;
            float altoBloque = 34.0f;
            float alturaSuelo = 350.0f;
            int numBloques = static_cast<int>(ventana.getSize().x / anchoBloque) + 1;

            // Creamos el piso
            for (int i = 0; i < numBloques * 3; ++i) {
                cout << "Cantidad de bloques: " << i << endl;
                int contadorPiso = i;

                // Agregamos huecos en el piso
                if (contadorPiso > 9 && contadorPiso < 12 || contadorPiso > 20 && contadorPiso < 23 || contadorPiso > 27 && contadorPiso < 30 || contadorPiso > 40 && contadorPiso < 50) {
                    continue;
                }
                float posXBloque = i * anchoBloque;
                bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", posXBloque, alturaSuelo, false, administradorDeTexturas));
            }

            // Creamos bloques flotantes
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 250.0f, 280.0f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 318.0f, 212.0f, false, administradorDeTexturas));

            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 488.0f, 246.0f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 520.0f, 246.0f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 554.0f, 246.0f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 1500.0f, 300.0f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 1550.0f, 300.0f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 1300.f, 150.0f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 1843.f, 284.f, false, administradorDeTexturas));

            // Creamos tres bloques apilados
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 1986.f, 318.f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 1986.f, 284.f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 1986.f, 250.f, false, administradorDeTexturas));

            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 2736.f, 318.f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 2736.f, 284.f, false, administradorDeTexturas));
            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 2736.f, 250.f, false, administradorDeTexturas));

            // Creamos una escalera de bloques
            float posXEscalera = 680.0f;
            float posYEscalera = alturaSuelo + altoBloque;
            int numEscalones = 5;

            for (int i = 0; i < numEscalones; i++) {
                bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", posXEscalera, posYEscalera, false, administradorDeTexturas));
                posXEscalera += anchoBloque;
                posYEscalera -= altoBloque;
            }

            bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", 986.0f, 212.0f, false, administradorDeTexturas));

            // Agregamos plataformas móviles
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 1050.f, 150.f, 100.f, 1360.f, 1700.f, administradorDeTexturas));

            // Creamos los enemigos
            float posYEnemigo = alturaSuelo - 34.f;
            float velocidadEnemigos = 100.0f;

            // Enemigos terrestres
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 100.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 400.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 620.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 1100.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));

            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 1200.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 1350.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 2111.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 2211.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos + 20));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 2311.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos - 20));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 2500.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos + 30));

            // Enemigos voladores
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 1200.f, 200.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 1530.f, 200.f, administradorDeTexturas, velocidadEnemigos));

            // Enemigos saltarines
            enemigos.push_back(std::make_unique<EnemigoSaltarin>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 660.f, 240.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.back()->setEscala(2.0f, 2.0f);
            enemigos.push_back(std::make_unique<EnemigoSaltarin>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 1050.f, 240.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.back()->setEscala(2.0f, 2.0f);
            enemigos.push_back(std::make_unique<EnemigoSaltarin>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 2650.f, 240.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.back()->setEscala(2.0f, 2.0f);

            // Creamos el jefe final
            jefeFinal = std::make_unique<Boss>("Boss1", "C:/PepePepinillo/jefe.png", 2500.0f, 300.0f, administradorDeTexturas, 2000.0f, 2600.0f);

            break;
        }
              // Creamos los bloques para el segundo nivel
        case 2: {
            float anchoBloque = 34.0f;
            float altoBloque = 34.0f;
            float alturaSuelo = 350.0f;
            int numBloques = static_cast<int>(ventana.getSize().x / anchoBloque) + 1;

            for (int i = 0; i < numBloques * 3; ++i) {
                int contadorPiso = i;

                // Agregamos huecos más grandes en el piso
                if (contadorPiso > 10 && contadorPiso < 20 || contadorPiso > 36 && contadorPiso < 90) {
                    continue;
                }
                float posXBloque = i * anchoBloque;
                bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", posXBloque, alturaSuelo, false, administradorDeTexturas));
            }

            // Creamos plataformas flotantes
            float posXPlataforma = 400.0f;
            float posYPlataforma = 300.0f;
            int numPlataformas = 4;

            for (int i = 0; i < numPlataformas; i++) {
                bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", posXPlataforma, posYPlataforma, false, administradorDeTexturas));
                posXPlataforma += anchoBloque;
            }

            posXPlataforma = 1750.0f;
            posYPlataforma = 180.0f;
            numPlataformas = 15;

            for (int i = 0; i < numPlataformas; i++) {
                bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", posXPlataforma, posYPlataforma, false, administradorDeTexturas));
                posXPlataforma += anchoBloque;
            }

            // Creamos escaleras de bloques
            float posXEscalera = 1000.0f;
            float posYEscalera = alturaSuelo + altoBloque;
            int numEscalones = 5;

            for (int i = 0; i < numEscalones; i++) {
                bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", posXEscalera, posYEscalera, false, administradorDeTexturas));
                posXEscalera += anchoBloque;
                posYEscalera -= altoBloque;
            }

            posXEscalera = 2240.0f;
            posYEscalera = 180;
            numEscalones = 3;

            for (int i = 0; i < numEscalones; i++) {
                bloques.push_back(std::make_unique<Bloque>("C:/PepePepinillo/pasto.png", posXEscalera, posYEscalera, false, administradorDeTexturas));
                posXEscalera += anchoBloque;
                posYEscalera -= altoBloque;
            }

            // Agregamos plataformas móviles
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 600.f, 250.f, 90.f, 550.f, 650.f, administradorDeTexturas));
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 1200.f, 200.f, 110.f, 1150.f, 1250.f, administradorDeTexturas));
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 1350.f, 200.f, 110.f, 1300.f, 1400.f, administradorDeTexturas));
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 1550.f, 200.f, 110.f, 1500.f, 1700.f, administradorDeTexturas));
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 2500.f, 200.f, 110.f, 2342.f, 2600.f, administradorDeTexturas));
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 2800.f, 250.f, 150.f, 2700.f, 3000.f, administradorDeTexturas));
            plataformasMoviles.push_back(std::make_unique<PlataformaMovil>("C:/PepePepinillo/plataforma.png", 3200.f, 250.f, 110.f, 3100.f, 3300.f, administradorDeTexturas));

            // Creamos enemigos voladores
            float posYEnemigo = alturaSuelo - 34.f;
            float velocidadEnemigos = 100.0f;

            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 200.f, 200.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 600.f, 150.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 1000.f, 100.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 1400.f, 50.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 1800.f, 0.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 2500.f, 100.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 2850.f, 150.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoVolador>("C:/PepePepinillo/maloVolador.png", std::make_unique<Mecanicas>(980.0f), true, 3000.f, 100.f, administradorDeTexturas, velocidadEnemigos));

            // Creamos enemigos terrestres
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 100.f, posYEnemigo, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 400.f, posYEnemigo - 64, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 2005.f, 145, administradorDeTexturas, velocidadEnemigos));
            enemigos.push_back(std::make_unique<EnemigoTerrestre>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 2105.f, 145, administradorDeTexturas, velocidadEnemigos + 25));
            // Creamos enemigos saltarines
            enemigos.push_back(std::make_unique<EnemigoSaltarin>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 900.f, 240.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.back()->setEscala(2.0f, 2.0f);
            enemigos.push_back(std::make_unique<EnemigoSaltarin>("C:/PepePepinillo/malo.png", std::make_unique<Mecanicas>(980.0f), true, 3120.f, 240.f, administradorDeTexturas, velocidadEnemigos));
            enemigos.back()->setEscala(2.0f, 2.0f);

            break;
        }
        default:
            break;
        }
    }
};

// Clase Juego que gestiona el juego
class Juego {
private:
    View camara;
    int puntaje;
    int enemigosDerrotados;
    int vecesMuerto;
    bool pantallaCompleta = false;
    sf::Clock reloj;
    std::vector<std::unique_ptr<Enemigo>> enemigos;
    AdministradorDeTexturas administradorDeTexturas;
    sf::Text textoPuntaje;
    sf::Font fuentePuntaje;

public:
    Juego();
    sf::RenderWindow ventana;

    // Método para alternar entre pantalla completa y ventana
    void alternarPantallaCompleta() {
        pantallaCompleta = !pantallaCompleta;
        ventana.close();
        if (pantallaCompleta) {
            ventana.create(VideoMode::getDesktopMode(), "Pepe el pepinillo", Style::Fullscreen);
        }
        else {
            ventana.create(VideoMode(1200, 720), "Pepe el pepinillo", Style::Default);
        }
        ventana.setView(camara);

        ventana.setFramerateLimit(60);
        ventana.setVerticalSyncEnabled(true);
    }

    // Método principal para ejecutar el juego
    void ejecutar() {
        RenderWindow ventana(VideoMode(1200, 720), "Juego");
        Menu menu(1200, 720, "C:/PepePepinillo/fuente/Sign Rover Layered.ttf");
        Texture texturaFondoMenu;
        texturaFondoMenu.loadFromFile("C:/PepePepinillo/fondoMenu.png");
        Sprite fondoMenu;
        fondoMenu.setTexture(texturaFondoMenu);

        while (true) {
            if (numPagina == 1000) {
                while (ventana.isOpen()) {
                    Event evento;
                    while (ventana.pollEvent(evento)) {
                        if (evento.type == Event::Closed) {
                            ventana.close();
                            break;
                        }
                        if (evento.type == Event::KeyPressed) {
                            if (evento.key.code == Keyboard::W || evento.key.code == Keyboard::Up) {
                                menu.moverArriba();
                            }
                            if (evento.key.code == Keyboard::S || evento.key.code == Keyboard::Down) {
                                menu.moverAbajo();
                            }
                            if (evento.key.code == Keyboard::Return) {
                                if (menu.obtenerSeleccionado() == 1) {
                                    numPagina = 1;
                                }
                                if (menu.obtenerSeleccionado() == 2) {
                                    numPagina = 2;
                                    std::exit(0); // Termina la ejecución del programa completamente
                                }
                            }
                        }
                    }
                    ventana.clear();
                    if (numPagina != 1000) {
                        break;
                    }
                    ventana.draw(fondoMenu);
                    menu.dibujar(ventana);
                    ventana.display();
                }
                if (numPagina == -1) {
                    ventana.close();
                    break;
                }
                if (numPagina == 1) {
                    ventana.close();
                    iniciarJuego();
                    break;
                }
                if (numPagina == 2) {
                    // Instrucciones
                }
            }
        }
    }

    // Método para iniciar el juego
    void iniciarJuego() {
        iniciarNivel(1);  // Comenzamos con el nivel 1
    }

    // Método para iniciar un nivel específico
    void iniciarNivel(int nivel) {
        bool enPausa = false;
        Nivel nivelJuego("C:/PepePepinillo/fondo.png", ventana, camara, administradorDeTexturas);
        nivelJuego.crearBloques(administradorDeTexturas, nivel, enemigos);

        auto mecanicasPepe = std::make_unique<Mecanicas>(980.0f); // Establecemos la gravedad correctamente
        Personaje pepe("C:/PepePepinillo/pepe.png", std::move(mecanicasPepe), true, administradorDeTexturas);
        pepe.sprite.setPosition(50.f, 316.f); // Ajustamos la posición inicial de Pepe

        while (ventana.isOpen()) {
            sf::Event evento;
            while (ventana.pollEvent(evento)) {
                if (evento.type == Event::Closed)
                    ventana.close();
                if (evento.key.code == Keyboard::Escape && evento.type == Event::KeyPressed) {
                    ventana.close();
                }
                if (evento.key.code == Keyboard::F11 && evento.type == Event::KeyPressed) {
                    alternarPantallaCompleta();
                }
                if (evento.key.code == Keyboard::N && evento.type == Event::KeyPressed) {
                    nivelActual = (nivelActual % 2) + 1;  // Cambiamos de nivel
                    iniciarNivel(nivelActual);
                    return;  // Salimos del bucle y reiniciamos el nivel
                }
            }

            float deltaTime = reloj.restart().asSeconds();

            Vector2f pepePosicion = pepe.sprite.getPosition();
            float limiteDerechoCamara = pepePosicion.x > 300.f ? pepePosicion.x : 300.f;
            float limiteIzquierdoCamara = 3 * ventana.getSize().x - camara.getSize().x / 2; // Ajuste de nivel más largo

            if (limiteDerechoCamara < limiteIzquierdoCamara) {
                camara.setCenter(limiteDerechoCamara, camara.getCenter().y);
            }
            else {
                camara.setCenter(limiteIzquierdoCamara, camara.getCenter().y);
            }

            if (pepePosicion.y > 350) {
                pepe.sprite.setPosition(50.f, 316.f); // Ajustamos la posición de reinicio de Pepe
                camara.setCenter(300, camara.getCenter().y);
            }

            // Cambiamos de nivel cuando Pepe llega al borde derecho de la ventana
            if (pepePosicion.x + pepe.sprite.getGlobalBounds().width >= 3 * ventana.getSize().x) { // Ajuste de nivel más largo
                nivelActual++;
                iniciarNivel(nivelActual);
                return; // Salimos del bucle y reiniciamos el nivel
            }

            actualizarPosicionTextoPuntaje();

            ventana.clear();
            ventana.setView(camara);

            nivelJuego.dibujarFondo(ventana);
            nivelJuego.dibujarBloques(ventana);
            nivelJuego.dibujarPlataformasMoviles(ventana); // Dibujamos las plataformas móviles

            nivelJuego.moverPlataformasMoviles(deltaTime); // Movemos las plataformas móviles

            // Movemos y dibujamos el jefe final
            nivelJuego.moverJefeFinal(deltaTime, pepe.sprite.getPosition());
            nivelJuego.dibujarJefeFinal(ventana);

            // Verificamos colisiones con el jefe
            pepe.verificarColisionesConJefe(*nivelJuego.obtenerJefeFinal());

            pepe.ejecutarMovimiento(ventana, deltaTime);
            pepe.dibujar(ventana);

            for (auto& enemigo : enemigos) {
                enemigo->mover(deltaTime);
                enemigo->dibujar(ventana);
            }

            for (auto& enemigo : enemigos) {
                if (pepe.sprite.getGlobalBounds().intersects(enemigo->sprite.getGlobalBounds())) {
                    if (pepe.sprite.getPosition().y + pepe.sprite.getGlobalBounds().height >= enemigo->sprite.getPosition().y &&
                        pepe.sprite.getPosition().y + pepe.sprite.getGlobalBounds().height <= enemigo->sprite.getPosition().y + enemigo->sprite.getGlobalBounds().height &&
                        pepe.sprite.getPosition().x + pepe.sprite.getGlobalBounds().width >= enemigo->sprite.getPosition().x &&
                        pepe.sprite.getPosition().x <= enemigo->sprite.getPosition().x + enemigo->sprite.getGlobalBounds().width) {
                        enemigo->marcarEliminado();
                        puntaje += 1000;
                        actualizarPuntaje();
                        pepe.mecanicas->saltar();
                        pepe.setEnElSuelo(false); // Nos aseguramos de que Pepe pueda seguir saltando después de eliminar un enemigo
                    }
                    else {
                        pepe.sprite.setPosition(50.f, 316.f); // Ajustamos la posición de reinicio de Pepe
                        camara.setCenter(300, camara.getCenter().y);
                    }
                }
            }

            auto it = enemigos.begin();
            while (it != enemigos.end()) {
                if ((*it)->estaEliminado()) {
                    it = enemigos.erase(it);
                }
                else {
                    ++it;
                }
            }

            verificarColisiones(nivelJuego.obtenerBloques(), nivelJuego.obtenerPlataformasMoviles(), pepe, deltaTime);

            // Dibujamos el puntaje
            ventana.draw(textoPuntaje);

            ventana.display();
        }
    }

    // Método para verificar colisiones
    void verificarColisiones(std::vector<std::unique_ptr<Bloque>>& bloques, std::vector<std::unique_ptr<PlataformaMovil>>& plataformasMoviles, Personaje& pepe, float deltaTime) {
        bool enElSuelo = false;
        bool debajoDeBloque = false;
        bool sobrePlataforma = false;
        sf::Vector2f velocidadPlataforma(0.f, 0.f);

        for (auto& bloque : bloques) {
            if (pepe.sprite.getGlobalBounds().intersects(bloque->obtenerLimitesGlobales())) {
                sf::FloatRect limitesPepe = pepe.sprite.getGlobalBounds();
                sf::FloatRect limitesBloque = bloque->obtenerLimitesGlobales();

                bool colisionSuperior = limitesPepe.top + limitesPepe.height <= limitesBloque.top + 10.f;
                bool colisionInferior = limitesPepe.top >= limitesBloque.top + limitesBloque.height - 10.f;
                bool colisionIzquierda = limitesPepe.left + limitesPepe.width <= limitesBloque.left + 10.f;
                bool colisionDerecha = limitesPepe.left >= limitesBloque.left + limitesBloque.width - 10.f;

                if (colisionSuperior) {
                    pepe.sprite.setPosition(pepe.sprite.getPosition().x, bloque->sprite.getPosition().y - limitesPepe.height);
                    pepe.mecanicas->resetearVelocidadY();
                    enElSuelo = true;
                    pepe.setEnElSuelo(true);
                }
                else if (colisionInferior) {
                    pepe.mecanicas->resetearVelocidadY();
                    pepe.sprite.setPosition(pepe.sprite.getPosition().x, bloque->sprite.getPosition().y + limitesBloque.height);
                    debajoDeBloque = true;
                }
                else if (colisionIzquierda) {
                    pepe.sprite.setPosition(limitesBloque.left - limitesPepe.width, pepe.sprite.getPosition().y);
                }
                else if (colisionDerecha) {
                    pepe.sprite.setPosition(limitesBloque.left + limitesBloque.width, pepe.sprite.getPosition().y);
                }
            }
        }

        for (auto& plataforma : plataformasMoviles) {
            if (pepe.sprite.getGlobalBounds().intersects(plataforma->obtenerLimitesGlobales())) {
                sf::FloatRect limitesPepe = pepe.sprite.getGlobalBounds();
                sf::FloatRect limitesPlataforma = plataforma->obtenerLimitesGlobales();

                bool colisionSuperior = limitesPepe.top + limitesPepe.height <= limitesPlataforma.top + 10.f;
                bool colisionInferior = limitesPepe.top >= limitesPlataforma.top + limitesPlataforma.height - 10.f;
                bool colisionIzquierda = limitesPepe.left + limitesPepe.width <= limitesPlataforma.left + 10.f;
                bool colisionDerecha = limitesPepe.left >= limitesPlataforma.left + limitesPepe.width - 10.f;

                if (colisionSuperior) {
                    pepe.sprite.setPosition(pepe.sprite.getPosition().x, plataforma->sprite.getPosition().y - limitesPepe.height);
                    pepe.mecanicas->resetearVelocidadY();
                    enElSuelo = true;
                    pepe.setEnElSuelo(true);
                    sobrePlataforma = true;
                    velocidadPlataforma = plataforma->obtenerVelocidad();
                }
                else if (colisionInferior) {
                    pepe.mecanicas->resetearVelocidadY();
                    pepe.sprite.setPosition(pepe.sprite.getPosition().x, plataforma->sprite.getPosition().y + limitesPlataforma.height);
                    debajoDeBloque = true;
                }
                else if (colisionIzquierda) {
                    pepe.sprite.setPosition(limitesPlataforma.left - limitesPepe.width, pepe.sprite.getPosition().y);
                }
                else if (colisionDerecha) {
                    pepe.sprite.setPosition(limitesPlataforma.left + limitesPlataforma.width, pepe.sprite.getPosition().y);
                }
            }
        }

        // Permitimos el movimiento lateral del personaje sobre la plataforma móvil
        if (sobrePlataforma && !sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
            !sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            pepe.sprite.move(velocidadPlataforma * deltaTime);
        }

        if (!enElSuelo) {
            pepe.setEnElSuelo(false);
        }

        pepe.mecanicas->debajoDeBloque = debajoDeBloque;
    }

    // Método para cargar texturas
    void cargarTexturas() {
        administradorDeTexturas.cargarTextura("C:/PepePepinillo/pepe.png", "C:/PepePepinillo/pepe.png");
        administradorDeTexturas.cargarTextura("C:/PepePepinillo/malo.png", "C:/PepePepinillo/malo.png");
        administradorDeTexturas.cargarTextura("C:/PepePepinillo/maloVolador.png", "C:/PepePepinillo/maloVolador.png");
        administradorDeTexturas.cargarTextura("C:/PepePepinillo/pasto.png", "C:/PepePepinillo/pasto.png");
        administradorDeTexturas.cargarTextura("C:/PepePepinillo/fondo.png", "C:/PepePepinillo/fondo.png");
        administradorDeTexturas.cargarTextura("C:/PepePepinillo/plataforma.png", "C:/PepePepinillo/plataforma.png"); // Cargamos textura de la plataforma móvil
    }

    // Método para actualizar el puntaje
    void actualizarPuntaje() {
        textoPuntaje.setString("Score: " + std::to_string(puntaje));
    }

    // Método para actualizar la posición del texto de puntaje
    void actualizarPosicionTextoPuntaje() {
        textoPuntaje.setPosition(camara.getCenter().x - camara.getSize().x / 2 + 10, camara.getCenter().y - camara.getSize().y / 2 + 10);
    }
};

// Constructor de la clase Juego
Juego::Juego() : ventana(VideoMode(1200, 720), "Pepe el pepinillo", Style::Default),
camara(Vector2f(300.f, 185.f), Vector2f(600.f, 367.f)),
puntaje(0), enemigosDerrotados(0), vecesMuerto(0) {
    ventana.setView(camara);
    cargarTexturas();

    ventana.setFramerateLimit(60);
    ventana.setVerticalSyncEnabled(true);

    // Cargamos la fuente y configuramos el texto de puntaje
    fuentePuntaje.loadFromFile("C:/PepePepinillo/fuente/Sign Rover Layered.ttf");
    textoPuntaje.setFont(fuentePuntaje);
    textoPuntaje.setCharacterSize(30);
    textoPuntaje.setFillColor(sf::Color::Black);
    actualizarPuntaje();
}

int main() {
    Juego juego;
    juego.ejecutar();

    return 0;
}

// Definimos el método para dibujar el jefe final
void Nivel::dibujarJefeFinal(sf::RenderWindow& ventana) {
    if (jefeFinal) {
        jefeFinal->draw(ventana);
    }
}

// Definimos el método para mover el jefe final
void Nivel::moverJefeFinal(float deltaTime, sf::Vector2f posicionJugador) {
    if (jefeFinal) {
        jefeFinal->update(deltaTime, posicionJugador);
    }
}

// Definimos el método para verificar colisiones con el jefe
void Personaje::verificarColisionesConJefe(Boss& jefe) {
    jefe.manejarColisiones(*this);
}

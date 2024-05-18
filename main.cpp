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
    float velocidad = 0.2f;
    float velocidadY = 0.f;
    float velocidadJump = 0.4f; // La velocidad del salto
    float gravityIncrement = 0.001f; // Incremento de gravedad
    float incrementoVelocidad = 0.001f; // Incremento gradual de velocidad
    float velocidadMaxima = 0.3f; // Velocidad máxima cuando se presiona Shift
    float velocidadActual = 0.0f; // Velocidad actual del personaje
    float desaceleracion = 0.98f; // Factor de desaceleración
    int direccionDeseada = 0; // -1 para izquierda, 1 para derecha, 0 para ninguna
    bool saltando = false;

public:
    bool debajoDeBloque = false; // Para que personaje no pueda brincar mientras este debajo de un bloque

    float getVelocidadY() const {
        return velocidadY;
    }

    float getVelocidad() const {
        return velocidad;
    }

    Mecanicas(float gravity) : gravityIncrement(gravity) {}

    void movimientoBasico(sf::Sprite& sprite) {
        // Verificar si la tecla shift está presionada
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            // Aumentar la velocidad gradualmente hasta el máximo
            if (velocidad < velocidadMaxima) {
                velocidad += incrementoVelocidad;
            }
        }
        else {
            // Restaurar la velocidad original
            velocidad = 0.2f;
        }

        // Determinar la dirección deseada basada en la entrada del usuario
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            direccionDeseada = -1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            direccionDeseada = 1;
        }
        else {
            direccionDeseada = 0;
        }

        // Ajustar la velocidad actual hacia la dirección deseada
        if (direccionDeseada != 0) {
            if ((velocidadActual > 0 && direccionDeseada < 0) || (velocidadActual < 0 && direccionDeseada > 0)) {
                // Si estamos cambiando de dirección, desacelerar primero
                velocidadActual += direccionDeseada * incrementoVelocidad * 2; // Doble incremento para cambio de dirección
            }
            else {
                // Aumentar la velocidad gradualmente en la dirección deseada
                velocidadActual += direccionDeseada * incrementoVelocidad;
            }
            if (velocidadActual > velocidadMaxima) {
                velocidadActual = velocidadMaxima;
            }
            else if (velocidadActual < -velocidadMaxima) {
                velocidadActual = -velocidadMaxima;
            }
        }
        else {
            // Desacelerar gradualmente si no se presiona ninguna tecla de dirección
            velocidadActual *= desaceleracion;
            if (std::abs(velocidadActual) < incrementoVelocidad) {
                velocidadActual = 0.0f;
            }
        }

        // Aplicar el movimiento horizontal al sprite
        sprite.move(velocidadActual, 0.f);

        // Mover hacia arriba solo si no estamos debajo de un bloque y si no estamos ya saltando
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !debajoDeBloque && !saltando) {
            saltar();
            saltando = true;
        }

        // Aplicar la aceleración gravitacional
        velocidadY += gravityIncrement;
        if (velocidadY > velocidadJump) {
            velocidadY = velocidadJump; // Limitar la velocidad de caída a la velocidad del salto
        }
        sprite.move(0.f, velocidadY);
    }

    void gravedad(sf::Sprite& sprite, sf::RenderWindow& window) {
        // Este método ya no es necesario ya que la gravedad se aplica en movimientoBasico
    }

    void resetearVelocidadY() {
        velocidadY = 0.0f;
    }

    void saltar() {
        velocidadY = -velocidadJump;
    }

    void setSaltando(bool value) {
        saltando = value;
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
    }

    Objeto(sf::Texture& texture, Mecanicas* movimientos, bool movible) : textura(texture), mecanicas(movimientos), movible(movible) {
        sprite.setTexture(textura);
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
    bool enElSuelo;

    Personaje(const std::string& imagePath, Mecanicas* movimientos, bool movible) : Objeto(imagePath, movimientos, movible), enElSuelo(false) {
        sprite.setPosition(5.f, 600.f);
    }

    Personaje(sf::Texture& texture, Mecanicas* movimientos, bool movible) : Objeto(texture, movimientos, movible), enElSuelo(false) {
        sprite.setPosition(5.f, 600.f);
    }

    void setEnElSuelo(bool value) {
        enElSuelo = value;
        if (value) {
            mecanicas->resetearVelocidadY();
            mecanicas->setSaltando(false);
        }
    }

    void ejecutarMovimiento(sf::RenderWindow& window) {
        mecanicas->movimientoBasico(sprite);

        if (!enElSuelo) {
            mecanicas->gravedad(sprite, window);
        }

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
};


class Enemigo : public Personaje {
private:
    bool moveRight = true;
    float speed = 0.05f;
    float initialX;
    float groundY;
    bool moveUp = true;
    float initialY;
public:
    void setEscala(float escalaX, float escalaY) {
        sprite.setScale(escalaX, escalaY);
    }

    Enemigo(const std::string& imagePath, Mecanicas* movimientos, bool movible, float initialXPos, float initialYPos)
        : Personaje(imagePath, movimientos, movible), initialX(initialXPos), initialY(initialYPos) {
        sprite.setPosition(initialX, initialY);
    }

    void moverTerrestre() {
        if (moveRight) {
            sprite.move(speed, 0.f);
            if (sprite.getPosition().x >= initialX + 100.f) {
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

    void moverSaltarin() {
        if (moveUp) {
            sprite.move(0.f, speed);
            if (sprite.getPosition().y >= initialY + 50.f) {
                moveUp = false;
            }
        }
        else {
            sprite.move(0.f, -speed);
            if (sprite.getPosition().y <= initialY) {
                moveUp = true;
            }
        }
    }

    void mover() {
        // Movimiento horizontal aleatorio
        if (moveRight) {
            sprite.move(speed, 0.f);
            if (sprite.getPosition().x >= initialX + 100.f) {
                moveRight = false;
            }
        }
        else {
            sprite.move(-speed, 0.f);
            if (sprite.getPosition().x <= initialX) {
                moveRight = true;
            }
        }

        // Movimiento vertical aleatorio
        if (moveUp) {
            sprite.move(0.f, -speed);
            if (sprite.getPosition().y <= initialY - 50.f) {
                moveUp = false;
            }
        }
        else {
            sprite.move(0.f, speed);
            if (sprite.getPosition().y >= initialY + 50.f) {
                moveUp = true;
            }
        }
    }
};

class Bloque : public Objeto {
public:
    Bloque(const std::string& imagePath, float x, float y, bool movible) : Objeto(imagePath, nullptr, movible) {
        if (!textura.loadFromFile(imagePath)) {
            std::cerr << "Error cargando la textura del bloque desde: " << imagePath << std::endl;
            exit(1);
        }
        sprite.setTexture(textura);
        sprite.setPosition(x, y);
        sprite.setScale(34.f / textura.getSize().x, 34.f / textura.getSize().y);
    }

    void dibujar(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::FloatRect getGlobalBounds() {
        return sprite.getGlobalBounds();
    }
};

class Rectangulo : public sf::Sprite {
public:
    Rectangulo(const std::string& imagePath, float x, float y) {
        if (!textura.loadFromFile(imagePath)) {
            std::cerr << "Error cargando la textura del rectángulo desde: " << imagePath << std::endl;
            exit(1);
        }
        setTexture(textura);
        setPosition(x, y);
        // Establecer escala para que el rectángulo tenga un tamaño de 64x64
        setScale(34.f / textura.getSize().x, 34.f / textura.getSize().y);
    }

private:
    sf::Texture textura;
};

class Nivel {
private:
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    sf::RenderWindow& window;
    std::vector<Bloque> bloques;
    View& camara;

public:
    Nivel(const sf::Texture& backgroundTexture, sf::RenderWindow& window, View& camara) : window(window), camara(camara) {
        bgTexture = backgroundTexture;
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
        //Originalmente los bloques eran de 64*64, pero los cambié a 34*34
        float bloqueWidth = 34.0f;
        float bloqueHeight = 34.0f;
        float floorHeight = 350.0f;
        int numBloques = static_cast<int>(window.getSize().x / bloqueWidth) + 1;

        //Bucle para poner piso de nivel
        for (int i = 0; i < numBloques; ++i) {
            //Tengo que averiguar cómo crear huecos para el piso
            int contadorPiso = i; //para crear huecos

            //Este if es para crear huecos del tamaño de 5 bloques
            if (contadorPiso > 5 && contadorPiso < 13 || contadorPiso > 20 && contadorPiso < 30) {
                continue;
            }

            float bloquePosX = i * bloqueWidth;
            Bloque bloque("C:/PepePepinillo/PUTAMADRE.bmp", bloquePosX, floorHeight, false);
            bloques.push_back(bloque);
            cout << "Cantidad de bloques= " << i << endl;
        }
        // Agregar un bloque en el aire
        Bloque bloqueEnAire("C:/PepePepinillo/PUTAMADRE.bmp", 250.0f, 280.0f, false);
        bloques.push_back(bloqueEnAire);

        Bloque bloqueEnAire2("C:/PepePepinillo/PUTAMADRE.bmp", 318.0f, 212.0f, false);
        bloques.push_back(bloqueEnAire2);

        // Crear un rectángulo
        Rectangulo rectangulo("C:/PepePepinillo/rectangulo.png", 250.f, 280.f);

        //Me gustaría poder desplegar escaleras, eventualmente crear función para desplegar escaleras

        // Crear escaleras
        float posicionEscaleraX = 680.0f;
        float posicionEscaleraY = floorHeight + bloqueHeight;
        int numEscalones = 5;

        for (int i = 0; i < numEscalones; i++) {
            Bloque bloqueEscalera("C:/PepePepinillo/PUTAMADRE.bmp", posicionEscaleraX, posicionEscaleraY, false);
            bloques.push_back(bloqueEscalera);

            // Actualizar la posición para el siguiente escalón
            posicionEscaleraX += bloqueWidth;
            posicionEscaleraY -= bloqueHeight;
        }
    }
};

class Juego {
private:
    View camara;
    int puntaje; // Variable para almacenar el puntaje
    int enemigosDerrotados; // Variable para almacenar el numero de enemigos derrotados
    int vecesMuerto; // Variable para almacenar el numero de veces que el personaje ha muerto
    bool pantallaCompleta = true; // Iniciar en pantalla completa por defecto
    sf::Clock reloj; // Reloj para medir el tiempo entre frames

public:
    Juego() : window(VideoMode::getDesktopMode(), "Pepe pepinillo", Style::Fullscreen),
        camara(Vector2f(300.f, 185.f), Vector2f(600.f, 367.f)), puntaje(0), enemigosDerrotados(0), vecesMuerto(0) {
        window.setView(camara);
    }

    sf::RenderWindow window;

    void alternarPantallaCompleta() {
        pantallaCompleta = !pantallaCompleta;
        window.close();
        if (pantallaCompleta) {
            window.create(VideoMode::getDesktopMode(), "Pepe pepinillo", Style::Fullscreen);
        }
        else {
            window.create(VideoMode(1200, 720), "Pepe pepinillo", Style::Default);
        }
        window.setView(camara);
    }

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
        //Booleano para poder pausar juego
        bool enPausa = false;

        // Cargar texturas
        sf::Texture fondoTexture;
        if (!fondoTexture.loadFromFile("C:/PepePepinillo/fondo.png")) {
            std::cerr << "Error cargando la textura del fondo" << std::endl;
            exit(1);
        }

        sf::Texture pepeTexture;
        if (!pepeTexture.loadFromFile("C:/PepePepinillo/pepe.png")) {
            std::cerr << "Error cargando la textura del personaje" << std::endl;
            exit(1);
        }

        sf::Texture enemigoTexture;
        if (!enemigoTexture.loadFromFile("C:/PepePepinillo/malo.png")) {
            std::cerr << "Error cargando la textura del enemigo" << std::endl;
            exit(1);
        }

        sf::Texture bloqueTexture;
        if (!bloqueTexture.loadFromFile("C:/PepePepinillo/PUTAMADRE.bmp")) {
            std::cerr << "Error cargando la textura del enemigo" << std::endl;
            exit(1);
        }

        // Crear el nivel y cargar bloques
        Nivel nivel(fondoTexture, window, camara);
        nivel.crearBloques();

        // Crear objetos del juego
        Mecanicas movimientos(0.001f);
        Personaje pepe(pepeTexture, &movimientos, true);
        // Ajustar la posicion inicial del personaje
        pepe.sprite.setPosition(0.f, window.getSize().y / 2.f);

        float groundHeight = 350.f;
        float enemyPosY = groundHeight - pepe.sprite.getGlobalBounds().height;
        Enemigo* enemigo1 = new Enemigo("C:/PepePepinillo/malo.png", &movimientos, true, 100.f, enemyPosY);
        Enemigo* enemigo2 = new Enemigo("C:/PepePepinillo/malo.png", &movimientos, true, 400.f, enemyPosY);
        Enemigo* enemigo3 = new Enemigo("C:/PepePepinillo/malo.png", &movimientos, true, 620.f, enemyPosY);
        Enemigo* enemigo4 = new Enemigo("C:/PepePepinillo/malo.png", &movimientos, true, 1100.f, enemyPosY);
        Enemigo* enemigoVolador1 = new Enemigo("C:/PepePepinillo/maloVolador.png", &movimientos, true, 600.f, 200.f);
        Enemigo* enemigoVolador2 = new Enemigo("C:/PepePepinillo/maloVolador.png", &movimientos, true, 400.f, 100.f);
        Enemigo* enemigoSaltarin1 = new Enemigo("C:/PepePepinillo/maloSaltarin.png", &movimientos, true, 520.f, 240.f);
        enemigoSaltarin1->setEscala(2.0f, 2.0f); // Cambiar la escala del sprite
        Enemigo* enemigoSaltarin2 = new Enemigo("C:/PepePepinillo/maloSaltarin.png", &movimientos, true, 1050.f, 240.f);
        enemigoSaltarin2->setEscala(2.0f, 2.0f); // Cambiar la escala del sprite

        // Bucle principal del juego
        while (window.isOpen()) {
            sf::Event evento;
            while (window.pollEvent(evento)) {
                if (evento.type == Event::Closed)
                    window.close();
                if (evento.key.code == Keyboard::Escape && evento.type == Event::KeyPressed) {
                    window.close();
                }
                if (evento.key.code == Keyboard::F11 && evento.type == Event::KeyPressed) {
                    alternarPantallaCompleta();
                }
            }

            // Calcular el tiempo transcurrido desde el último frame
            float deltaTime = reloj.restart().asSeconds();

            Vector2f pepePosicion = pepe.sprite.getPosition();
            if (pepePosicion.x > 300.f && pepePosicion.x < 900.f) {
                camara.setCenter(pepePosicion.x, camara.getCenter().y);
            }

            //Pepe muere cuando cae al abismo 
            if (pepePosicion.y > 350) {
                std::cout << "¡Has perdido!" << std::endl;
                pepe.sprite.setPosition(5.f, groundHeight - pepe.sprite.getLocalBounds().height);
                //Reiniciar camara
                camara.setCenter(300, camara.getCenter().y);
            }

            window.clear();
            window.setView(camara);

            nivel.dibujarFondo(window);
            nivel.dibujarBloques(window);

            pepe.ejecutarMovimiento(window);
            pepe.dibujar(window);

            enemigo1->moverTerrestre();
            enemigo2->moverTerrestre();
            enemigo3->moverTerrestre();
            enemigo4->moverTerrestre();

            enemigoVolador1->mover();
            enemigoVolador2->mover();
            enemigoSaltarin1->moverSaltarin();
            enemigoSaltarin2->moverSaltarin();

            //Para interactuar con enemigos
            for (auto& enemigo : { enemigo1, enemigo2, enemigo3, enemigo4, enemigoVolador1, enemigoVolador2, enemigoSaltarin1, enemigoSaltarin2 }) {
                if (pepe.sprite.getGlobalBounds().intersects(enemigo->sprite.getGlobalBounds())) {

                    // Incrementar el puntaje al derrotar al enemigo
                    if (pepe.sprite.getPosition().y + pepe.sprite.getGlobalBounds().height >= enemigo->sprite.getPosition().y &&
                        pepe.sprite.getPosition().y + pepe.sprite.getGlobalBounds().height <= enemigo->sprite.getPosition().y + enemigo->sprite.getGlobalBounds().height &&
                        pepe.sprite.getPosition().x + pepe.sprite.getGlobalBounds().width >= enemigo->sprite.getPosition().x &&
                        pepe.sprite.getPosition().x <= enemigo->sprite.getPosition().x + enemigo->sprite.getGlobalBounds().width) {
                        // El personaje toca la parte superior del enemigo
                        enemigo->sprite.setPosition(-1000.f, -1000.f); // Mover el enemigo fuera de la pantalla
                        puntaje += 1000; // Incrementar el puntaje en 1000
                        std::cout << "¡Has derrotado al enemigo! Score: " << puntaje << std::endl; // Imprimir el puntaje

                        // Hacer saltar a Pepe
                        pepe.mecanicas->saltar();
                    }
                    else {
                        //Muerte de pepe
                        std::cout << "¡Has perdido!" << std::endl;
                        pepe.sprite.setPosition(5.f, groundHeight - pepe.sprite.getLocalBounds().height);
                        //Para reiniciar posicion de camara 
                        camara.setCenter(300, camara.getCenter().y);
                    }
                }
            }

            // Al dibujar los enemigos en el bucle principal del juego
            for (auto& enemigo : { enemigo1, enemigo2, enemigo3, enemigo4, enemigoVolador1, enemigoVolador2, enemigoSaltarin1, enemigoSaltarin2 }) {
                enemigo->dibujar(window);
                enemigoVolador1->dibujar(window);
                enemigoVolador2->dibujar(window);
            }
            checkCollisions(nivel.getBloques(), pepe);
            window.display();
        }
    }

    //Comportamiento de colision de pepe con bloques
    void checkCollisions(std::vector<Bloque>& bloques, Personaje& pepe) {
        for (auto& bloque : bloques) {
            if (pepe.sprite.getGlobalBounds().intersects(bloque.getGlobalBounds())) {
                // Obtener colision del personaje y del bloque
                sf::FloatRect pepeLimites = pepe.sprite.getGlobalBounds();
                sf::FloatRect bloqueLimites = bloque.getGlobalBounds();

                // Detectar en que lado del bloque se hizo la colision
                bool topCollision = pepeLimites.top + pepeLimites.height <= bloqueLimites.top + 10.f;
                bool bottomCollision = pepeLimites.top >= bloqueLimites.top + bloqueLimites.height - 10.f;
                bool leftCollision = pepeLimites.left + pepeLimites.width <= bloqueLimites.left + 10.f;
                bool rightCollision = pepeLimites.left >= bloqueLimites.left + bloqueLimites.width - 10.f;

                // Si la colision es desde arriba
                if (topCollision) {
                    pepe.mecanicas->debajoDeBloque = false;
                    pepe.setEnElSuelo(true);
                    // Ajustar la posicion vertical del personaje para que este justo encima del bloque
                    pepe.sprite.setPosition(pepe.sprite.getPosition().x, bloque.sprite.getPosition().y - pepeLimites.height * pepe.sprite.getScale().y);
                    // Reiniciar la velocidad vertical del personaje
                    pepe.mecanicas->resetearVelocidadY();
                }

                // Si la colision es desde abajo
                else if (bottomCollision) {
                    // Detener el movimiento vertical del personaje
                    pepe.mecanicas->resetearVelocidadY();

                    pepe.mecanicas->resetearVelocidadY();
                    // Ajustar la posicion vertical del personaje para que este justo debajo del bloque
                    pepe.sprite.setPosition(pepe.sprite.getPosition().x, bloque.sprite.getPosition().y + bloqueLimites.height);
                    pepe.mecanicas->debajoDeBloque = true;
                }

                // Si la colision es desde el lado izquierdo
                else if (leftCollision) {
                    // Detener el movimiento horizontal del personaje
                    pepe.sprite.setPosition(bloqueLimites.left - pepeLimites.width * pepe.sprite.getScale().x, pepe.sprite.getPosition().y);
                }

                // Si la colision es desde el lado derecho
                else if (rightCollision) {
                    // Detener el movimiento horizontal del personaje
                    pepe.sprite.setPosition(bloqueLimites.left + bloqueLimites.width, pepe.sprite.getPosition().y);
                }
            }
        }
    }
};

int main() {
    Juego juego;
    juego.ejecutar();

    return 0;
}

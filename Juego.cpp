//Clase Juego
//Encargada del bucle del juego
class Juego {
public:
    //Constructor para crear ventanta principal del juego
    Juego() : window(sf::VideoMode(800, 600), "Pepe pepinillo") { sf::Style::Fullscreen; }

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
        Nivel nivel("C:/PepePepinillo/fondo.jpg", window);

        Mecanicas movimientos(0.001f);

        Personaje pepe("C:/PepePepinillo/pepe.png", &movimientos, true);

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

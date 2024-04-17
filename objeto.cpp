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

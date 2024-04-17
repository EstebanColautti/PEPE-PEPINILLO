
#include <iostream>

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

    void dibujarFondo(sf::RenderWindow& window) {
        window.draw(bgSprite);
    }

};

//Clase Objeto
class Objeto {
public:

    sf::Sprite sprite;
    sf::Texture textura;
    Mecanicas* mecanicas;
    bool movible;

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

        
    }




};

int main()
{
    std::cout<<"Hello World";

    return 0;
}

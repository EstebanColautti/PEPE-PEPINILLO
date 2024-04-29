// SFMLTemplate.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "WindowClass.h"
#include <SFML/Graphics.hpp>

//Idea random para clase objeto, con el proposito de agregar objeto al cuego.
//Que la clase acepte como input el objeto, puede ser un sprite del jugador, y que esta clase pueda tambien usar setPosition para
//Meter el objeto en la posicion deseada de la ventana
//Para la clase nive, parece ser que consistira de la ventana.






int main()
{
    float windowHeight = 400;
    float windowWidth = 400;
    string windowTitle= "testWindow"

    sf::RenderWindow window(sf::VideoMode(windowWidth,windowHeight, windowTitle));
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
    
   
    return 0;
}


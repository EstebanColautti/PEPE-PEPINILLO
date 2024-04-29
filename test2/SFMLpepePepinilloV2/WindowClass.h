#include <SFML/Graphics.hpp>

class Window {

	sf::RenderWindow* window = nullptr;

	void update()
	{
		window->clear();
		window->display();
	}

	void close(){
		delete window;
	}


	void loop() {
		while (window->isOpen()) {
			sf::Event event;
			while (window->pollEvent(event)) {

				if (event.type == sf::Event::Closed) {
					window->close();
				}
			}
			update();
		}
		close();
	}

public:

	void start(unsigned int width, unsigned int height, const char* Title) {

		window = new sf::RenderWindow(sf::VideoMode(width, height), Title);
		loop();


	}

	


};
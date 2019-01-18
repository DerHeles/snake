#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "ResourceManager.hpp"
#include "states/StateManager.hpp"
#include <SFML/Graphics.hpp>

class Application
{
public:
	Application(const std::string& title);
	~Application();

	void run();

private:
	void processEvents();
	void processInput();
	void update(sf::Time elapsedTime);
	void render();

    void setIcon();

    std::shared_ptr<StateManager> m_stateManager;
    std::shared_ptr<ResourceManager> m_resourceManager;
	std::shared_ptr<sf::RenderWindow> m_window;
};

#endif
#ifndef STATE_HPP
#define STATE_HPP

#include "ResourceManager.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

class StateManager;

class State
{
public:
	State(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager);
    virtual ~State();

    virtual void enter();
    virtual void update(sf::Time elapsedTime);
	virtual void processEvents();
	virtual void processInput();
    virtual void render();
	virtual void renderAsNonTopState();
    virtual void exit();
    virtual void setEffectsVolume(float volume);

protected:
	std::shared_ptr<sf::RenderWindow> m_window;
	std::weak_ptr<StateManager> m_stateManager;
    std::shared_ptr<ResourceManager> m_resourceManager;
};

#endif
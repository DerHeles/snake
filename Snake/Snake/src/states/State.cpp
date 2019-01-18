#include "states/State.hpp"
#include "states/StateManager.hpp"

State::State(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager)
    :m_window(window), m_stateManager(stateManager), m_resourceManager(resourceManager)
{
}

State::~State()
{
}

void State::enter()
{
}

void State::update(sf::Time elapsedTime)
{
}

void State::processEvents()
{
	sf::Event event;
	while (m_window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			m_window->close();
			break;
		}
	}
}

void State::processInput()
{
}

void State::render()
{
}

void State::renderAsNonTopState()
{
}

void State::exit()
{
}

void State::setEffectsVolume(float volume)
{
}

#ifndef PAUSE_OVER_STATE_HPP
#define PAUSE_OVER_STATE_HPP

#include "State.hpp"
#include <array>

class PauseState : public State
{
public:
	PauseState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager);
	~PauseState() override;

	void enter() override;
	void update(sf::Time elapsedTime) override;
	void processEvents() override;
	void processInput() override;
	void render() override;
	void renderAsNonTopState() override;
	void exit() override;

private:
    bool setMenuItemSelection(int index);

    int m_currentSelection {0};
    sf::Text m_textInfo;
    std::array<sf::Text, 3> m_textListMenus;
    sf::RectangleShape m_backgroundLayer;
};
#endif
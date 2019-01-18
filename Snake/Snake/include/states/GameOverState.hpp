#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP

#include "State.hpp"
#include <array>

class GameOverState : public State
{
public:
	GameOverState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager);
	~GameOverState() override;

	void enter() override;
	void update(sf::Time elapsedTime) override;
	void processEvents() override;
	void processInput() override;
	void render() override;
	void renderAsNonTopState() override;
	void exit() override;

	void setScore(int score);

private:
    bool setMenuItemSelection(int index);

	int m_score {0};
    int m_currentSelection {0};
	sf::Text m_textInfo;
	sf::Text m_textScore;
	std::array<sf::Text, 2> m_textListMenus;
	sf::RectangleShape m_backgroundLayer;
};
#endif
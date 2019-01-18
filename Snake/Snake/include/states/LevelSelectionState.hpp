#ifndef LEVEL_SELECTION_STATE_HPP
#define LEVEL_SELECTION_STATE_HPP

#include "State.hpp"
#include <array>

class LevelSelectionState : public State
{
public:
	LevelSelectionState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager);
	~LevelSelectionState() override;

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
	sf::Text m_textTitle;
	sf::Text m_textEscape;
    std::array<sf::Sprite, 3> m_selectionSprites;
    std::array<sf::Sprite, 3> m_selectionSpritesSelected;
};
#endif
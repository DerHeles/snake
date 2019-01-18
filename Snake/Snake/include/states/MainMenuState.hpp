#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP

#include "State.hpp"
#include <array>

class MainMenuState : public State
{
public:
    MainMenuState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager);
    ~MainMenuState() override;

    void enter() override;
    void update(sf::Time elapsedTime) override;
    void processEvents() override;
    void processInput() override;
    void render() override;
	void renderAsNonTopState() override;
    void exit() override;

private:
    bool setMenuItemSelection(int index);

    int m_currentSelection{0};
    sf::Text m_textTitle;
    std::array<sf::Text, 3> m_textListMenus;
    sf::Text m_textCopyright;
};
#endif
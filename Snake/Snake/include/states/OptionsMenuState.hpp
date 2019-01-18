#ifndef OPTIONS_MENU_STATE_HPP
#define OPTIONS_MENU_STATE_HPP

#include "State.hpp"
#include <array>

class OptionsMenuState : public State
{
public:
    OptionsMenuState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager);
    ~OptionsMenuState() override;

    void enter() override;
    void update(sf::Time elapsedTime) override;
    void processEvents() override;
    void processInput() override;
    void render() override;
	void renderAsNonTopState() override;
    void exit() override;

    void setEffectsVolume(float volume) override;

private:
    bool setMenuItemSelection(int index);
	void lowerVolume();
	void raiseVolume();

    int m_currentSelection {0};
	int m_musicVolume = 5;
	int m_effectVolume = 5;

    sf::Text m_textTitle;
    sf::Text m_textReturn;
	sf::Text m_textMusic;
	sf::Text m_textEffects;

	std::array<sf::RectangleShape, 10> m_musicVolumeShapes;
	std::array<sf::RectangleShape, 10> m_musicVolumeShapesActive;
	std::array<sf::RectangleShape, 10> m_effectVolumeShapes;
	std::array<sf::RectangleShape, 10> m_effectVolumeShapesActive;

    sf::Sound m_soundVolumeUp;
    sf::Sound m_soundVolumeDown;
};
#endif
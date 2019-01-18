#include "states\OptionsMenuState.hpp"

#include "states/OptionsMenuState.hpp"
#include "states/StateManager.hpp"

#include <iostream>

OptionsMenuState::OptionsMenuState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager)
    :State(window, stateManager, resourceManager)
{
    auto font = resourceManager->getFont("font_menu");
    if (font)
    {
        m_textTitle.setFont(*font);
        m_textTitle.setString("Options");
        m_textTitle.setCharacterSize(44);
        m_textTitle.setLetterSpacing(1.5f);
        m_textTitle.setPosition(400.0f - m_textTitle.getGlobalBounds().width / 2.0f, 140.0f);

		m_textMusic.setFont(*font);
		m_textMusic.setString("music");
		m_textMusic.setCharacterSize(20);
		m_textMusic.setPosition(400.0f - (m_textMusic.getGlobalBounds().width + 188.0f) / 2.0f,
            140.0f + 3.0f * 50.0f - m_textMusic.getGlobalBounds().height / 2.0f);

		m_textEffects.setFont(*font);
		m_textEffects.setString("effects");
		m_textEffects.setCharacterSize(20);
		m_textEffects.setPosition(400.0f - (m_textEffects.getGlobalBounds().width + 188.0f) / 2.0f,
			200.0f + 3.0f * 50.0f - m_textEffects.getGlobalBounds().height / 2.0f);

        m_textReturn.setFont(*font);
        m_textReturn.setString("return");
        m_textReturn.setCharacterSize(20);
        m_textReturn.setPosition(400.0f - m_textReturn.getGlobalBounds().width / 2.0f,
            270.0f + 3.0f * 50.0f - m_textReturn.getGlobalBounds().height / 2.0f);

        // Necessary to calculate the position of the volume shapes
        sf::Vector2f effectTextPosition = m_textEffects.getPosition();
        sf::Vector2f musicTextPosition = m_textMusic.getPosition();
        float effectTextWidth = m_textEffects.getGlobalBounds().width;
        float effectTextHeight = m_textEffects.getGlobalBounds().height;

		for (int i = 0; i < 10; ++i)
		{
			m_effectVolumeShapes[i].setSize(sf::Vector2f(8, 20));
            m_effectVolumeShapes[i].setPosition(effectTextPosition);
			m_effectVolumeShapes[i].move(effectTextWidth + 50.0f + i * 16.0f, static_cast<int>(effectTextHeight / 2.0f) - 10.0f);
			m_effectVolumeShapes[i].setFillColor(sf::Color::Transparent);
			m_effectVolumeShapes[i].setOutlineThickness(2.0f);

            m_effectVolumeShapesActive[i].setSize(sf::Vector2f(8, 20));
            m_effectVolumeShapesActive[i].setPosition(effectTextPosition);
            m_effectVolumeShapesActive[i].move(effectTextWidth + 50.0f + i * 16.0f, static_cast<int>(effectTextHeight / 2.0f) - 10.0f);

            m_musicVolumeShapes[i].setSize(sf::Vector2f(8, 20));
            m_musicVolumeShapes[i].setPosition(effectTextPosition.x, musicTextPosition.y);
            m_musicVolumeShapes[i].move(effectTextWidth + 50.0f + i * 16.0f, static_cast<int>(effectTextHeight / 2.0f) - 10.0f);
            m_musicVolumeShapes[i].setFillColor(sf::Color::Transparent);
            m_musicVolumeShapes[i].setOutlineThickness(2.0f);

            m_musicVolumeShapesActive[i].setSize(sf::Vector2f(8, 20));
            m_musicVolumeShapesActive[i].setPosition(effectTextPosition.x, musicTextPosition.y);
            m_musicVolumeShapesActive[i].move(effectTextWidth + 50.0f + i * 16.0f, static_cast<int>(effectTextHeight / 2.0f) - 10.0f);
		}
    }

    auto soundBuffer = resourceManager->getSoundBuffer("sound_volume_up");
    if (soundBuffer)
        m_soundVolumeUp.setBuffer(*soundBuffer);

    soundBuffer = resourceManager->getSoundBuffer("sound_volume_down");
    if (soundBuffer)
        m_soundVolumeDown.setBuffer(*soundBuffer);
}

OptionsMenuState::~OptionsMenuState()
{
}

void OptionsMenuState::enter()
{
	setMenuItemSelection(0);
}

void OptionsMenuState::update(sf::Time elapsedTime)
{
}

void OptionsMenuState::processEvents()
{
    sf::Event event;
    std::shared_ptr<StateManager> tmpStateManager;

    while (m_window->pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            m_window->close();
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code)
            {
            case sf::Keyboard::Escape:
                tmpStateManager = m_stateManager.lock();
                if (tmpStateManager)
                {
                    tmpStateManager->popState();
                    tmpStateManager->playSoundUIConfirm();
                }
                break;
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                if (setMenuItemSelection(m_currentSelection - 1))
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                        tmpStateManager->playSoundUI();
                }
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if (setMenuItemSelection(m_currentSelection + 1))
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                        tmpStateManager->playSoundUI();
                }
                break;
            case sf::Keyboard::Return:
                if (m_currentSelection == 2)
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
						tmpStateManager->popState();
                        tmpStateManager->playSoundUIConfirm();
                    }
                }
                break;
			case sf::Keyboard::Left:
			case sf::Keyboard::A:
				lowerVolume();
				break;
			case sf::Keyboard::Right:
			case sf::Keyboard::D:
				raiseVolume();
				break;
            }
            break;
        }
    }
}

void OptionsMenuState::processInput()
{
}

void OptionsMenuState::render()
{
    m_window->clear();
	renderAsNonTopState();    
    m_window->display();
}

void OptionsMenuState::renderAsNonTopState()
{
	m_window->draw(m_textTitle);

    // Draw all inactive shapes since the
    // active shapes "fill" the inactive ones
	for (const auto& shape : m_musicVolumeShapes)
		m_window->draw(shape);
	for (const auto& shape : m_effectVolumeShapes)
		m_window->draw(shape);

	for (int i = 0; i < m_musicVolume; ++i)
		m_window->draw(m_musicVolumeShapesActive[i]);
	for (int i = 0; i < m_effectVolume; ++i)
		m_window->draw(m_effectVolumeShapesActive[i]);

	m_window->draw(m_textMusic);
	m_window->draw(m_textEffects);
	m_window->draw(m_textReturn);
}

void OptionsMenuState::exit()
{
}

void OptionsMenuState::setEffectsVolume(float volume)
{
    m_soundVolumeUp.setVolume(volume);
    m_soundVolumeDown.setVolume(volume);
}

bool OptionsMenuState::setMenuItemSelection(int index)
{
    if (index < 0 || index > 2)
        return false;

    // Music
	if (index == 0)
	{
		for (int i = 0; i < 10; ++i)
		{
			m_musicVolumeShapes[i].setOutlineColor(sf::Color::Green);
			m_musicVolumeShapesActive[i].setFillColor(sf::Color::Green);

            m_effectVolumeShapes[i].setOutlineColor(sf::Color::White);
            m_effectVolumeShapesActive[i].setFillColor(sf::Color::White);
		}
		m_textMusic.setFillColor(sf::Color::Green);
		m_textEffects.setFillColor(sf::Color::White);
		m_textReturn.setFillColor(sf::Color::White);
	}
    // Effects
	else if (index == 1)
	{
		for (int i = 0; i < 10; ++i)
		{
			m_musicVolumeShapes[i].setOutlineColor(sf::Color::White);
			m_musicVolumeShapesActive[i].setFillColor(sf::Color::White);

            m_effectVolumeShapes[i].setOutlineColor(sf::Color::Green);
            m_effectVolumeShapesActive[i].setFillColor(sf::Color::Green);
		}
		m_textMusic.setFillColor(sf::Color::White);
		m_textEffects.setFillColor(sf::Color::Green);
		m_textReturn.setFillColor(sf::Color::White);
	}
    // Return
	else if (index == 2)
	{
		for (int i = 0; i < 10; ++i)
		{
			m_musicVolumeShapes[i].setOutlineColor(sf::Color::White);
			m_musicVolumeShapesActive[i].setFillColor(sf::Color::White);
			m_effectVolumeShapes[i].setOutlineColor(sf::Color::White);
			m_effectVolumeShapesActive[i].setFillColor(sf::Color::White);
		}
		m_textMusic.setFillColor(sf::Color::White);
		m_textEffects.setFillColor(sf::Color::White);
		m_textReturn.setFillColor(sf::Color::Green);
	}

    m_currentSelection = index;
    return true;
}

void OptionsMenuState::lowerVolume()
{
    // Music volume
	if (m_currentSelection == 0 && m_musicVolume > 0)
	{
		--m_musicVolume;
        if (m_musicVolume == 0)
            m_textMusic.setStyle(sf::Text::Style::StrikeThrough);

        std::shared_ptr<StateManager> tmpStateManager = m_stateManager.lock();
        if (tmpStateManager)
            tmpStateManager->setMusicVolume(10.f * m_musicVolume);
	}
    // Effects volume
	else if (m_currentSelection == 1 && m_effectVolume > 0)
	{
        m_soundVolumeDown.play();
		--m_effectVolume;
        if (m_effectVolume == 0)
            m_textEffects.setStyle(sf::Text::Style::StrikeThrough);

        std::shared_ptr<StateManager> tmpStateManager = m_stateManager.lock();
        if (tmpStateManager)
            tmpStateManager->setEffectsVolume(10.f * m_effectVolume);
	}
}

void OptionsMenuState::raiseVolume()
{
    // Music volume
	if (m_currentSelection == 0 && m_musicVolume < 10)
	{
        if (m_musicVolume == 0)
            m_textMusic.setStyle(sf::Text::Style::Regular);
		++m_musicVolume;

        std::shared_ptr<StateManager> tmpStateManager = m_stateManager.lock();
        if (tmpStateManager)
            tmpStateManager->setMusicVolume(10.f * m_musicVolume);
	}
    // Effects volume
	else if (m_currentSelection == 1 && m_effectVolume < 10)
	{
        m_soundVolumeUp.play();
        if (m_effectVolume == 0)
            m_textEffects.setStyle(sf::Text::Style::Regular);
		++m_effectVolume;

        std::shared_ptr<StateManager> tmpStateManager = m_stateManager.lock();
        if (tmpStateManager)
            tmpStateManager->setEffectsVolume(10.f * m_effectVolume);
	}
}

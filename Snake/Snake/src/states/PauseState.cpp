#include "states/PauseState.hpp"
#include "states/StateManager.hpp"

PauseState::PauseState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager)
	:State(window, stateManager, resourceManager)
{
	auto font = resourceManager->getFont("font_menu");
	if (font)
	{
        m_textListMenus[0].setString("resume");
        m_textListMenus[1].setString("options");
        m_textListMenus[2].setString("exit");

        int i = 0;
        for (auto& text : m_textListMenus)
        {
            text.setFont(*font);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color(0, 180, 0));
            text.setPosition(400.0f - text.getGlobalBounds().width / 2.0f,
                280.0f + i++ * 50.0f - text.getGlobalBounds().height / 2.0f);
        }
	}

    m_textInfo.setFont(*font);
    m_textInfo.setCharacterSize(38);
    m_textInfo.setFillColor(sf::Color::White);
    m_textInfo.setOutlineColor(sf::Color(0, 120, 0));
    m_textInfo.setString("PAUSED");
	m_textInfo.setPosition(400.0f - m_textInfo.getGlobalBounds().width / 2.0f,
		180.0f);
    
    m_backgroundLayer.setSize(sf::Vector2f(800, 600));
    m_backgroundLayer.setFillColor(sf::Color(0, 0, 0, 220));
}

PauseState::~PauseState()
{
}

void PauseState::enter()
{
	setMenuItemSelection(0);
}

void PauseState::update(sf::Time elapsedTime)
{
}

void PauseState::processEvents()
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
            case sf::Keyboard::Escape:
                tmpStateManager = m_stateManager.lock();
                if (tmpStateManager)
                {
                    tmpStateManager->popState();
                    tmpStateManager->playSoundUIConfirm();
                }
                break;
            case sf::Keyboard::Return:
                // Resume
                if (m_currentSelection == 0)
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
                        tmpStateManager->popState();
                        tmpStateManager->playSoundUIConfirm();
                    }
                }
                // Options
                else if (m_currentSelection == 1)
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
                        tmpStateManager->pushState(StateManager::StateID::Options);
                        tmpStateManager->playSoundUIConfirm();
                    }
                }
                // Exit
                else if (m_currentSelection == 2)
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
                        // Pop PauseState and GameState
                        tmpStateManager->popState();
                        tmpStateManager->replaceCurrentState(StateManager::StateID::MainMenu);
                        tmpStateManager->playSoundUIConfirm();
                    }
                }
                break;
            }
            break;
        }
    }
}

void PauseState::processInput()
{
}

void PauseState::render()
{
	m_window->clear();
	renderAsNonTopState();
	m_window->display();
}

void PauseState::renderAsNonTopState()
{
    m_window->draw(m_backgroundLayer);
    m_window->draw(m_textInfo);
    for (const auto& text : m_textListMenus)
        m_window->draw(text);
}

void PauseState::exit()
{
}

bool PauseState::setMenuItemSelection(int index)
{
    if (index < 0 || index > 2/* || index == m_currentSelection*/)
        return false;

    m_textListMenus[m_currentSelection].setFillColor(sf::Color::White);
    m_currentSelection = index;
    m_textListMenus[m_currentSelection].setFillColor(sf::Color::Green);

    return true;
}

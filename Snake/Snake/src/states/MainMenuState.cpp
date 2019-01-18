#include "states/MainMenuState.hpp"
#include "states/StateManager.hpp"

MainMenuState::MainMenuState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager)
    :State(window, stateManager, resourceManager)
{
    auto font = resourceManager->getFont("font_menu");
    if (font)
    {
        m_textTitle.setFont(*font);
        m_textTitle.setString("SNAKE");
        m_textTitle.setCharacterSize(52);
        m_textTitle.setFillColor(sf::Color::White);
        m_textTitle.setLetterSpacing(1.5f);
        m_textTitle.setPosition(400.0f - m_textTitle.getGlobalBounds().width / 2.0f, 140.0f);

        m_textListMenus[0].setString("play");
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

        m_textCopyright.setFont(*font);
        m_textCopyright.setString("© tobias heiles");
        m_textCopyright.setCharacterSize(14);
        m_textCopyright.setFillColor(sf::Color::White);
        m_textCopyright.setPosition(400.0f - m_textCopyright.getGlobalBounds().width / 2.0f, 550.0f);
    }
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::enter()
{
    setMenuItemSelection(0);
}

void MainMenuState::update(sf::Time elapsedTime)
{
}

void MainMenuState::processEvents()
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
                    {
                        tmpStateManager->playSoundUI();
                    }
                }
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if (setMenuItemSelection(m_currentSelection + 1))
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
                        tmpStateManager->playSoundUI();
                    }
                }
                break;
            case sf::Keyboard::Return:
                // Play
                if (m_currentSelection == 0)
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
                        tmpStateManager->replaceCurrentState(StateManager::StateID::LevelSelection);
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
                    m_window->close();
                    // Playing a sound now would cause a nullptr exception
                    //tmpStateManager->playSoundUIConfirm();
                }
                break;
            }
            break;
		}
	}
}

void MainMenuState::processInput()
{
}

void MainMenuState::render()
{
    m_window->clear();
	renderAsNonTopState();    
    m_window->display();
}

void MainMenuState::renderAsNonTopState()
{
	m_window->draw(m_textTitle);
	for (const auto& text : m_textListMenus)
		m_window->draw(text);
	m_window->draw(m_textCopyright);
}

void MainMenuState::exit()
{
}

bool MainMenuState::setMenuItemSelection(int index)
{
    if (index < 0 || index > 2/* || index == m_currentSelection*/)
        return false;

    m_textListMenus[m_currentSelection].setFillColor(sf::Color::White);
    m_currentSelection = index;
    m_textListMenus[m_currentSelection].setFillColor(sf::Color::Green);

    return true;
}
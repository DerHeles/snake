#include "states/GameOverState.hpp"
#include "states/StateManager.hpp"

GameOverState::GameOverState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager)
	:State(window, stateManager, resourceManager)
{
	auto font = resourceManager->getFont("font_menu");
	if (font)
	{
        m_textListMenus[0].setString("play again");
        m_textListMenus[1].setString("exit");

        m_textScore.setFont(*font);
        m_textScore.setCharacterSize(22);
        m_textScore.setFillColor(sf::Color::White);
        m_textScore.setStyle(sf::Text::Style::Italic);

        int i = 1;
        for (auto& text : m_textListMenus)
        {
            text.setFont(*font);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            text.setPosition(400.0f - text.getGlobalBounds().width / 2.0f,
                300.0f + i++ * 50.0f - text.getGlobalBounds().height / 2.0f);
        }

        m_textInfo.setFont(*font);
        m_textInfo.setCharacterSize(38);
        m_textInfo.setFillColor(sf::Color::White);
        m_textInfo.setOutlineColor(sf::Color(0, 120, 0));
        m_textInfo.setString("GAME OVER");
        m_textInfo.setPosition(400.0f - m_textInfo.getGlobalBounds().width / 2.0f,
            180.0f);
	}

	m_backgroundLayer.setSize(sf::Vector2f(800, 600));
	m_backgroundLayer.setFillColor(sf::Color(25, 0, 0, 220));
}

GameOverState::~GameOverState()
{
}

void GameOverState::enter()
{
    setMenuItemSelection(0);
    setScore(0);
}

void GameOverState::update(sf::Time elapsedTime)
{
}

void GameOverState::processEvents()
{
	sf::Event event;
    std::shared_ptr<StateManager> tmpStateManager;

	while (m_window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                if (setMenuItemSelection(m_currentSelection - 1))
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                        tmpStateManager->playSoundUIConfirm();
                }
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                if (setMenuItemSelection(m_currentSelection + 1))
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                        tmpStateManager->playSoundUIConfirm();
                }
                break;
            case sf::Keyboard::Return:
                // Play again
                if (m_currentSelection == 0)
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
                        // Remove GameOverState and then replace the GameState with a new one
                        tmpStateManager->popState();
                        tmpStateManager->replaceCurrentState(StateManager::StateID::Game);
                        tmpStateManager->playSoundUIConfirm();
                    }
                }
                // Exit
                else if (m_currentSelection == 1)
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                    {
                        // Pop Pause and GameState
                        tmpStateManager->popState();
                        tmpStateManager->replaceCurrentState(StateManager::StateID::MainMenu);
                        tmpStateManager->playSoundUIConfirm();
                    }
                }
                break;
			};
			break;
		case sf::Event::Closed:
			m_window->close();
			break;
		}
	}
}

void GameOverState::processInput()
{
}

void GameOverState::render()
{
	m_window->clear();
    renderAsNonTopState();
	m_window->display();
}

void GameOverState::renderAsNonTopState()
{
	m_window->draw(m_backgroundLayer);
    m_window->draw(m_textInfo);
    m_window->draw(m_textScore);

    for (const auto& text : m_textListMenus)
        m_window->draw(text);
}

void GameOverState::exit()
{
}

void GameOverState::setScore(int score)
{
	m_score = score;
	m_textScore.setString("Your score: " + std::to_string(score));
    m_textScore.setPosition(400.0f - m_textScore.getGlobalBounds().width / 2.0f,
        290.0f - m_textScore.getGlobalBounds().height / 2.0f);
}

bool GameOverState::setMenuItemSelection(int index)
{
    if (index < 0 || index > 1)
        return false;

    m_textListMenus[m_currentSelection].setFillColor(sf::Color::White);
    m_currentSelection = index;
    m_textListMenus[m_currentSelection].setFillColor(sf::Color::Green);

    return true;
}

#include "states/LevelSelectionState.hpp"
#include "states/StateManager.hpp"

LevelSelectionState::LevelSelectionState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager)
	:State(window, stateManager, resourceManager)
{
	auto font = resourceManager->getFont("font_menu");
	if (font)
	{
		m_textTitle.setFont(*font);
		m_textTitle.setString("Select Level");
		m_textTitle.setCharacterSize(28);
		m_textTitle.setFillColor(sf::Color::White);
		m_textTitle.setLetterSpacing(1.5f);
		m_textTitle.setPosition(400.0f - m_textTitle.getGlobalBounds().width / 2.0f, 120.0f);

		m_textEscape.setFont(*font);
		m_textEscape.setString("ESC to return");
		m_textEscape.setCharacterSize(14);
		m_textEscape.setFillColor(sf::Color::White);
		m_textEscape.setPosition(400.0f - m_textEscape.getGlobalBounds().width / 2.0f, 520.0f);
	}

    // Setup sprites
    auto tex1 = m_resourceManager->getTexture("level1");
    auto tex2 = m_resourceManager->getTexture("level2");
    auto tex3 = m_resourceManager->getTexture("level3");

    if (tex1)
        m_selectionSprites[0].setTexture(*tex1);
    if (tex2)
        m_selectionSprites[1].setTexture(*tex2);
    if (tex3)
        m_selectionSprites[2].setTexture(*tex3);

    m_selectionSprites[0].setPosition(54.0f, 230.0f);
    m_selectionSprites[1].setPosition(302.0f, 230.0f);
    m_selectionSprites[2].setPosition(550.0f, 230.0f);

    // Setup selected sprites
    auto tex1Selected = m_resourceManager->getTexture("level1_selected");
    auto tex2Selected = m_resourceManager->getTexture("level2_selected");
    auto tex3Selected = m_resourceManager->getTexture("level3_selected");

    if (tex1Selected)
        m_selectionSpritesSelected[0].setTexture(*tex1Selected);
    if (tex2Selected)
        m_selectionSpritesSelected[1].setTexture(*tex2Selected);
    if (tex3Selected)
        m_selectionSpritesSelected[2].setTexture(*tex3Selected);

    m_selectionSpritesSelected[0].setPosition(54.0f, 230.0f);
    m_selectionSpritesSelected[1].setPosition(302.0f, 230.0f);
    m_selectionSpritesSelected[2].setPosition(550.0f, 230.0f);
}

LevelSelectionState::~LevelSelectionState()
{
}

void LevelSelectionState::enter()
{
    setMenuItemSelection(0);
}

void LevelSelectionState::update(sf::Time elapsedTime)
{
}

void LevelSelectionState::processEvents()
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
			case sf::Keyboard::Left:
			case sf::Keyboard::A:
                if (setMenuItemSelection(m_currentSelection - 1))
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                        tmpStateManager->playSoundUI();
                }
				break;
			case sf::Keyboard::Right:
            case sf::Keyboard::D:
                if (setMenuItemSelection(m_currentSelection + 1))
                {
                    tmpStateManager = m_stateManager.lock();
                    if (tmpStateManager)
                        tmpStateManager->playSoundUI();
                }
				break;
			case sf::Keyboard::Return:
				// Play
                tmpStateManager = m_stateManager.lock();
                if (tmpStateManager)
                {
                    //tmpStateManager->changeToState(StateManager::StateID::Game);
                    tmpStateManager->setNextLevelForGameState(m_currentSelection);
                    tmpStateManager->replaceCurrentState(StateManager::StateID::Game);
                    tmpStateManager->playSoundUIConfirm();
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
				}
				break;
            case sf::Keyboard::Escape:
                tmpStateManager = m_stateManager.lock();
                if (tmpStateManager)
                {
                    tmpStateManager->replaceCurrentState(StateManager::StateID::MainMenu);
                    tmpStateManager->playSoundUIConfirm();
                }
                break;
			}
			break;
		}
	}
}

void LevelSelectionState::processInput()
{
}

void LevelSelectionState::render()
{
	m_window->clear();
	renderAsNonTopState();
	m_window->display();
}

void LevelSelectionState::renderAsNonTopState()
{
	m_window->draw(m_textTitle);

    if (m_currentSelection == 0)
    {
        m_window->draw(m_selectionSpritesSelected[0]);
        m_window->draw(m_selectionSprites[1]);
        m_window->draw(m_selectionSprites[2]);
    }
    else if (m_currentSelection == 1)
    {
        m_window->draw(m_selectionSprites[0]);
        m_window->draw(m_selectionSpritesSelected[1]);
        m_window->draw(m_selectionSprites[2]);
    }
    else if (m_currentSelection == 2)
    {
        m_window->draw(m_selectionSprites[0]);
        m_window->draw(m_selectionSprites[1]);
        m_window->draw(m_selectionSpritesSelected[2]);
    }

	m_window->draw(m_textEscape);
}

void LevelSelectionState::exit()
{
}

bool LevelSelectionState::setMenuItemSelection(int index)
{
	if (index < 0 || index > 2/* || index == m_currentSelection*/)
		return false;

	m_currentSelection = index;

    return true;
}

#include "states/GameState.hpp"
#include "states/StateManager.hpp"
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;
using std::cerr;

GameState::GameState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager)
    :State(window, stateManager, resourceManager),
    m_superFoodDistribution(0, 99)
{
    buildLevels();

    auto font = resourceManager->getFont("font_menu");
    if (font)
    {
        m_scoreText.setFont(*font);
        m_scoreText.setString("0000");
        m_scoreText.setCharacterSize(26);
        m_scoreText.setOutlineThickness(3.0f);
	    m_scoreText.setPosition(400.0f - m_scoreText.getGlobalBounds().width / 2.0f, 25.0f);
    }
    else
        std::cerr << "GameState ctor: error getting font!" << std::endl;

    auto soundBuffer = resourceManager->getSoundBuffer("sound_food");
    if (soundBuffer)
    {
        m_soundFood.setBuffer(*soundBuffer);
    }

    soundBuffer = resourceManager->getSoundBuffer("sound_food_big");
    if (soundBuffer)
    {
        m_soundFoodBig.setBuffer(*soundBuffer);
    }

    soundBuffer = resourceManager->getSoundBuffer("sound_teleport");
    if (soundBuffer)
    {
        m_soundTeleport.setBuffer(*soundBuffer);
    }

    soundBuffer = resourceManager->getSoundBuffer("sound_movement");
    if (soundBuffer)
    {
        m_soundMovement.setBuffer(*soundBuffer);
    }

    soundBuffer = resourceManager->getSoundBuffer("sound_movement_growing");
    if (soundBuffer)
    {
        m_soundMovementGrowing.setBuffer(*soundBuffer);
    }

    soundBuffer = resourceManager->getSoundBuffer("sound_collision");
    if (soundBuffer)
    {
        m_soundCollision.setBuffer(*soundBuffer);
    }

	m_scoreUIShape.setFillColor(sf::Color(30, 230, 230));
	m_scoreUIShape.setSize(sf::Vector2f(120.0f, 50.f));
	m_scoreUIShape.setOutlineThickness(8.0f);
	m_scoreUIShape.setOutlineColor(sf::Color::Black);
	sf::Vector2f pos = m_scoreText.getPosition();
	pos.x -= m_scoreUIShape.getSize().x / 2.0f;
	pos.x += m_scoreText.getGlobalBounds().width / 2.0f;
	pos.y -= m_scoreUIShape.getSize().y / 2.0f;
	pos.y += m_scoreText.getGlobalBounds().height / 2.0f - m_scoreUIShape.getOutlineThickness() / 4.0f;
	m_scoreUIShape.setPosition(pos);

    m_gameOverEffect.setFillColor(sf::Color(255, 0, 0, 64));
    m_gameOverEffect.setSize(sf::Vector2f(800, 600));

    m_randomGenerator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

GameState::~GameState()
{
}

void GameState::processEvents()
{
	std::shared_ptr<StateManager> tmpStateManager;
    sf::Event event;

    while (m_window->pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::LostFocus:
            tmpStateManager = m_stateManager.lock();
            if (tmpStateManager)
                tmpStateManager->pushState(StateManager::StateID::Pause, true);
            break;
        case sf::Event::KeyPressed:
            switch(event.key.code)
            {
            case sf::Keyboard::W:
            case sf::Keyboard::Up:
                if (m_lastMovement != PlayerInput::Down)
                    m_lastInput = PlayerInput::Up;
                break;
            case sf::Keyboard::S:
            case sf::Keyboard::Down:
                if (m_lastMovement != PlayerInput::Up)
                    m_lastInput = PlayerInput::Down;
                break;
            case sf::Keyboard::A:
            case sf::Keyboard::Left:
                if (m_lastMovement != PlayerInput::Right)
                    m_lastInput = PlayerInput::Left;
                break;
            case sf::Keyboard::D:
            case sf::Keyboard::Right:
                if (m_lastMovement != PlayerInput::Left)
                    m_lastInput = PlayerInput::Right;
                break;
            case sf::Keyboard::Escape:
				tmpStateManager = m_stateManager.lock();
				if (tmpStateManager)
				{
					tmpStateManager->pushState(StateManager::StateID::Pause, true);
                    tmpStateManager->playSoundUIConfirm();
				}
                break;
            }
            break;
        case sf::Event::Closed:
            m_window->close();
            break;
        }
    }
}

void GameState::processInput()
{

}

void GameState::enter()
{
    setScore(0);
    loadLevel(m_nextLevelIndex);

    m_lastInput = PlayerInput::Up;
    m_lastMovement = PlayerInput::Up;
    m_snakeIsDead = false;
    m_remainingFoodSpawnCooldown = 0;
    m_foodInLevel = 0;
    m_remainingGrowthTicks = 0;

    m_snakeClock.restart();
}

void GameState::update(sf::Time elapsedTime)
{
    if (m_snakeIsDead)
        return;

	m_snakeTime += elapsedTime;

    while (m_snakeTime >= TIME_PER_SNAKE_MOVEMENT)
    {
    	moveSnake();
    	m_snakeTime -= TIME_PER_SNAKE_MOVEMENT;

        // Spawn food every x ticks up to a certain maximum
        if (0 < m_remainingFoodSpawnCooldown)
            --m_remainingFoodSpawnCooldown;

        if(0 >= m_remainingFoodSpawnCooldown && m_foodInLevel < m_maxFoodInLevel)
        {
            std::vector<sf::Vector2i> listOfEmptyTiles;
            m_gameGrid.getListOfEmptyTiles(&listOfEmptyTiles);
            if(!listOfEmptyTiles.empty())
            {
                // Get random tile to spawn the food on
                std::uniform_int_distribution<int> distribution(0, listOfEmptyTiles.size()-1);
                sf::Vector2i chosenTile = listOfEmptyTiles[distribution(m_randomGenerator)];
                ++m_foodInLevel;

                // 5 % chance for spawning a super food
                if(m_superFoodDistribution(m_randomGenerator) < 95)
                    m_gameGrid.updateTile(chosenTile, GameGrid::TileValue::Food);
                else
                    m_gameGrid.updateTile(chosenTile, GameGrid::TileValue::SuperFood);

                // Reset cooldown only in case of successful spawn
                m_remainingFoodSpawnCooldown = m_foodSpawnCooldown;

            }
        }
    }
}

void GameState::render()
{
    m_window->clear();
	renderAsNonTopState();
    m_window->display();
}

void GameState::renderAsNonTopState()
{
	m_window->draw(m_gameGrid);
	m_window->draw(m_scoreUIShape);
	m_window->draw(m_scoreText);
}

void GameState::exit()
{
}

void GameState::setNextLevel(int levelIndex)
{
    if (levelIndex < 0 || levelIndex > 2)
    {
        std::cerr << "GameState::setNexLevel(): " << levelIndex << " is no valid level!" << std::endl;
        return;
    }
    m_nextLevelIndex = levelIndex;
}

void GameState::setEffectsVolume(float volume)
{
    m_soundFood.setVolume(volume);
    m_soundFoodBig.setVolume(volume);
    m_soundTeleport.setVolume(volume);
    m_soundMovement.setVolume(volume);
    m_soundMovementGrowing.setVolume(volume);
    m_soundCollision.setVolume(volume);
}

void GameState::buildLevels()
{
    // Level 1
    m_levels[0].tileSize = sf::Vector2u(25, 25);
	m_levels[0].width = 32;
	m_levels[0].height = 24;
    m_levels[0].startPosition = sf::Vector2i(17, 18);
    m_levels[0].gridData = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    // Level 2
    m_levels[1].tileSize = sf::Vector2u(20, 20);
	m_levels[1].width = 40;
	m_levels[1].height = 30;
    m_levels[1].startPosition = sf::Vector2i(10, 20);
    m_levels[1].gridData = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 7, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 7, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 8, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };

    // Level 3
    m_levels[2].tileSize = sf::Vector2u(20, 20);
	m_levels[2].width = 40;
	m_levels[2].height = 30;
    m_levels[2].startPosition = sf::Vector2i(6, 18);
    m_levels[2].gridData = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 1, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 1, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 1, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 1, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 1, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 1, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 1, 1, 1, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 1, 1, 1, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 7, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 7, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 1, 9, 9, 1, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
}

void GameState::loadLevel(int levelIndex)
{
	m_snake.clear();

    m_gameGrid.loadFromArray(m_levels[levelIndex].gridData, m_levels[levelIndex].width,
		m_levels[levelIndex].height, m_levels[levelIndex].tileSize);
       
    m_snake.emplace_back(m_levels[levelIndex].startPosition);
    m_snake.emplace_back(m_levels[levelIndex].startPosition + sf::Vector2i(0, 1));
    m_snake.emplace_back(m_levels[levelIndex].startPosition + sf::Vector2i(0, 2));

	m_gameGrid.updateTile(m_snake[0].x, m_snake[0].y, GameGrid::TileValue::SnakeHead);
	m_gameGrid.updateTile(m_snake[1].x, m_snake[1].y, GameGrid::TileValue::SnakeBody);
	m_gameGrid.updateTile(m_snake[2].x, m_snake[2].y, GameGrid::TileValue::SnakeBody);
}

void GameState::moveSnake()
{
    // Only move if snake is at least 2 parts long and is not dead
    if (!isSnakeValid() || m_snakeIsDead)
        return;

    sf::Vector2i movement;
    GameGrid::TileValue tileValue;
    bool isValidTileValue = false;
    bool snakeDied = false;
    int snakeGrowth = 0;

    switch (m_lastInput)
    {
    case PlayerInput::Up:
        --movement.y;
        break;
    case PlayerInput::Down:
        ++movement.y;
        break;

    case PlayerInput::Left:
        --movement.x;
        break;

    case PlayerInput::Right:
        ++movement.x;
        break;

    default:
        // invalid state, do nothing
        return;
    }

    // Only move if next position is valid
    if (m_gameGrid.getTileValueAt(m_snake[0] + movement, &tileValue))
    {
        // Snake grows x ticks after eating food
        if (m_remainingGrowthTicks > 0)
        {
            m_soundMovementGrowing.play();
            --m_remainingGrowthTicks;
        }

        // Check for next position's tile value
        switch (tileValue)
        {
        case GameGrid::TileValue::Empty:
            // Do nothing, only move snake
            break;
        case GameGrid::TileValue::Wall:
        case GameGrid::TileValue::SnakeBody:
            snakeDied = true;
            m_soundCollision.play();
            break;
        case GameGrid::TileValue::SnakeHead:
            snakeDied = true;
            m_soundCollision.play();
            // Note: this shouldn't be possible with only one snake on the board
            // -> possible multiplayer extension
            break;
        case GameGrid::TileValue::Food:
            m_soundFood.play();
            --m_foodInLevel;
            setScore(m_score + 1);
            snakeGrowth = 1;
            break;
        case GameGrid::TileValue::SuperFood:
            m_soundFoodBig.play();
            --m_foodInLevel;
            setScore(m_score + 5);
            snakeGrowth = 5;
            break;
        case GameGrid::TileValue::Poison:
            // Trigger posion effects (changed controls, other effects)
            break;
        case GameGrid::TileValue::Teleport1:
            {
                auto otherTeleporter = m_gameGrid.getPositionOfOtherTeleporter1(m_snake[0] + movement);
                if (otherTeleporter)
                {
                    m_soundTeleport.play();

                    // Teleport the snake by adding an offset
                    movement += *otherTeleporter - m_snake[0];

                    // Check for collision on other telporter's side
                    if (m_gameGrid.getTileValueAt(m_snake[0] + movement, &tileValue))
                    {
                        switch (tileValue)
                        {
                        case GameGrid::TileValue::Wall:
                        case GameGrid::TileValue::SnakeBody:
                        case GameGrid::TileValue::SnakeHead:
                        case GameGrid::TileValue::NoPlayArea:
                            snakeDied = true;
                            m_soundCollision.play();
                            break;
                        case GameGrid::TileValue::Food:
                            --m_foodInLevel;
                            m_soundFood.play();
                            setScore(m_score + 1);
                            snakeGrowth = 1;
                            break;
                        case GameGrid::TileValue::SuperFood:
                            --m_foodInLevel;
                            m_soundFoodBig.play();
                            setScore(m_score + 5);
                            snakeGrowth = 5;
                            break;
                        }
                    }
                }
            }
            break;
        case GameGrid::TileValue::Teleport2:
            {
                auto otherTeleporter = m_gameGrid.getPositionOfOtherTeleporter2(m_snake[0] + movement);
                if (otherTeleporter)
                {
                    m_soundTeleport.play();

                    // Teleport the snake by adding an offset
                    movement += *otherTeleporter - m_snake[0];

                    // Check for collision on other telporter's side
                    if (m_gameGrid.getTileValueAt(m_snake[0] + movement, &tileValue))
                    {
                        switch (tileValue)
                        {
                        case GameGrid::TileValue::Wall:
                        case GameGrid::TileValue::SnakeBody:
                        case GameGrid::TileValue::SnakeHead:
                        case GameGrid::TileValue::NoPlayArea:
                            snakeDied = true;
                            m_soundCollision.play();
                            break;
                        case GameGrid::TileValue::Food:
                            --m_foodInLevel;
                            m_soundFood.play();
                            snakeGrowth = 1;
                            setScore(m_score + 1);
                            break;
                        case GameGrid::TileValue::SuperFood:
                            --m_foodInLevel;
                            m_soundFoodBig.play();
                            setScore(m_score + 5);
                            snakeGrowth = 1;
                            break;
                        }
                    }
                }
            }
            break;
        case GameGrid::TileValue::NoPlayArea:
            snakeDied = true;
            m_soundCollision.play();
            // Note: this shouldn't be possible, trigger death for safety reasons
            break;
        default: ;
        }

        if (!m_snakeIsDead)
        {
            // Clear the tail's previous tile
            m_gameGrid.updateTile(m_snake[m_snake.size() - 1], GameGrid::TileValue::Empty);

            // Move the snake parts from tail to head
            for (std::size_t i = m_snake.size() - 1; i > 0; --i)
            {
                m_snake[i] = m_snake[i - 1];
                m_gameGrid.updateTile(m_snake[i], GameGrid::TileValue::SnakeBody);
            }
            m_snake[0] += movement;
            m_gameGrid.updateTile(m_snake[0], GameGrid::TileValue::SnakeHead);
        }
    }

    if (snakeDied)
        triggerSnakeDeath();

    if (snakeGrowth > 0)
        snakeGrows(snakeGrowth);

    m_lastMovement = m_lastInput;
}

bool GameState::isSnakeValid() const
{
    return m_snake.size() > 1;
}

void GameState::triggerSnakeDeath()
{
    m_snakeIsDead = true;
    if (m_snake.size() > 1)
    {
        for (auto it = m_snake.begin() + 1; it != m_snake.end(); ++it)
            m_gameGrid.updateTile(*it, GameGrid::TileValue::DeadSnakeBody);

		// Update snake head at last element to show head on top when colliding with snake
        m_gameGrid.updateTile(m_snake[0], GameGrid::TileValue::DeadSnakeHead);
    }

	std::shared_ptr<StateManager> tmpStateManager = m_stateManager.lock();
	if (tmpStateManager)
	{
		tmpStateManager->pushState(StateManager::StateID::GameOver, true);
        tmpStateManager->setScoreForGameOverState(m_score);
	}
}

void GameState::snakeGrows(int growth)
{
    // If there is no existing snake it's not possible to grow
    if (m_snake.empty())
        return;

    // As long as all snake parts look the same, just stack
    // the new snake parts at the tail's position
    for (int i = 0; i < growth; ++i)
    {
        m_snake.push_back(m_snake.back());
    }
    m_remainingGrowthTicks += growth;
}

void GameState::setScore(int score)
{
    m_score = score;
    m_scoreText.setString(std::to_string(m_score));
	m_scoreText.setPosition(400.0f - m_scoreText.getGlobalBounds().width / 2.0f, 25.0f);
}

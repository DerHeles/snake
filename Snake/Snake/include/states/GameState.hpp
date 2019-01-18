#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "State.hpp"
#include "GameGrid.hpp"
#include <array>
#include <random>

class GameState : public State
{
public:
    GameState(std::shared_ptr<sf::RenderWindow> window, std::weak_ptr<StateManager> stateManager, std::shared_ptr<ResourceManager> resourceManager);
	~GameState();

	virtual void enter() override;
	virtual void update(sf::Time elapsedTime) override;
	virtual void processEvents() override;
	virtual void processInput() override;
	virtual void render() override;
	virtual void renderAsNonTopState() override;
	virtual void exit() override;
    virtual void setEffectsVolume(float volume) override;

    void setNextLevel(int levelIndex);

private:
    enum class PlayerInput
    {
        Up, Down, Left, Right
    };

    struct LevelData
    {
        std::vector<int> gridData;
        sf::Vector2u tileSize;
        int width;
        int height;
        sf::Vector2i startPosition;
    };

    void buildLevels();
    void loadLevel(int levelIndex);
    void moveSnake();
    bool isSnakeValid() const;
    void triggerSnakeDeath();
    void snakeGrows(int growth);
    void setScore(int score);


    const sf::Time TIME_PER_SNAKE_MOVEMENT = sf::seconds(1.0f / 4.0f);

    std::vector<sf::Vector2i> m_snake;
    std::array<LevelData, 3> m_levels;
    GameGrid m_gameGrid;
    int m_score {0};
    int m_nextLevelIndex {0};
    int m_foodInLevel {0};
    int m_maxFoodInLevel {20};
    int m_foodSpawnCooldown {5};
    int m_remainingGrowthTicks {0};
    int m_remainingFoodSpawnCooldown {0};
    PlayerInput m_lastInput {PlayerInput::Up};
    PlayerInput m_lastMovement {PlayerInput::Up};
    bool m_snakeIsDead {false};

    sf::Clock m_snakeClock;
    sf::Time m_snakeTime;
    std::default_random_engine m_randomGenerator;
    std::uniform_int_distribution<int> m_superFoodDistribution;

    sf::RectangleShape m_gameOverEffect;
	sf::RectangleShape m_scoreUIShape;
    sf::Font m_font;
    sf::Text m_scoreText;

    sf::Sound m_soundFood;
    sf::Sound m_soundFoodBig;
    sf::Sound m_soundTeleport;
    sf::Sound m_soundMovement;
    sf::Sound m_soundMovementGrowing;
    sf::Sound m_soundCollision;

};

#endif
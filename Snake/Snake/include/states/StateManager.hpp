#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include "ResourceManager.hpp"
#include <map>
#include <memory>
#include <SFML/System/Time.hpp>

namespace sf {
    class RenderWindow;
}

class State;

class StateManager : public std::enable_shared_from_this<StateManager>
{
public:
	enum class StateID
	{
		InvalidState, Game, HighscoreMenu, LevelSelection, MainMenu, Options, Pause, GameOver
	};

    struct StackInfo
    {
        StackInfo(StateID id, bool renderStackBelow, bool updateStackBelow)
            :id(id), renderStackBelow(renderStackBelow), updateStackBelow(updateStackBelow)
        {}
        StateID id;
        bool updateStackBelow;
        bool renderStackBelow;
    };

	StateManager();
	~StateManager();

    void init(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<ResourceManager> resourceManager);
	void update(sf::Time elapsedTime);	
	void processEvents(); // Outside the update to force the states to empty the event queue
	void processInput();
	void render();

	// Stack based design
	void pushState(StateID stateID, bool renderStateBelow = false, bool updateStateBelow = false);
	void popState();
    void replaceCurrentState(StateID stateID, bool renderStateBelow = false, bool updateStateBelow = false);
	// Possible additions: processInputOnStateBelow

    void startTheMusic();
    void setNextLevelForGameState(int levelIndex) const;
	void setScoreForGameOverState(int score) const;
    void playSoundUI();
    void playSoundUIConfirm();
    void setEffectsVolume(float volume);
    void setMusicVolume(float volume);

private:
	StateID m_currentState {StateID::InvalidState};
    std::map<StateID, std::unique_ptr<State>> m_states;
    std::vector<StateID> m_stack;
	std::vector<StackInfo> m_infoStack;
    std::shared_ptr<sf::RenderWindow> m_window;

    sf::Sound m_soundUI;
    sf::Sound m_soundUIConfirm;
    sf::Music m_music;
};
#endif
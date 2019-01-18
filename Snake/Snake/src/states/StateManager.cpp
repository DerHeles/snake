#include "states/StateManager.hpp"
#include "states/State.hpp"
#include "states/GameState.hpp"
#include "states/MainMenuState.hpp"
#include "states/LevelSelectionState.hpp"
#include "states/OptionsMenuState.hpp"
#include "states/PauseState.hpp"
#include "states/GameOverState.hpp"

StateManager::StateManager()
{
    // Necessary to compile the unique_ptr<State>
}

StateManager::~StateManager()
{
    // Necessary to compile the unique_ptr<State>
}

void StateManager::init(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<ResourceManager> resourceManager)
{
    m_states.emplace(StateID::Game, std::make_unique<GameState>(window, shared_from_this(),resourceManager));
    m_states.emplace(StateID::MainMenu, std::make_unique<MainMenuState>(window, shared_from_this(), resourceManager));
	m_states.emplace(StateID::Options, std::make_unique<OptionsMenuState>(window, shared_from_this(), resourceManager));
	m_states.emplace(StateID::GameOver, std::make_unique<GameOverState>(window, shared_from_this(), resourceManager));
	m_states.emplace(StateID::Pause, std::make_unique<PauseState>(window, shared_from_this(), resourceManager));
	m_states.emplace(StateID::LevelSelection, std::make_unique<LevelSelectionState>(window, shared_from_this(), resourceManager));

    pushState(StateID::MainMenu);

    m_window = window;

    auto soundBuffer = resourceManager->getSoundBuffer("sound_ui");
    if (soundBuffer)
    {
        m_soundUI.setBuffer(*soundBuffer);
    }

    soundBuffer = resourceManager->getSoundBuffer("sound_ui_confirm");
    if (soundBuffer)
    {
        m_soundUIConfirm.setBuffer(*soundBuffer);
    }

    if (m_music.openFromFile("resources/sounds/soundtrack_about_nothing.wav"))
    {
        m_music.setLoop(true);
    }

    setMusicVolume(50.f);
    setEffectsVolume(50.f);
}

void StateManager::update(sf::Time elapsedTime)
{
	if (m_infoStack.empty())
		return;

	std::vector<StateID> statesToUpdate;

    // Iterate in reverse since the last pushed state is updated first
	for (auto it = m_infoStack.rbegin(); it != m_infoStack.rend(); ++it)
	{
		statesToUpdate.push_back(it->id);

		// Stop queueing states once a state prohibits updating the state below
		if (!it->updateStackBelow)
			break;
	}
	for (auto it = statesToUpdate.rbegin(); it != statesToUpdate.rend(); ++it)
	{
		m_states[*it]->update(elapsedTime);
	}
}

void StateManager::processEvents()
{
	if (m_infoStack.empty())
	{
		sf::Event event;
		while (m_window->pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				m_window->close();
				break;
			}
		}
		return;
	}
	if (m_states.find(m_infoStack.back().id) != m_states.end())
	{
		m_states[m_infoStack.back().id]->processEvents();
	}
}

void StateManager::processInput()
{
	if (m_infoStack.empty())
		return;

	if (m_states.find(m_infoStack.back().id) != m_states.end())
	{
		m_states[m_infoStack.back().id]->processInput();
	}
}

void StateManager::render()
{
    m_window->clear();

    if (m_infoStack.empty())
        return;

    std::vector<StateID> statesToRender;

    // Iterate in reverse since the last pushed state is rendered on top
	for (auto it = m_infoStack.rbegin(); it != m_infoStack.rend(); ++it)
	{
        statesToRender.push_back(it->id);

        // Stop queueing states once a state does not render the state below
        if (!it->renderStackBelow)
            break;
	}
    for (auto it = statesToRender.rbegin(); it != statesToRender.rend(); ++it)
    {
        m_states[*it]->renderAsNonTopState();
    }

    m_window->display();
}

void StateManager::pushState(StateID stateID, bool renderStateBelow, bool updateStateBelow)
{
	m_infoStack.emplace_back(stateID, renderStateBelow, updateStateBelow);
	m_states[stateID]->enter();
}

void StateManager::popState()
{
	if (!m_infoStack.empty())
	{
		m_states[m_infoStack.back().id]->exit();
		m_infoStack.pop_back();
	}
}

void StateManager::replaceCurrentState(StateID stateID, bool renderStateBelow, bool updateStateBelow)
{
    popState();
    pushState(stateID, renderStateBelow, updateStateBelow);
}

void StateManager::startTheMusic()
{
    m_music.play();
}

void StateManager::setNextLevelForGameState(int levelIndex) const
{
    if (m_states.find(StateID::Game) != m_states.end())
    {
        static_cast<GameState*>(m_states.at(StateID::Game).get())->setNextLevel(levelIndex);
    }
}

void StateManager::setScoreForGameOverState(int score) const
{
	if (m_states.find(StateID::GameOver) != m_states.end())
	{
		static_cast<GameOverState*>(m_states.at(StateID::GameOver).get())->setScore(score);
	}
}

void StateManager::playSoundUI()
{
    m_soundUI.play();
}

void StateManager::playSoundUIConfirm()
{
    m_soundUIConfirm.play();
}

void StateManager::setEffectsVolume(float volume)
{
    for (const auto& state : m_states)
        state.second->setEffectsVolume(volume);

    m_soundUI.setVolume(volume);
    m_soundUIConfirm.setVolume(volume);
}

void StateManager::setMusicVolume(float volume)
{
    m_music.setVolume(volume);
}

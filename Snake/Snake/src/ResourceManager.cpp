#include "ResourceManager.hpp"

bool ResourceManager::loadResources()
{
    //////// Fonts //////

    std::shared_ptr<sf::Font> font = std::make_shared<sf::Font>();
    if (!font->loadFromFile("resources/PressStart2P-Regular.ttf"))
    {
        return false;
    }
    m_fonts.emplace("font_menu", font);

    //////// Textures //////

    std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("resources/img/level1.png"))
    {
        return false;
    }
    m_textures.emplace("level1", texture);

	texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("resources/img/level1_selected.png"))
    {
        return false;
    }
    m_textures.emplace("level1_selected", texture);

    texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("resources/img/level2.png"))
    {
        return false;
    }
    m_textures.emplace("level2", texture);

	texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("resources/img/level2_selected.png"))
    {
        return false;
    }
    m_textures.emplace("level2_selected", texture);

    texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("resources/img/level3.png"))
    {
        return false;
    }
    m_textures.emplace("level3", texture);

    texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("resources/img/level3_selected.png"))
    {
        return false;
    }
    m_textures.emplace("level3_selected", texture);

    //////// SoundBuffers //////

    std::shared_ptr<sf::SoundBuffer> soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/food.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_food", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/food_big.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_food_big", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/teleport.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_teleport", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/movement.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_movement", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/movement_growing.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_movement_growing", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/collision.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_collision", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/ui.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_ui", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/ui_confirm.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_ui_confirm", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/volume_up.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_volume_up", soundBuffer);

    soundBuffer = std::make_shared<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile("resources/sounds/volume_down.wav"))
    {
        return false;
    }
    m_soundBuffers.emplace("sound_volume_down", soundBuffer);
}

std::shared_ptr<sf::Font> ResourceManager::getFont(const std::string & identifier) const
{
    if (m_fonts.find(identifier) != m_fonts.end())
    {
        return m_fonts.at(identifier);
    }
    return nullptr;
}

std::shared_ptr<sf::Texture> ResourceManager::getTexture(const std::string & identifier) const
{
    if (m_textures.find(identifier) != m_textures.end())
    {
        return m_textures.at(identifier);
    }
    return nullptr;
}

std::shared_ptr<sf::SoundBuffer> ResourceManager::getSoundBuffer(const std::string & identifier) const
{
    if (m_soundBuffers.find(identifier) != m_soundBuffers.end())
    {
        return m_soundBuffers.at(identifier);
    }
    return nullptr;
}

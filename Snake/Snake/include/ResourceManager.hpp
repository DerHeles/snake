#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio.hpp>
#include <optional>
#include <unordered_map>

class ResourceManager
{
public:
    bool loadResources();
    std::shared_ptr<sf::Font> getFont(const std::string& identifier) const;
    std::shared_ptr<sf::Texture> getTexture(const std::string& identifier) const;
    std::shared_ptr<sf::SoundBuffer> getSoundBuffer(const std::string& identifier) const;

private:
    std::unordered_map<std::string, std::shared_ptr<sf::Font>> m_fonts;
    std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> m_soundBuffers;
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> m_textures;
};

#endif
#ifndef GAME_GRID_HPP
#define GAME_GRID_HPP

#include <SFML/Graphics.hpp>
#include <optional>

class GameGrid : public sf::Drawable, public sf::Transformable
{
public:
	enum class TileValue
	{
		Empty, Wall, SnakeBody, SnakeHead, Food, SuperFood,
		Poison, Teleport1, Teleport2, NoPlayArea, DeadSnakeBody, DeadSnakeHead
	};

	bool loadFromArray(std::vector<int> level, int width, int height, sf::Vector2u tileSize);
	void updateTile(int x, int y, TileValue value);
	void updateTile(sf::Vector2i position, TileValue value);
    bool isValidPosition(int x, int y) const;
    bool isValidPosition(sf::Vector2i position) const;
    bool getTileValueAt(int x, int y, TileValue* const tileValueToSet) const;
    bool getTileValueAt(sf::Vector2i position, TileValue* const tileValueToSet) const;
    void getListOfEmptyTiles(std::vector<sf::Vector2i>* tileList) const;
    std::optional<sf::Vector2i> getPositionOfOtherTeleporter1(sf::Vector2i position) const;
    std::optional<sf::Vector2i> getPositionOfOtherTeleporter2(sf::Vector2i position) const;

protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    struct TeleporterPair
    {
        TeleporterPair(sf::Vector2i pos1, sf::Vector2i pos2)
            :pos1(pos1), pos2(pos2){}
        sf::Vector2i pos1;
        sf::Vector2i pos2;
    };

	static sf::Color getColorFromTileValue(TileValue value);
    
	int m_width {0};
	int m_height {0};
	std::vector<int> m_grid;
	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
    sf::Vector2u m_tileSize;
    std::optional<TeleporterPair> m_teleporterPair1;
    std::optional<TeleporterPair> m_teleporterPair2;
};

#endif

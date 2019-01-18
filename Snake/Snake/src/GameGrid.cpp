#include "GameGrid.hpp"
#include <cassert>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

bool GameGrid::loadFromArray(std::vector<int> level, int width, int height, sf::Vector2u tileSize)
{
	if(level.size() != width * height)
	{
		cerr << "Size of level data is not equal (width * height)!" << endl;
		return false;
	}

	m_width = width;
	m_height = height;
	m_grid.resize(width * height * 4);

	// Resize the vertex array to fit the level size
	m_vertices.setPrimitiveType(sf::Quads);
	m_vertices.resize(width * height * 4);

	int teleportOneCount = 0;
	int teleportTwoCount = 0;

    sf::Vector2i teleportOnePos1;
    sf::Vector2i teleportOnePos2;
    sf::Vector2i teleportTwoPos1;
    sf::Vector2i teleportTwoPos2;

	// Populate the vertex array, with one quad per tile
	for (unsigned int i = 0; i < width; ++i)
		for (unsigned int j = 0; j < height; ++j)
		{
			// Get the current tile number
			int intValue = level[i + j * width];
			TileValue tileValue = static_cast<TileValue>(intValue);

            // Check for teleporter tiles
			if (tileValue == TileValue::Teleport1)
			{
                if (teleportOneCount == 0)
                    teleportOnePos1 = sf::Vector2i(i, j);
                else if (teleportOneCount == 1)
                    teleportOnePos2 = sf::Vector2i(i, j);
			    ++teleportOneCount;
			}
			else if (tileValue == TileValue::Teleport2)
			{
                if (teleportTwoCount == 0)
                    teleportTwoPos1 = sf::Vector2i(i, j);
                else if (teleportTwoCount == 1)
                    teleportTwoPos2 = sf::Vector2i(i, j);
				++teleportTwoCount;
			}

			// Get a pointer to the current tile's quad
			sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

			// Define its 4 corners
			quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
			quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
			quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
			quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

			// Set color of quad
			sf::Color tileColor = getColorFromTileValue(tileValue);
			for (std::size_t k = 0; k < 4; ++k)
				quad[k].color = tileColor;

			// set logical tile value
			m_grid[i + j * width] = intValue;
		}

    // Only none or exactly one pair of each teleporter type is allowed in a level
	if(!(teleportOneCount == 0 || teleportOneCount == 2) || !(teleportTwoCount == 0 || teleportTwoCount == 2))
	{
		cerr << "count of teleporter tiles not valid!" << endl;
		return false;
	}

    // Save the valid teleporter positions in a pair
    if(teleportOneCount == 2)
        m_teleporterPair1 = std::make_optional<TeleporterPair>(teleportOnePos1, teleportOnePos2);
    if(teleportTwoCount == 2)
        m_teleporterPair2 = std::make_optional<TeleporterPair>(teleportTwoPos1, teleportTwoPos2);

	return true;
}

void GameGrid::updateTile(int x, int y, TileValue value)
{
    if(!isValidPosition(x, y))
    {
        cerr << "updateTile(): invalid x or y coordinate!" << endl;
        return;
    }

    int intValue = static_cast<int>(value);

    // Get a pointer to the current tile's quad
    sf::Vertex* quad = &m_vertices[(x + y * m_width) * 4];

    // Set color of quad
    sf::Color tileColor = getColorFromTileValue(value);
    for (std::size_t k = 0; k < 4; ++k)
        quad[k].color = tileColor;

    // Set logical tile value
    m_grid[x + y * m_width] = intValue;
}

void GameGrid::updateTile(sf::Vector2i position, TileValue value)
{
	updateTile(position.x, position.y, value);
}

bool GameGrid::isValidPosition(int x, int y) const
{
    return x >= 0 && y >= 0 && x < m_width && y < m_height;
}

bool GameGrid::isValidPosition(sf::Vector2i position) const
{
    return isValidPosition(position.x, position.y);
}

bool GameGrid::getTileValueAt(int x, int y, TileValue* const tileValueToSet) const
{
    if (!isValidPosition(x, y))
        return false;

    *tileValueToSet = static_cast<TileValue>(m_grid[x + y * m_width]);
    return true;
}

bool GameGrid::getTileValueAt(sf::Vector2i position, TileValue* const tileValueToSet) const
{
    return getTileValueAt(position.x, position.y, tileValueToSet);
}

void GameGrid::getListOfEmptyTiles(std::vector<sf::Vector2i>* tileList) const
{
    // Could be optimized with a cached list
    for (unsigned int i = 0; i < m_width; ++i)
        for (unsigned int j = 0; j < m_height; ++j)
            if(TileValue::Empty == static_cast<TileValue>(m_grid[i + j * m_width]))
                tileList->emplace_back(i, j);   
}

std::optional<sf::Vector2i> GameGrid::getPositionOfOtherTeleporter1(sf::Vector2i position) const
{
    if(m_teleporterPair1)
    {
        if(position == m_teleporterPair1->pos1)
            return m_teleporterPair1->pos2;
        if (position == m_teleporterPair1->pos2)
            return m_teleporterPair1->pos1;
    }
	return std::nullopt;
}

std::optional<sf::Vector2i> GameGrid::getPositionOfOtherTeleporter2(sf::Vector2i position) const
{
    if (m_teleporterPair2)
    {
        if (position == m_teleporterPair2->pos1)
            return m_teleporterPair2->pos2;
        if (position == m_teleporterPair2->pos2)
            return m_teleporterPair2->pos1;
    }
    return std::nullopt;
}

void GameGrid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(m_vertices, states);
}

sf::Color GameGrid::getColorFromTileValue(GameGrid::TileValue value)
{
    switch (value)
    {
    case TileValue::Empty:
        return sf::Color(235, 235, 235);
    case TileValue::Wall:
        return sf::Color(140, 140, 140);
    case TileValue::SnakeBody:
        return sf::Color::Green;
    case TileValue::SnakeHead:
        return sf::Color(0, 200, 0);
    case TileValue::Food:
        return sf::Color::Red;
    case TileValue::SuperFood:
        return sf::Color::Yellow;
    case TileValue::Poison:
        // Currently not used
        return sf::Color::Magenta;
    case TileValue::Teleport1:
        return sf::Color::Cyan;
    case TileValue::Teleport2:
        return sf::Color(255, 140, 0);
    case TileValue::NoPlayArea:
        return sf::Color::Black;
    case TileValue::DeadSnakeBody:
        return sf::Color(181, 164, 144);
    case TileValue::DeadSnakeHead:
        return sf::Color(127, 91, 51);
    default:
        return sf::Color::White;
    }
}

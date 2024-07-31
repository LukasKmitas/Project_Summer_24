#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Global.h"

struct Edge
{
    sf::Vector2f m_start;
    sf::Vector2f m_end;
};

std::vector<Edge> calculateEdges(const std::vector<sf::RectangleShape>& m_shapes);
sf::Vector2f normalize(const sf::Vector2f& m_vec);
std::vector<sf::Vertex> calculateLightPolygon(const sf::Vector2f& m_lightPos, const std::vector<Edge>& m_edges, float m_lightRadius);

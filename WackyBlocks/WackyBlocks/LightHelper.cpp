#include "LightHelper.h"

std::vector<Edge> calculateEdges(const std::vector<sf::RectangleShape>& m_shapes)
{
    std::vector<Edge> edges;
    for (const auto& shape : m_shapes)
    {
        sf::FloatRect bounds = shape.getGlobalBounds();
        edges.push_back({ sf::Vector2f(bounds.left, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top) });
        edges.push_back({ sf::Vector2f(bounds.left + bounds.width, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height) });
        edges.push_back({ sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), sf::Vector2f(bounds.left, bounds.top + bounds.height) });
        edges.push_back({ sf::Vector2f(bounds.left, bounds.top + bounds.height), sf::Vector2f(bounds.left, bounds.top) });
    }
    return edges;
}

std::vector<sf::Vertex> calculateLightPolygon(const sf::Vector2f& m_lightPos, float m_lightRadius, float m_intensity, const std::vector<Edge>& m_edges)
{
    std::vector<sf::Vertex> vertices;
    std::vector<sf::Vector2f> points;

    for (const auto& edge : m_edges)
    {
        points.push_back(edge.m_start);
        points.push_back(edge.m_end);
    }

    int numRays = 360;
    float increment = 360.0f / numRays;

    for (int i = 0; i < numRays; ++i)
    {
        float angle = increment * i;
        sf::Vector2f direction(std::cos(angle * 3.14159f / 180.0f), std::sin(angle * 3.14159f / 180.0f));
        points.push_back(m_lightPos + direction * m_lightRadius);
    }

    // Add the four corners of the screen
    points.push_back(sf::Vector2f(0, 0));
    points.push_back(sf::Vector2f(SCREEN_WIDTH, 0));
    points.push_back(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    points.push_back(sf::Vector2f(0, SCREEN_HEIGHT));

    std::vector<std::pair<sf::Vector2f, float>> directions;
    for (const auto& point : points)
    {
        sf::Vector2f direction = normalize(point - m_lightPos);
        directions.push_back({ direction, std::atan2(direction.y, direction.x) });
    }
    
    // Sort directions based on angle
    std::sort(directions.begin(), directions.end(),
        [](const std::pair<sf::Vector2f, float>& a, const std::pair<sf::Vector2f, float>& b)
        {
            return a.second < b.second;
        });

    sf::Color lightColor(255, 255, 255, static_cast<sf::Uint8>(255 * m_intensity));
    vertices.push_back(sf::Vertex(m_lightPos, lightColor));

    for (const auto& dir : directions)
    {
        sf::Vector2f farPoint = m_lightPos + dir.first * m_lightRadius;
        vertices.push_back(sf::Vertex(farPoint, sf::Color(255, 255, 255, 0)));
    }

    // Add the first point again to close the fan
    if (!vertices.empty())
    {
        vertices.push_back(vertices[1]);
    }
    return vertices;
}

std::vector<sf::Vertex> calculateShadowPolygon(const sf::Vector2f& m_lightPos, const sf::RectangleShape& m_block, float m_shadowDistance, float m_lightRadius, const sf::Vector2f& playerLightPos, float playerRadius)
{
    std::vector<sf::Vertex> shadowVertices;

    // Check if the block is within the light radius
    sf::Vector2f blockCenter = m_block.getPosition();
    float distanceToLight = std::sqrt(std::pow(blockCenter.x - m_lightPos.x, 2) + std::pow(blockCenter.y - m_lightPos.y, 2));
    if (distanceToLight > m_lightRadius)
    {
        return shadowVertices;
    }

    // Get block corners
    const sf::Vector2f blockCorners[4] = 
    {
        m_block.getTransform().transformPoint(0.0f, 0.0f),
        m_block.getTransform().transformPoint(m_block.getSize().x, 0.0f),
        m_block.getTransform().transformPoint(m_block.getSize().x, m_block.getSize().y),
        m_block.getTransform().transformPoint(0.0f, m_block.getSize().y)
    };

    // Find which corners are in shadow
    for (int i = 0; i < 4; ++i)
    {
        sf::Vector2f corner1 = blockCorners[i];
        sf::Vector2f corner2 = blockCorners[(i + 1) % 4];

        sf::Vector2f dir1 = normalize(corner1 - m_lightPos);
        sf::Vector2f dir2 = normalize(corner2 - m_lightPos);

        // Calculate gradient shadow effect
        float shadowStrength = std::max(0.0f, 1.0f - (distanceToLight / m_lightRadius));
        sf::Color shadowColor = sf::Color(0, 0, 0, static_cast<sf::Uint8>(shadowStrength * 255));

        sf::Vector2f shadowEnd1 = corner1 + dir1 * m_shadowDistance;
        sf::Vector2f shadowEnd2 = corner2 + dir2 * m_shadowDistance;

        shadowVertices.push_back(sf::Vertex(corner1, shadowColor));
        shadowVertices.push_back(sf::Vertex(shadowEnd1, sf::Color(0, 0, 0, 0)));
        shadowVertices.push_back(sf::Vertex(shadowEnd2, sf::Color(0, 0, 0, 0)));
        shadowVertices.push_back(sf::Vertex(corner2, shadowColor));
    }

    return shadowVertices;
}

sf::Vector2f normalize(const sf::Vector2f& m_vec)
{
    float length = std::sqrt(m_vec.x * m_vec.x + m_vec.y * m_vec.y);
    if (length != 0)
    {
        return sf::Vector2f(m_vec.x / length, m_vec.y / length);
    }
    else
    {
        return sf::Vector2f(0, 0);
    }
}
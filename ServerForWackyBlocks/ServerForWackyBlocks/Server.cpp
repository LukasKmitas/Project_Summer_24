#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

void Server::run()
{
    if (m_listener.listen(PORT) != sf::Socket::Done)
    {
        std::cerr << "Failed to bind listener socket to port " << PORT << std::endl;
        return;
    }
    std::cout << "Server is starting..." << std::endl;
    std::cout << "Server is listening on port " << PORT << std::endl;

    std::thread connectionThread(&Server::handleConnections, this);
    connectionThread.detach();

    std::string command;
    while (m_running)
    {
        std::cin >> command;
        if (command == "exit")
        {
            m_running = false;
            m_listener.close();
            break;
        }
    }
}

void Server::handleConnections()
{
    while (m_running)
    {
        std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();
        if (m_listener.accept(*client) == sf::Socket::Done)
        {
            std::cout << "New client connected: " << client->getRemoteAddress() << std::endl;
            std::thread clientThread(&Server::handleClient, this, client.get());
            clientThread.detach();

            std::lock_guard<std::mutex> lock(m_mutex);
            m_clients.push_back(std::move(client));
        }
    }
}

void Server::handleClient(sf::TcpSocket* m_client)
{
    char buffer[128];
    std::size_t received;

    while (m_client->receive(buffer, sizeof(buffer), received) == sf::Socket::Done) 
    {
        std::string request(buffer, received);

        if (request == "init")
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::string randomID = generateRandomID();
            Session session = { m_client->getRemoteAddress(), randomID };
            m_sessions.push_back(session);
            m_client->send(randomID.c_str(), randomID.size());
        }
        else if (request == "host") 
        {

        }
        else if (request == "searchForHost") 
        {
            sendAvailableSessions(m_client);
        }
        else if (request.substr(0, 5) == "join:")
        {
            std::string sessionID = request.substr(6);
            notifyHost(sessionID);
        }
        else if (request.substr(0, 14) == "levelSelected:")
        {

        }
    }
}

void Server::sendAvailableSessions(sf::TcpSocket* m_client)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string sessions;
    for (const auto& session : m_sessions) 
    {
        sessions += session.m_name + " (" + session.m_address.toString() + ")\n";
    }
    if (sessions.empty())
    {
        sessions = "No available sessions found.";
    }
    std::cout << "Sending sessions to client(s):\n" << sessions << std::endl;
    m_client->send(sessions.c_str(), sessions.size());
}

void Server::notifyHost(const std::string& m_sessionID)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& session : m_sessions)
    {
        std::cout << m_sessionID << std::endl;
        std::cout << session.m_name << std::endl;

        if (session.m_name == m_sessionID)
        {
            for (auto& client : m_clients)
            {
                if (client->getRemoteAddress() == session.m_address)
                {
                    std::string notification = "playerJoined";
                    client->send(notification.c_str(), notification.size());
                    break;
                }
            }
            break;
        }
    }
}

std::string Server::generateRandomID()
{
    const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int len = 6;
    std::string id;
    id.reserve(len);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distrib(0, sizeof(alphanum) - 2);

    for (int i = 0; i < len; ++i)
    {
        id += alphanum[distrib(generator)];
    }

    return id;
}

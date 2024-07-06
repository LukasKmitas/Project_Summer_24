#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>

const unsigned short PORT = 53000;

struct Session
{
    sf::IpAddress m_address;
    std::string m_name;
};

class Server
{
public:

    Server();
    ~Server();

    void run();

private:

    void handleConnections();
    void handleClient(sf::TcpSocket* m_client);
    void sendAvailableSessions(sf::TcpSocket* m_client);
    void notifyHost(const std::string& m_sessionID);

    std::string generateRandomID();

    sf::TcpListener m_listener;
    std::vector<std::unique_ptr<sf::TcpSocket>> m_clients;
    std::vector<Session> m_sessions;
    std::mutex m_mutex;

    bool m_running = true;

};

#endif
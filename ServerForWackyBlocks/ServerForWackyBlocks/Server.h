#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <unordered_map>
#include <sstream>

const unsigned short PORT = 53000;

struct Session
{
    sf::IpAddress m_address;
    std::string m_name;
    sf::TcpSocket* m_playerOneSocket;
    sf::TcpSocket* m_playerTwoSocket;
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

    void notifyServerOfNewHost(sf::TcpSocket* m_client);
    void sendAvailableSessions(sf::TcpSocket* m_client);
    void notifyHostPlayerHasJoined(const std::string& m_hostID, const std::string& clientID);
    void notifyClients(const std::string& m_sessionID, const std::string& m_levelName);
    
    void displayInfo();
    void displayAllClients();
    void displayClientSessions();

    std::string generateRandomID();

    std::vector<std::string> split(const std::string& m_string, char m_delimiter);

    sf::TcpListener m_listener;
    std::vector<std::unique_ptr<sf::TcpSocket>> m_clients;
    std::unordered_map<std::string, Session> m_sessions;
    std::mutex m_mutex;

    bool m_running = true;

};

#endif
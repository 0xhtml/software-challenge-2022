#pragma once

#include <boost/asio.hpp>
#include <string>
#include <pugixml.hpp>

#include "gamestate.hpp"

class Network {
private:
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::socket socket{ioService};
    boost::asio::streambuf receiveBuffer;
    std::string roomId;
    GameState gameState{""};

    void send(const std::string &data);

    std::string receive(const std::string &delim);

    void connect(const std::string &host, const int port);

    void receiveRoomId();

    bool receiveRoomMessage();

    void receiveProtocolEnd();

    void close();

public:
    Network(const std::string &host, const int port, const std::string &reservation);
};

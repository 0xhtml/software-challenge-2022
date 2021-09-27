#pragma once

#include <boost/asio.hpp>
#include <pugixml.hpp>
#include <string>

#include "types.hpp"

struct Packet {
    pugi::xml_node data;
    std::string dataClass;
    Time time;
};

class Network {
private:
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::socket socket{ioService};
    boost::asio::streambuf receiveBuffer;
    std::string roomId;

    void send(const std::string &data);

    std::string receive(const std::string &delim);

public:
    Network(const std::string &host, const int port, const std::string &reservation);

    void sendRoomPacket(const std::string &data);

    Packet receiveRoomPacket();

    void close();
};

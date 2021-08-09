#include "network.hpp"

#include <assert.h>
#include <boost/asio.hpp>
#include <chrono>
#include <pugixml.hpp>
#include <string>

#include "types.hpp"

Network::Network(const std::string &host, const int port, const std::string &reservation) {
    connect(host, port);

    if (reservation.empty()) {
        send("<join gameType=\"swc_2022_blokus\"/>");
    } else {
        send("<joinPrepared reservationCode=\"" + reservation + "\" />");
    }

    receiveRoomId();

    while (receiveRoomMessage());

    receiveProtocolEnd();

    close();
}

void Network::send(const std::string &data) {
    boost::system::error_code error;
    write(socket, boost::asio::buffer(data), error);
}

std::string Network::receive(const std::string &delim) {
    boost::system::error_code error;

    std::size_t bytes = read_until(socket, receiveBuffer, delim, error);

    if (!error) {
        std::string data{
                boost::asio::buffers_begin(receiveBuffer.data()),
                boost::asio::buffers_begin(receiveBuffer.data()) + bytes
        };
        receiveBuffer.consume(bytes);
        return data;
    }

    return "";
}

void Network::connect(const std::string &host, const int port) {
    boost::asio::ip::tcp::endpoint endpoint;

    if (host == "localhost") {
        endpoint.address(boost::asio::ip::make_address("127.0.0.1"));
    } else {
        boost::asio::ip::tcp::resolver::query query{host, ""};
        endpoint = *boost::asio::ip::tcp::resolver(ioService).resolve(query);
    }

    endpoint.port(port);

    socket.connect(endpoint);
    send("<protocol>");

    assert(receive("<protocol>"));
}

void Network::receiveRoomId() {
    std::string data = receive("/>");

    pugi::xml_document xmlDocument;
    xmlDocument.load_string(data.data());

    pugi::xml_node joined = xmlDocument.child("joined");
    assert(joined);

    roomId = joined.attribute("roomId").value();
}

bool Network::receiveRoomMessage() {
    std::string data = receive("</room>");

    if (data.empty()) {
        printf("WARN: data is empty, expected room packet\n");
        return false;
    }

    pugi::xml_document xmlDocument;
    xmlDocument.load_string(data.data());

    pugi::xml_node xml = xmlDocument.child("room");

    pugi::xml_node roomMessageData = xml.child("data");
    std::string roomMessageDataClass = roomMessageData.attribute("class").value();

    if (roomMessageDataClass == "sc.framework.plugins.protocol.MoveRequest") {
        // TODO: Call aplhabeta
    } else if (roomMessageDataClass == "memento") {
        // TODO: Parse gamestate
    } else if (roomMessageDataClass == "result") {
        pugi::xml_node winner = roomMessageData.child("winner");

        printf("INFO: WINNER %s", winner.attribute("displayName").value());
        printf("%s\n", winner.child("color").text().get());

        return false;
    } else if (roomMessageDataClass == "error") {
        printf("ERROR: %s\n", roomMessageData.attribute("message").value());
        return false;
    } else {
        printf("INFO: unknown room message '%s'\n", roomMessageDataClass.c_str());
    }

    return true;
}

void Network::receiveProtocolEnd() {
    std::string data = receive("</protocol>");

    if (data.empty()) {
        printf("WARN: protocol end is empty\n");
        return;
    }

    data.insert(0, "<protocol>");

    pugi::xml_document xmlDocument;
    xmlDocument.load_string(data.data());

    pugi::xml_node xml = xmlDocument.child("protocol");

    for (pugi::xml_node roomMessage : xml.children("room")) {
        pugi::xml_node roomMessageData = roomMessage.child("data");
        std::string roomMessageDataClass = roomMessageData.attribute("class").value();
        printf("INFO: protocol end includes '%s'\n", roomMessageDataClass.c_str());
    }

    send("</protocol>");
}

void Network::close() {
    boost::system::error_code error;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    socket.close(error);
}

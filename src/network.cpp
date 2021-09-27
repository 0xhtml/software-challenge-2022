#include "network.hpp"

#include <assert.h>
#include <boost/asio.hpp>
#include <chrono>
#include <pugixml.hpp>
#include <string>

#include "types.hpp"
#include "parser.hpp"

Network::Network(const std::string &host, const int port, const std::string &reservation) {
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

    if (reservation.empty()) {
        send("<join />");
    } else {
        send("<joinPrepared reservationCode=\"" + reservation + "\" />");
    }

    std::string data = receive("<protocol>");
    if (data.empty()) throw std::runtime_error("Didn't receive <protocol>");

    data = receive("/>");

    pugi::xml_document xmlDocument;
    xmlDocument.load_string(data.data());

    pugi::xml_node joined = xmlDocument.child("joined");
    if (!joined) throw std::runtime_error("Didn't join game");

    roomId = joined.attribute("roomId").value();
}

void Network::send(const std::string &data) {
    boost::system::error_code error;
    write(socket, boost::asio::buffer(data), error);
}

void Network::sendRoomPacket(const std::string &data) {
    send("<room roomId=\"" + roomId +  "\">" + data + "</room>");
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

Packet Network::receiveRoomPacket() {
    std::string data = receive("</room>");

    Time time = std::chrono::system_clock::now();

    if (data.empty()) throw std::runtime_error("Didn't receive room message");

    pugi::xml_document xmlDocument;
    xmlDocument.load_string(data.data());

    pugi::xml_node xml = xmlDocument.child("room").child("data");

    return {xml, xml.attribute("class").value(), time};
}

void Network::close() {
    std::string data = receive("</protocol>");

    if (!data.empty()) {
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
    } else {
        printf("INFO: protocol end empty\n");
    }

    boost::system::error_code error;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

    socket.close(error);
}

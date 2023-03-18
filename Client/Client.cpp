// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include "../flatbuffers/PropertyTree_generated.h"
#include "PropertyData.h"

using namespace boost::asio;
using ip::tcp;
using namespace std;
using namespace MyPropertyTree;

std::atomic<int> countAtomic = 0;

class Sender
{

    std::vector<std::jthread> m_senderThreadPool;
    int numberOfThread{ 1 };

public:

    Sender(const int iNoOfThreads) :numberOfThread(iNoOfThreads)
    {}
    ///////////////////////////////////////////////////////////
    void start()
    {
        for (int i = 0; i < numberOfThread; i++)
        {
            m_senderThreadPool.push_back(std::jthread(&Sender::sender, this));
            std::this_thread::sleep_for(5s);
        }
    }
    ///////////////////////////////////////////////////////////
    void send(vector<uint8_t>& iBuffer)
    {
        boost::asio::io_service io_service;

        //socket creation
        tcp::socket socket(io_service);

        //connection
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));

        // request/message from client
        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(iBuffer), error);

        if (error)
        {
            cout << "Sending of buffer failed: " << error.message() << endl;
        }
    }
    ///////////////////////////////////////////////////////////
    void readBuffer(vector<uint8_t>& buffer)
    {
        uint8_t* buffer_ptr = &buffer[0];

        auto property = GetProperty(buffer_ptr);

        stringstream ss;

        ss << "-----Start----" << endl;

        ss << property->name()->str() << endl;
        ss << property->value()->str() << endl;
        ss << property->type() << endl;

        for (int i = 0; i < property->subprop()->size(); i++)
        {
            ss << property->subprop()->Get(i)->data1()->str() << endl;
            ss << property->subprop()->Get(i)->data2() << endl;
        }

        ss << "-----End----" << endl;

        cout << ss.str() << endl;
    }
    ///////////////////////////////////////////////////////////
    void updateBuffer(vector<uint8_t>& buffer)
    {
        auto property = GetMutableProperty(&buffer[0]);
        property->mutate_type(MyPropertyTree::Type_V);
    }
    ///////////////////////////////////////////////////////////
    void sender()
    {
        while (true)
        {
            vector<uint8_t> buffer;

            const int var = countAtomic.fetch_add(1);

            stringstream ss;

            ss << this_thread::get_id() << " :: Message Count :" << var;

            common::PropertyData propertyData;

            propertyData.createBuffer(buffer, ss.str());

            updateBuffer(buffer);

            readBuffer(buffer);

            send(buffer);

            std::this_thread::sleep_for(10s);
        }
    }
};

int main()
{
    Sender senderObj(3/*No of threads*/);

    senderObj.start();
    
    return 0;
}

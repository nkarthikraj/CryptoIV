// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/asio.hpp>
#include "../flatbuffers/PropertyTree_generated.h"
#include "PropertyData.h"
#include <mutex>
#include "flatbuffers/reflection.h"
//#include "flatbuffers/reflection_generated.h"
//#include "flatbuffers/minireflect.h"

using namespace MyPropertyTree;

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
using namespace std;

class Receiver
{
    common::PropertyData m_propertyData;
    std::mutex syncLock;

public:

    void readBuffer(tcp::socket& iSocket)
    {
        boost::asio::streambuf buf;

        try
        {
            boost::system::error_code error;
           
            boost::asio::read(iSocket, buf, error);
        }
        catch (std::exception& ex)
        {
            cout << ex.what() << endl;
        }
       
       
        if (false) /* reflection */
        {
            std::string bfbsfile("PropertyTree.bfbs");

            auto& schema = *reflection::GetSchema(bfbsfile.c_str());

            auto root_table = schema.root_table();

            auto fields = root_table->fields();

            auto name_field_ptr = fields->LookupByKey("name");

            auto& name_field = *name_field_ptr;

            auto value_field_ptr = fields->LookupByKey("value");

            auto& value_field = *value_field_ptr;

            auto& root = *flatbuffers::GetAnyRoot(boost::asio::buffer_cast<const uint8_t*>(buf.data()));

           // auto name = flatbuffers::GetAnyFieldS(root, name_field, & schema);

            //auto value = flatbuffers::GetAnyFieldS(root, name_field, &schema);

           // cout << "name:" << name << endl;
           // cout << "value:" << value << endl;
        }
        else
        {
            auto propertyPtr = GetProperty(boost::asio::buffer_cast<const uint8_t*>(buf.data()));
            //Synchronize data
            {
                lock_guard<mutex> lock(syncLock);
                m_propertyData.convertBufferToCppObject(propertyPtr);
                m_propertyData.print();
            }
        }
    }

    void receiverThread()
    {
        while (true)
        {
            boost::asio::io_service io_service;

            //listen for new connection
            tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

            tcp::socket socket(io_service);

            //waiting for connection
            acceptor.accept(socket);

            //read operation
            readBuffer(socket);

        }
    }
};

int main()
{
    Receiver receiverObj;

    std::vector<std::jthread> threadGrp;

    for (int i = 0; i < 1; i++)
    {
        //threadGrp.push_back(std::jthread(&Receiver::receiverThread, ref(receiverObj)));
    }

    receiverObj.receiverThread();

    return 0;
}

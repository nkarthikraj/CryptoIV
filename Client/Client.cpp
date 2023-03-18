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

class Sender
{
  private:

      std::atomic<int> count = 0;
      common::PropertyData m_propertyData;

  public:

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

            if (!error)
            {
                cout << "Client sent buffer successfully" << endl;
            }
            else
            {
                cout << "Sending of buffer failed: " << error.message() << endl;
            }
     }

    void readBuffer(vector<uint8_t>& buffer)
    {
         uint8_t* buffer_ptr = &buffer[0];

         auto property = GetProperty(buffer_ptr);

         cout << property->name()->str() << endl;
         cout << property->value()->str() << endl;
         cout << property->type() << endl;

         for (int i = 0; i < property->subprop()->size(); i++)
         {
              cout << property->subprop()->Get(i)->data1()->str() << endl;
              cout << property->subprop()->Get(i)->data2() << endl;
         }

         cout << "-----XXXX----" << endl;
    }

    void updateBuffer(vector<uint8_t>& buffer)
    {
         auto property = GetMutableProperty(&buffer[0]);
         property->mutate_type(MyPropertyTree::Type_V);
    }

    void senderThread()
    {
        while (true)
        {
            vector<uint8_t> buffer;

            const int var = count.fetch_add(1) ;

            stringstream ss; 
            
            ss << this_thread::get_id() << " :: Message Count :" << var;

            m_propertyData.createBuffer(buffer, ss.str());

            readBuffer(buffer);

            updateBuffer(buffer);

            send(buffer);

            std::this_thread::sleep_for(10s);
        }
    }
};

int main()
{
    std::vector<std::jthread> threadGrp;

    Sender senderObj;

    for (int i = 0; i < 1; i++)
    {
        //threadGrp.push_back(std::jthread(&Sender::senderThread, ref(senderObj)));
    }

    senderObj.senderThread();

    
    return 0;
}

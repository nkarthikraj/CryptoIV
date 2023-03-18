// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/asio.hpp>
#include "../flatbuffers/PropertyTree_generated.h"
#include "PropertyData.h"
#include <mutex>
#include "flatbuffers/reflection.h"
#include <queue>
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
private:
   
    std::queue<std::vector<char>> m_consumer;
    std::mutex syncLock;
    std::condition_variable cv;
    std::vector<std::jthread> m_consumerThreadPool;
    std::vector<std::jthread> m_producerThreadPool;
    int numberOfThread{ 1 };

public:

    Receiver(const int iNoOfThreads):numberOfThread(iNoOfThreads)
    {
        
    }
    ///////////////////////////////////////////////////////////
    void start()
    {
        for (int i = 0; i < numberOfThread; i++)
        {
            m_consumerThreadPool.push_back(std::jthread(&Receiver::consumerThread, this));
            m_producerThreadPool.push_back(std::jthread(&Receiver::producerThread, this));
        }
    }
    ///////////////////////////////////////////////////////////
    void producerThread()
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
    ///////////////////////////////////////////////////////////
    void readBuffer(tcp::socket& iSocket)
    {
        try
        {
            boost::system::error_code error;
            boost::asio::streambuf buf;

            boost::asio::read(iSocket, buf, error);

            std::vector<char> target(buf.size());
            buffer_copy(boost::asio::buffer(target), buf.data());

            {  /*Critical section*/
                unique_lock<mutex> lock(syncLock);
                m_consumer.push(target);
                cv.notify_all();
            }
        }
        catch (std::exception& ex)
        {
            cout << ex.what() << endl;
        }
    }
    ///////////////////////////////////////////////////////////
    void consumerThread()
    {
        while (true)
        {
             std::vector<char> buf;
             {
                  std::lock_guard<mutex> lock(syncLock);
                  if (!m_consumer.empty())
                  {
                      buf = m_consumer.front();

                      m_consumer.pop();
                  }
             }

             if (!buf.empty())
             {
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

                        //  auto& root = *flatbuffers::GetAnyRoot(boost::asio::buffer_cast<const uint8_t*>(buf.data()));

                          // auto name = flatbuffers::GetAnyFieldS(root, name_field, & schema);

                           //auto value = flatbuffers::GetAnyFieldS(root, name_field, &schema);

                          // cout << "name:" << name << endl;
                          // cout << "value:" << value << endl;
                    }
                  else
                  {
                       auto propertyPtr = GetProperty(&(buf[0]));

                       common::PropertyData propertyData;

                       propertyData.convertBufferToCppObject(propertyPtr);

                       propertyData.print();
                  }
             }

             unique_lock<mutex> lock(syncLock);
             cv.wait(lock, [&]() { return !m_consumer.empty(); });
        }
    }
};

int main()
{
    Receiver receiverObj(6/*No of threads*/);

    receiverObj.start();

    return 0;
}

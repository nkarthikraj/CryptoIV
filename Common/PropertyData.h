#include <iostream>
#include <vector>
#include "../flatbuffers/PropertyTree_generated.h"

using namespace std;
using namespace MyPropertyTree;

namespace common
{
	enum class Type :int
	{
		X = 1,
		Y = 2,
	    V = 3
	};

	struct SubPropertyData
	{
		SubPropertyData(const std::string& iData1, const short iData2):m_data1(iData1),m_data2(iData2) {}
		std::string m_data1;
		short m_data2;
	};

	class PropertyData
	{
		std::string m_name;
		std::string m_value;
		Type m_type;
		std::vector<SubPropertyData> m_subproperty;

	public:
		PropertyData():m_name("PropertyName"), m_value("PropertyValue"), m_type(Type::X)
		{
			for (int i = 0; i < 1; i++)
			{
				m_subproperty.emplace_back("Data1", i);
			}
		}
		void convertBufferToCppObject(const MyPropertyTree::Property* iPropertyPtr)
		{
			m_name = iPropertyPtr->name()->str();
			m_value = iPropertyPtr->value()->str();
			m_type = Type(iPropertyPtr->type());

			m_subproperty.clear();

			for (int i = 0; i < iPropertyPtr->subprop()->size(); i++)
			{
				SubPropertyData SubPropertyDataObj(iPropertyPtr->subprop()->Get(i)->data1()->str(), iPropertyPtr->subprop()->Get(i)->data2());

				m_subproperty.push_back(SubPropertyDataObj);
			}
		}

		void createBuffer(vector< uint8_t>& oBuffer, const string& iIdentifier)
		{
			flatbuffers::FlatBufferBuilder builder(1024);
			
			auto propertyName = builder.CreateString(m_name);

			auto propertyValue = builder.CreateString(m_value);

			std::vector<flatbuffers::Offset<SubProperty>> subProperty_vector;

			for (int i = 0; i < 1; i++)
			{
				auto stringData = builder.CreateString(m_subproperty[i].m_data1 + " : " + iIdentifier);

				short shortData = m_subproperty[i].m_data2;

				auto subProperty = CreateSubProperty(builder, stringData, shortData);

				subProperty_vector.push_back(subProperty);
			}

			auto subProperty = builder.CreateVector(subProperty_vector);

			auto property = CreateProperty(builder, propertyName, propertyValue, Type_X, subProperty);

			builder.Finish(property);

			oBuffer.resize(builder.GetSize());

			memcpy(&oBuffer[0], builder.GetBufferPointer(), builder.GetSize());

			cout << "Size:" << builder.GetSize()  << endl;
		}

		void print()
		{
			cout << "---------" << endl;

			cout << m_name << endl;
			cout << m_value << endl;

			if(m_type == Type::X) 
				cout << "X"  << endl;
			else if (m_type == Type::Y)
				cout << "Y" << endl;
			else if (m_type == Type::V)
				cout << "V" << endl;

			for(const auto& it : m_subproperty)
			{
				cout << it.m_data1 << endl;
				cout << it.m_data2 << endl;
			}

			cout << "----XXX-----" << endl;
		}
	};
};

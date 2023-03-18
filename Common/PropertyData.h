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
		const size_t subpropertySize{ 3 };

	public:
		PropertyData():m_name("PropertyName"), m_value("PropertyValue"), m_type(Type::X)
		{
			for (int i = 1; i <= subpropertySize; i++)
			{
				stringstream ss; ss << i;
				m_subproperty.emplace_back("DataValue" + ss.str(), i);
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

			for (int i = 0; i < subpropertySize; i++)
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

			//cout << "Size:" << builder.GetSize()  << endl;
		}

		void print()
		{
			stringstream ss;

			ss << "-----Start----" << endl;

			ss << "Name:" << m_name << endl;
			ss << "Value:" << m_value << endl;
			ss << "Type:";

			if(m_type == Type::X) 
				ss << "X"  << endl;
			else if (m_type == Type::Y)
				ss << "Y" << endl;
			else if (m_type == Type::V)
				ss << "V" << endl;

			for(const auto& it : m_subproperty)
			{
				ss << "Data1:" << it.m_data1 << endl;
				ss << "Data2:" << it.m_data2 << endl;
			}

			ss << "-----End----" << endl;

			cout << ss.str() << endl;
		}
	};
};

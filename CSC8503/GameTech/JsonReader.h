#pragma once

#include "filereadstream.h"
#include "document.h"
#include <iostream>
#include <vector>
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace rapidjson;
using namespace std;
using namespace NCL;
using namespace Maths;

class JsonReader
{
public:
	JsonReader() {};
	~JsonReader() {};
	
	struct objectData
	{
		string* name;
		Vector3 position;
		Vector4 rotation;
		Vector3 size;
		string* colliderType;
		bool isWall;
	};

	vector<objectData*> objectsData;

	void ReadJson(const char& filename)
	{
		FILE* fp;
		fopen_s(&fp, &filename, "rb");

		//char readBuffer[10000];
		char* readBuffer = new char[65536];

		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		Document document;
		document.ParseStream(is);


		const Value& data = document["data"];
		std::cout << "Number of objects read: " << data.Size() << std::endl;

		for (int i = 0; i < data.Size(); i++)
		{
			objectData* objData = new objectData;

			std::cout << "Object " << i << std::endl;
			const Value& gameObjectData = data[i];

			for (Value::ConstMemberIterator objectIterator = gameObjectData.MemberBegin();
				objectIterator != gameObjectData.MemberEnd(); ++objectIterator)
			{
				std::cout << objectIterator->name.GetString() << ": ";

				// For string values
				if (objectIterator->value.GetType() == rapidjson::Type::kStringType)
				{
					std::cout << objectIterator->value.GetString() << "\n";

					if (strcmp(objectIterator->name.GetString(), "name") == 0)
						objData->name = new string(objectIterator->value.GetString());
					if (strcmp(objectIterator->name.GetString(), "colliderType") == 0)
					{
						//objData->colliderType = objectIterator->value.GetString();
						objData->colliderType = new string(objectIterator->value.GetString());
					}
					continue;
				}

				if (objectIterator->value.GetType() == rapidjson::Type::kFalseType)
				{
					std::cout << "false";
					objData->isWall = false;
					continue;
				}
				else if (objectIterator->value.GetType() == rapidjson::Type::kTrueType)
				{
					std::cout << "true";
					objData->isWall = true;
					continue;
				}


				// For vector values
				for (Value::ConstMemberIterator vectorIterator = objectIterator->value.MemberBegin(); vectorIterator != objectIterator->value.MemberEnd(); ++vectorIterator)
				{
					std::cout << vectorIterator->name.GetString() << ": " << vectorIterator->value.GetFloat() << " ";

					if (strcmp(objectIterator->name.GetString(), "position") == 0)
					{
						if (strcmp(vectorIterator->name.GetString(), "x") == 0)
							objData->position.x = vectorIterator->value.GetFloat();
						else if (strcmp(vectorIterator->name.GetString(), "y") == 0)
							objData->position.y = vectorIterator->value.GetFloat();
						else if (strcmp(vectorIterator->name.GetString(), "z") == 0)
							objData->position.z = -vectorIterator->value.GetFloat();	// Z is reverse to match Unity coordinate system
					}
					else if (strcmp(objectIterator->name.GetString(), "size") == 0)
					{
						if (strcmp(vectorIterator->name.GetString(), "x") == 0)
							objData->size.x = vectorIterator->value.GetFloat();
						else if (strcmp(vectorIterator->name.GetString(), "y") == 0)
							objData->size.y = vectorIterator->value.GetFloat();
						else if (strcmp(vectorIterator->name.GetString(), "z") == 0)
							objData->size.z = vectorIterator->value.GetFloat();
					}
					else if (strcmp(objectIterator->name.GetString(), "rotation") == 0)
					{
						if (strcmp(vectorIterator->name.GetString(), "x") == 0)
							objData->rotation.x = vectorIterator->value.GetFloat();
						if (strcmp(vectorIterator->name.GetString(), "y") == 0)
							objData->rotation.y = vectorIterator->value.GetFloat();
						if (strcmp(vectorIterator->name.GetString(), "z") == 0)
							objData->rotation.z = vectorIterator->value.GetFloat();
						if (strcmp(vectorIterator->name.GetString(), "w") == 0)
							objData->rotation.w = vectorIterator->value.GetFloat();
					}
					
				}

				std::cout << std::endl;
			}

			objectsData.push_back(objData);
			std::cout << std::endl;
		}


		fclose(fp);
		delete[] readBuffer;
	}

protected:

};


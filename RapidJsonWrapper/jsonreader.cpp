#include "jsonreader.h"

jsonreader::jsonreader()
{
}

jsonreader::~jsonreader()
{
}

void jsonreader::ReadJson(std::string filepath)
{
	FILE* fp;
	//fopen_s(&fp, "../JSONs/AssetsobjectData.json", "rb");
	fopen_s(&fp, "../../JSONs/AssetsobjectData.json", "rb");

	//char readBuffer[10000];
	char* readBuffer = new char[65536];

	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document document;
	document.ParseStream(is);


	const Value& data = document["data"];
	std::cout << "Number of objects read: " << data.Size() << std::endl;

	for (int i = 0; i < data.Size(); i++)
	{
		std::cout << "Object " << i << std::endl;
		const Value& gameObjectData = data[i];

		for (Value::ConstMemberIterator objectIterator = gameObjectData.MemberBegin();
			objectIterator != gameObjectData.MemberEnd(); ++objectIterator)
		{
			// Print object member name
			std::cout << objectIterator->name.GetString() << ": ";

			// Deal with string member values
			if (objectIterator->value.GetType() == rapidjson::Type::kStringType)
			{
				std::cout << objectIterator->value.GetString() << "\n";
				continue;
			}

			// Deal with vector member values
			for (Value::ConstMemberIterator vectorIterator = objectIterator->value.MemberBegin(); vectorIterator != objectIterator->value.MemberEnd(); ++vectorIterator)
			{
				std::cout << vectorIterator->name.GetString() << ": " << vectorIterator->value.GetFloat() << " ";

			}

			std::cout << std::endl;
		}

		std::cout << std::endl;

	}


	fclose(fp);
	delete[] readBuffer;
}

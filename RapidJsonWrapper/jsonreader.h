#pragma once

#include "filereadstream.h"
#include "document.h"
#include <iostream>

using namespace rapidjson;
using namespace std;


class jsonreader
{
public:
	jsonreader();
	~jsonreader();

	void ReadJson(std::string filepath);

protected:

};


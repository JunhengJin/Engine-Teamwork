#include "NetworkCommon.h"

// Can't use reference.
Document String2Document(std::string str)
{
	Document doc;
	doc.Parse(str.c_str());
	return doc;
}

std::string Document2String(Document& d)
{
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);

	return buffer.GetString();
}
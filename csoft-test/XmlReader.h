#pragma once

#include "tinyxml2.h"

#include <string>
#include <list>
#include <vector>
#include <map>

class XmlReader
{
public:
    bool init(const char* fileName);
    std::map<const char*, float> getPlane();
    std::list<std::map<const char*, float>> getLines();

private:
    bool next(const char* name);
    std::map<const char*, float> read(const char* fieldsName[], int size);

    tinyxml2::XMLDocument xml_doc;
    tinyxml2::XMLNode* root;
    tinyxml2::XMLElement* element;
};

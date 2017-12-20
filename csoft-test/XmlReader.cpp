#include "XmlReader.h"

#include <iostream>
#include <cstdlib>

bool XmlReader::init(const char* fileName)
{
    tinyxml2::XMLError eResult = xml_doc.LoadFile(fileName);

    if (eResult != tinyxml2::XML_SUCCESS)
        std::cout << "error to load file" << std::endl;
    else
        std::cout << xml_doc.Error() << std::endl;

    root = xml_doc.FirstChildElement();

    if (root == nullptr)
    {
        std::cout << "Error to first child" << std::endl;
        return false;
    }

    return true;
}

bool XmlReader::next(const char* name)
{
    element = element->NextSiblingElement(name);

    if (element != nullptr)
         return true;

    return false;
}

std::map<const char*, float> XmlReader::read(const char* fieldsName[], int size)
{
    std::map<const char*, float> coordinate;

    for (int i = 0; i < size; ++i)
    {
        tinyxml2::XMLElement *item = element->FirstChildElement(fieldsName[i]);
        if (item != nullptr)
        {
            float value;
            tinyxml2::XMLError error = item->QueryFloatText(&value);
            if (error == tinyxml2::XML_SUCCESS)
                coordinate[fieldsName[i]] = value;
            else
                coordinate[fieldsName[i]] = 0;
        }
        else
            std::cout << fieldsName[i] << " field not found" << std::endl;
    }

    return coordinate;
}

std::map<const char*, float> XmlReader::getPlane()
{
    const char* fields[] = { "x", "y", "z", "n_x", "n_y", "n_z" };
    uint8_t fieldsSize = 6;
    std::map<const char*, float> coordinate;

    element = root->FirstChildElement("plane");

    if (element != nullptr)
    {
        do
        {
            auto coordinate_tmp = read(fields, fieldsSize);
            uint8_t count = 0;

            for (uint8_t i = 0; i < 3; ++i)
            {
                if (coordinate_tmp[fields[i+3]] != 0)
                    count++;
            }

            if (count == 1)
                coordinate = coordinate_tmp;

        } while (next("plane"));
    }

    return coordinate;
}

std::list<std::map<const char*, float>> XmlReader::getLines()
{
    const char *fields[] = {"x", "y", "z", "length", "n_x", "n_y", "n_z"};
    uint8_t fieldsSize = 7;
    std::list<std::map<const char*, float>> list;

    element = root->FirstChildElement("line");

    if (element != nullptr)
    {
        do
        {
            auto coordinate = read(fields, fieldsSize);
            uint8_t count = 0;

            for (uint8_t i = 0; i < 3; ++i)
            {
                if (coordinate[fields[i+4]] != 0)
                    count++;
            }

            if (count == 1 && coordinate["length"] > 0)
                list.push_back(coordinate);

        } while (next("line"));
    }

    return list;
}

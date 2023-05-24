#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <string>

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
    unsigned char *data;
};
#endif
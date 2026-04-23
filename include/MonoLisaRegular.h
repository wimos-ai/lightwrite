#ifndef MONOLISAREGULAR_H
#define MONOLISAREGULAR_H

#include <stddef.h>

namespace bin2cpp
{
  #ifndef BIN2CPP_FILE_OBJECT_CLASS
  #define BIN2CPP_FILE_OBJECT_CLASS
  class File
  {
  public:
    virtual size_t getSize() const = 0;
    /* DEPRECATED */ virtual inline const char * getFilename() const { return getFileName(); }
    virtual const char * getFileName() const = 0;
    virtual const char * getFilePath() const = 0;
    virtual const char * getBuffer() const = 0;
    virtual bool save(const char * filename) const = 0;
  };
  #endif //BIN2CPP_FILE_OBJECT_CLASS
  const File & getMonoLisaRegularTtfFile();
}; //bin2cpp

#endif //MONOLISAREGULAR_H

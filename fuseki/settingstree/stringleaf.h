#ifndef STRINGLEAF_H
#define STRINGLEAF_H

#include "settingstree/leaf.h"

namespace fuseki {
namespace settingsTree {

class StringLeaf : public Leaf {
  private:
    StringLeaf();
    StringLeaf(const StringLeaf&);
  public:
    StringLeaf(
        const String& name,
        const String& readers,
        const String& writers,
        const Branch* parent,
        Server* server
      );
    virtual ~StringLeaf() { }
  protected:
    String value;

    virtual String setValue(const String& v);
    virtual String getValue() const;
};

}}

#endif // STRINGLEAF_H


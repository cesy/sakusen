#ifndef TEDOMARI__UI__MODIFIED_KEY_H
#define TEDOMARI__UI__MODIFIED_KEY_H

#include <set>

#include <sakusen/gnu_extensions.h>
#include <sakusen/stringutils.h>
#include "ui/key.h"

namespace tedomari {
namespace ui {

class ModifiedKey {
  public:
    ModifiedKey() : key(K_Unknown), modifiers() {}
    ModifiedKey(Key k, const std::set<String>& m) : key(k), modifiers(m) {}
    ModifiedKey(const String& name);
  private:
    Key key;
    std::set<String> modifiers;
  public:
    inline bool operator==(const ModifiedKey& op) const {
      return key == op.key && modifiers == op.modifiers;
    }
    inline Key getKey() const { return key; }
    //#ifndef _MSC_VER
      inline const std::set<String>& getModifiers() const { return modifiers; }
    //#else
    ///*This is required due to some peculiarity of the way stlport does allocators*/
    //  inline std::set<String> getModifiers() const{ return modifiers; }
    //#endif
};

class ModifiedKeyHash {
  private:
    KeyHash keyHasher;
    boost::hash<String> stringHasher;
  public:
    inline size_t operator()(const ModifiedKey& k) const {
      size_t hash = keyHasher(k.getKey());
      for (std::set<String>::const_iterator mod = k.getModifiers().begin(); mod != k.getModifiers().end(); ++mod)
      {
        hash *= 17;
        hash += stringHasher(*mod);
      }
      return hash;
    }
};

}}

#endif // TEDOMARI__UI__MODIFIED_KEY_H


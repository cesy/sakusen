#include "modifiedkeyevent.h"

namespace tedomari {
namespace ui {

ModifiedKeyEvent::ModifiedKeyEvent(const String& name)
{
  assert(!name.empty());
  down = true;
  String modifiedKeyName(name);

  if (name[0] == '+') {
    modifiedKeyName.erase(modifiedKeyName.begin());
  } else if (name[0] == '-') {
    down = false;
    modifiedKeyName.erase(modifiedKeyName.begin());
  }

  modifiedKey = ModifiedKey(modifiedKeyName);
}

}}


#include "resourcetype.h"

#include "libsakusen-global.h"

#include <ostream>

using namespace std;

using namespace sakusen;

ostream& sakusen::operator<<(ostream& output, ResourceType resource)
{
  switch(resource) {
    #define CASE(type) case type: output << #type; break;
    CASE(resourceType_universe)
    CASE(resourceType_mapTemplate)
    CASE(resourceType_source)
    CASE(resourceType_module)
    #undef CASE
    default:
      output << "resourceType_" << uint32(resource);
      break;
  }
  return output;
}


#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include "libsakusen-global.h"

#include <list>
#include <sstream>

#include <boost/functional/hash/hash.hpp>

#include "gnu_extensions.h"

/** \file
 * \brief String utilities
 *
 * This header defines various utilities (primarily functions) for
 * manipulating strings.
 */

namespace sakusen {

/** \brief Convenience typedef for a set of strings */
typedef __gnu_cxx::hash_set<String, boost::hash<String> > hash_set_string;

/** \brief Convenience metafunction for constructing a map keyed by strings */
template<typename T>
struct hash_map_string {
  typedef __gnu_cxx::hash_map<String, T, boost::hash<String> > type;
};

/** \brief Split the string splitee at characters contained in spliton */
LIBSAKUSEN_API std::list<String> stringUtils_split(
    const String& splitee,
    const String& spliton
  );

/** \brief Join the list of strings together, joining them with delim */
LIBSAKUSEN_API String stringUtils_join(
    const std::list<String>& strings,
    const String& delim
  );

/** \brief Convert all characters of the argument into printable characters */
String LIBSAKUSEN_API stringUtils_makePrintable(const String&);

/** \brief Converts given unicode character to a UTF-8 string */
String LIBSAKUSEN_API stringUtils_charToUtf8(uint16);

/** \brief Runs the give iterator backwards until it's pointing at the start of
 * a UTF-8 character or it matches bound */
String::iterator LIBSAKUSEN_API stringUtils_findPreviousCharStart(
    String::iterator i,
    const String::iterator& bound
  );

/** \brief Convert a String to an number  */
template<typename T>
inline T numFromString(String str);

template<>
inline uint8 numFromString<uint8>(String str)
{
  std::istringstream s(str);
  sint32 i; /* Has to be an int32 because uint8 interpreted as a char */
  s >> i;
  assert(uint8(i) == i); /* Check for overflow */
  return i;
}

template<typename T>
inline T numFromString(String str)
{
  std::istringstream s(str);
  T i;
  s >> i;
  return i;
}

/** \brief Converts a number to a String */
template<typename T>
inline String numToString(T i);

template<>
inline String numToString<uint8>(uint8 i)
{
  std::ostringstream s;
  s << static_cast<uint32>(i); /* Has to be cast because otherwise
                                   interpreted as a char */
  return s.str();
}

template<typename T>
inline String numToString(T i)
{
  std::ostringstream s;
  s << i;
  return s.str();
}

String stringUtils_bufferToHex(const uint8* buffer, size_t length);

String LIBSAKUSEN_API stringUtils_getSecureHashAsString(const uint8* buffer, size_t length);

}

#endif // STRINGUTILS_H


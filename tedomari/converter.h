#ifndef TEDOMARI__CONVERTER_H
#define TEDOMARI__CONVERTER_H

#include "global.h"
#include "global.h"

#include <boost/utility.hpp>

namespace tedomari {

/** \brief Handles conversion of text between the native encoding (used for
 * terminal I/O) and UTF-8 (used for all sakusen strings internally)
 *
 * When DISABLE_CONVERSION is defined, this class performs no alterations to
 * the strings it is given to convert, but simply passes them on verbatim.
 *
 * \warning This may yield invalid UTF-8 strings.
 *
 * Otherwise, the icu library is used to convert strings between the native
 * encoding (as determined by locale settings) and UTF-8. */
class Converter : private boost::noncopyable {
  public:
    Converter();
    ~Converter();
#ifdef DISABLE_CONVERSION
    /** when DISABLE_CONVERSION is set we just forward
     * the strings verbatim */
    inline String convertNativeToUTF8(const String& s) {
      return s;
    }
    inline String convertUTF8ToNative(const String& s) {
      return s;
    }
#else
  private:
    const char* nativeLocale;
  public:
    /** \brief Converts a string from the native codeset to UTF-16 */
   String convertNativeToUTF8(const String&);
    /** \brief Converts a string from UTF-8 to the native codeset */
   String convertUTF8ToNative(const String&);
#endif
};

}

#endif // TEDOMARI__CONVERTER_H


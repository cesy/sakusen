#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "libsakusen-resources-global.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <list>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#define NativeMkdir(a,b) _mkdir(a)
#define NativeStat(a,b) _stat(a,b)
#define NativeFstat(a,b) _fstat(a,b)
#define NativeStructStat struct _stat
#define S_ISDIR(mode) (((mode) & _S_IFDIR) != 0)
#define NativeUnlink(a) _unlink(a)
#define NativeFtell64(a) _ftelli64(a)
#define NativeFileno(a) _fileno(a)
#else
#include <dirent.h>
#include <sys/unistd.h>
#define NativeMkdir(a,b) mkdir(a, b)
#define NativeStat(a,b) stat(a,b)
#define NativeFstat(a,b) fstat(a,b)
#define NativeStructStat struct stat
#define NativeUnlink(a) unlink(a)
/* Note that for ftello to really be 64-bit requires
 * #define _FILE_OFFSET_BITS 64, as in libsakusen-resource-global.h */
#define NativeFtell64(a) ftello(a)
#define NativeFileno(a) fileno(a)
#endif

namespace sakusen {
namespace resources {

LIBSAKUSEN_RESOURCES_API size_t fileUtils_read(
    int fd,
    void* buffer, size_t bufferLen
  );

LIBSAKUSEN_RESOURCES_API size_t fileUtils_write(
    int fd,
    const void* buffer, size_t length
  );

LIBSAKUSEN_RESOURCES_API void fileUtils_mkdirRecursive(
    String path,
    mode_t mode
  );

/** \brief Returns all files in the directory whose name
 * begins with the given name.
 * 
 * \todo More general behaviour is needed.
 *
 * \warning This method is not thread-safe!
 */
LIBSAKUSEN_RESOURCES_API std::list<String> fileUtils_findMatches(
    const String& directory,
    const String& name
  );

LIBSAKUSEN_RESOURCES_API String fileUtils_getHome();

LIBSAKUSEN_RESOURCES_API String fileUtils_notDirPart(const String& path);

}}

#endif // FILEUTILS_H


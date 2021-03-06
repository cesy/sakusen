#include <sakusen/resources/vfs/directorybranch.h>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/xpressive/regex_algorithms.hpp>

#include <sakusen/fileutils.h>
#include <sakusen/resources/filereader.h>
#include <sakusen/resources/filewriter.h>

using namespace std;

namespace sakusen {
namespace resources {
namespace vfs {

DirectoryBranch::DirectoryBranch(
    const String& name,
    const boost::filesystem::path& d,
    const Branch::Ptr& parent
  ) :
  Branch(name, parent),
  directory(d)
{
}

Branch::Ptr DirectoryBranch::getSubBranch(const String& subBranchName)
{
  /* for security reasons, it's crucial that the name contain no characters
   * that might be directory separators; for now we insist more
   * strongly that it match the filenameRegex. */
  if (!regex_match(subBranchName, filenameRegex)) {
    SAKUSEN_FATAL("invalid name");
  }
  assert(!subBranchName.empty());
  if (subBranchName == "..") {
    return parent.lock();
  }
  if (subBranchName == ".") {
    return ptrToThis.lock();
  }

  boost::filesystem::path potential = directory / subBranchName;
  if (!boost::filesystem::exists(potential)) {
    return Branch::Ptr();
  } else if (boost::filesystem::is_directory(potential)) {
    return create(subBranchName, potential, ptrToThis.lock());
  } else {
    /** \todo Support archives, etc. */
    return Branch::Ptr();
  }
}

Branch::Ptr DirectoryBranch::createSubBranch(const String& subBranchName)
{
  /* for security reasons, it's crucial that the name contain no characters
   * that might be directory separators; for now we insist more
   * strongly that it match the filenameRegex. */
  if (!regex_match(subBranchName, filenameRegex)) {
    SAKUSEN_FATAL("invalid name");
  }
  assert(!subBranchName.empty());
  /** \todo memoize? */
  if (subBranchName == "..") {
    return parent.lock();
  }
  if (subBranchName == ".") {
    return ptrToThis.lock();
  }

  boost::filesystem::path potential = directory / subBranchName;
  boost::filesystem::create_directory(potential);
  return create(subBranchName, potential, ptrToThis.lock());
}

list<Resource> DirectoryBranch::getResources(
    const String& resourceName,
    const String& extension
  )
{
  if (!boost::filesystem::exists(directory)) {
    return list<Resource>();
  }

  /*SAKUSEN_QDEBUG("Searching "<<directory.native_directory_string());*/
  list<boost::filesystem::path> matches =
    fileUtils_findMatches(directory, resourceName);
  list<Resource> result;

  BOOST_FOREACH(const boost::filesystem::path& match, matches) {
    String filename = match.filename().string();
    if (!boost::algorithm::ends_with(filename, extension)) {
        /*SAKUSEN_QDEBUG("Looking for extension to "<<name<<" "<<extension);*/
      continue;
    }
    Reader::Ptr reader(new FileReader(match));
    result.push_back(Resource(getSakusenPath()+"/"+filename, reader, match));
  }

  return result;
}

Writer::Ptr DirectoryBranch::getWriter(const String& writerName)
{
  /* for security reasons, it's crucial that the name contain no characters
   * that might be directory separators and isn't ..; for now we insist more
   * strongly that it match the filenameRegex. */
  if (!regex_match(writerName, filenameRegex)) {
    SAKUSEN_FATAL("invalid name");
  }
  boost::filesystem::path filePath = directory / writerName;
  if (boost::filesystem::exists(filePath) &&
      boost::filesystem::is_directory(filePath)) {
    return Writer::Ptr();
  }
  return Writer::Ptr(new FileWriter(filePath));
}

}}}


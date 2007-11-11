#include "asynchronousiohandler.h"
#include "errorutils.h"
#include "fileutils.h"
#include "fileioexn.h"

#ifndef DISABLE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <fcntl.h>

#include <cerrno>
#include <ostream>
#include <algorithm>
#include <boost/filesystem/operations.hpp>

using namespace std;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;
using namespace tedomari;

#ifdef DISABLE_READLINE

#define INPUT_BUFFER_LEN 512

#ifdef WIN32
#include <conio.h>
#include <stdio.h>
#endif

AsynchronousIOHandler::AsynchronousIOHandler(
    FILE* in,
    ostream& o,
    String hf,
    int hl
  ) :
  infd(NativeFileno(in)),
  out(o),
  historyFile(hf),
  historyLength(hl),
  commandBuffer(),
  eof(false),
  inputBuffer()
{
  out << "> " << flush;
#ifdef _MSC_VER
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
  hStdin = GetStdHandle(STD_INPUT_HANDLE);
  if (hStdin == INVALID_HANDLE_VALUE) 
  {
    /* No reason why this should stop the whole show. */
    Fatal("Error getting console handle. Will try again later.");
    return;
  }

  //Change the console mode to single-character. 
  if (! GetConsoleMode(hStdin, &fdwOldMode)) 
  {
    Fatal("GetConsoleMode Console Error"); 
    return;
  }

  fdwMode = fdwOldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT); 
  if (! SetConsoleMode(hStdin, fdwMode)) 
  {
    Fatal("SetConsoleMode Console Error"); 
    return;
  }
#endif
}

AsynchronousIOHandler::~AsynchronousIOHandler()
{
#ifdef _MSC_VER
  SetConsoleMode(hStdin, fdwOldMode);
#endif
}

void AsynchronousIOHandler::updateBuffer(const struct ::timeval& timeout)
{
#ifndef _MSC_VER
  fd_set inSet;
  FD_ZERO(&inSet);
  FD_SET(infd, &inSet);
  while(true) {
    struct timeval localTimeout = timeout;
    switch(select(infd+1, &inSet, NULL, NULL, &localTimeout)) {
      case -1:
        Fatal("select failed: " << errorUtils_parseErrno(socket_errno));
      case 0:
        /* Nothing further */
        return;
      case 1:
        /* There is something to read */
        {
          char buf[INPUT_BUFFER_LEN];
          /* Read what we can from the input file descriptor */
          try {
            size_t bytesRead = fileUtils_read(infd, buf, INPUT_BUFFER_LEN);
            /* Append what we've read to the input buffer */
            inputBuffer.append(buf, bytesRead);
          } catch (FileIOExn&) {
            Fatal("error reading input: " << errorUtils_errorMessage(errno));
          }
          /* Strip newline-delimited commands from the input buffer */
          String::iterator nl =
            find(inputBuffer.begin(), inputBuffer.end(), '\n');
          while (nl != inputBuffer.end()) {
            commandBuffer.push(String(inputBuffer.begin(), nl));
            inputBuffer.erase(inputBuffer.begin(), nl+1);
            nl = find(inputBuffer.begin(), inputBuffer.end(), '\n');
          }
        }
        break;
      default:
        Fatal("Unexpected return value from select.");
    }
  }
#else
    
/* Read what we can from the console */
  while (_kbhit())
  {
    char buf[3];
    if (! ReadConsole(hStdin, buf,1, &cRead, NULL) ){
      out<<"Error Reading from Console."<<endl;
      break;
    }
    if (! WriteConsole(hStdout,buf,1,&cWritten,NULL) ){
      out<<"Error Writing to Console."<<endl;
      break;
    }      
    /* Append what we've read to the input buffer */
    /*QDebug("[updateBuffer] got char " << chr);*/
    /* If a backspace is pressed, we want to remove a character from the buffer 
     * and then move the cursor back one.*/
    if (buf[0] == '\b'){
      CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
      GetConsoleScreenBufferInfo(hStdout, &ScreenBufferInfo);
      if(ScreenBufferInfo.dwCursorPosition.X>=2)
      {
        String temp(inputBuffer, 0, inputBuffer.length()-1);
        inputBuffer=temp;
        SetConsoleCursorPosition(hStdout, ScreenBufferInfo.dwCursorPosition);
        WriteConsole(hStdout," ",1,&cWritten,NULL);
        SetConsoleCursorPosition(hStdout, ScreenBufferInfo.dwCursorPosition);
      }
      else{
        ScreenBufferInfo.dwCursorPosition.X+=1;
        SetConsoleCursorPosition(hStdout, ScreenBufferInfo.dwCursorPosition);
      }
    }
    else
    {
      inputBuffer += buf[0];
      if ((buf[0] == '\n') || (buf[0] == '\r')) {
        out << "\n> " << flush;
      }
    }        
  }

  static const char newlines[2] = { '\n', '\r' };

  /* Strip newline-delimited commands from the input buffer */
  String::iterator nl = find_first_of(inputBuffer.begin(), inputBuffer.end(), newlines, newlines+2);
  while (nl != inputBuffer.end()){
    /*QDebug("[updateBuffer] found command");*/
    commandBuffer.push(String(inputBuffer.begin(), nl));
    inputBuffer.erase(inputBuffer.begin(), nl+1);
    nl = find(inputBuffer.begin(), inputBuffer.end(), '\n');
  } 
#endif
}

void AsynchronousIOHandler::message(const String& message)
{
  struct timeval timeout = {0, 0};
  updateBuffer(timeout);
  out << "\n" << converter.convertUTF8ToNative(message) <<
    "> " << inputBuffer << flush;
}

#else // DISABLE_READLINE

/** \brief Static data for tedomari::line_callback_handler to use
 * 
 * \internal */
AsynchronousIOHandler* handler = NULL;

/** \brief Static function because it needs to be used as a C-style function
 * pointer
 *
 * \relates AsynchronousIOHandler */
void tedomari::line_callback_handler(char* line)
{
  /* The readline library has passed us this completed line of input - we pass
   * it on to the IO handler */
  assert(handler != NULL);
  if (line == NULL) {
    handler->eof = true;
  } else {
    if (*line != '\0') {
      if (handler->lastLine != line) {
        add_history(line);
        handler->lastLine = line;
      }
    }
    handler->commandBuffer.push(line);
    free(line);
  }
}  

AsynchronousIOHandler::AsynchronousIOHandler(
    FILE* in,
    ostream& o,
    const boost::filesystem::path& hf,
    int hl
  ) :
  infd(fileno(in)),
  out(o),
  historyFile(hf),
  historyLength(hl),
  commandBuffer(),
  eof(false)
{
  if (handler != NULL) {
    Fatal("cannot install multiple AsynchronousIOHandlers");
  }
  handler = this;
  rl_callback_handler_install("> ", line_callback_handler);
  if (boost::filesystem::exists(historyFile)) {
    if (!boost::filesystem::is_directory(historyFile)) {
      if (0 == read_history(historyFile.native_file_string().c_str())) {
        if (0 == history_set_pos(history_length)) {
          Debug("error setting history position");
        }
        /** \todo set the value of lastLine appropriately */
      } else {
        Debug("error reading history");
      }
    }
  }
}

AsynchronousIOHandler::~AsynchronousIOHandler()
{
  if (0 == write_history(historyFile.native_file_string().c_str())) {
    if (0 != history_truncate_file(
          historyFile.native_file_string().c_str(), historyLength
        )) {
      Debug("error truncating history");
    }
  } else {
    Debug("error writing history");
  }
  rl_callback_handler_remove();
  handler = NULL;
}

void AsynchronousIOHandler::updateBuffer(const struct timeval& timeout2)
{
  fd_set inSet;
  FD_ZERO(&inSet);
  FD_SET(infd, &inSet);
  struct timeval timeout=timeout2;
  while(true) {
    switch(select(infd+1, &inSet, NULL, NULL, &timeout)) {
      case -1:
        Fatal("select failed , errno=" << errno << " (" <<
          errorUtils_parseErrno(errno) << ")");
      case 0:
        /* Nothing further */
        return;
      case 1:
        /* There is something to read */
        rl_callback_read_char();
        break;
      default:
        Fatal("Unexpected return value from select.");
    }
  }
}

void AsynchronousIOHandler::message(const String& message)
{
  struct timeval timeout = {0, 0};
  updateBuffer(timeout);
  out << "\n" << converter.convertUTF8ToNative(message);
  rl_on_new_line();
  rl_redisplay();
}

#endif // DISABLE_READLINE

bool AsynchronousIOHandler::getCommand(
    String& command,
    const struct timeval& timeout)
{
  updateBuffer(timeout);
  if (!commandBuffer.empty()) {
    command = converter.convertNativeToUTF8(commandBuffer.front());
    commandBuffer.pop();
    return true;
  }

  return false;
}

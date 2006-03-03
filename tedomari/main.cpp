#include "libsakusen-global.h"
#include "stringutils.h"
#include "partialworld.h"
#include "libsakusen-comms-global.h"
#include "unixdatagramconnectingsocket.h"
#include "errorutils.h"
#include "libsakusen-resources-global.h"
#include "fileutils.h"
#include "fileresourceinterface.h"

#include "tedomari-global.h"
#include "revision.h"
#include "serverinterface.h"
#include "asynchronousiohandler.h"
#include "game/game.h"

/* TODO: This include will need to be guarded by whatever symbol is defined by
 * the --enable-sdl configure switch once such a thing exists */
#include "ui/sdl/sdlui.h"

#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <list>
#include <optionsparser.h>

#define NANO 1000000000

using namespace std;
using namespace __gnu_cxx;
using namespace optimal;

using namespace sakusen;
using namespace sakusen::comms;
using namespace sakusen::resources;
using namespace sakusen::client;
using namespace tedomari;
using namespace tedomari::game;
using namespace tedomari::ui;
using namespace tedomari::ui::sdl;

/* struct to store options processed from the command line */

struct Options {
  Options() :
    abstract(true), unixSockets(true), evil(false), historyLength(100),
    test(false), solicitationAddress(), joinAddress(), help(false),
    version(false) {}
  ~Options() {}
  bool abstract;
  bool unixSockets;
  bool evil;
  int historyLength;
  bool test;
  SDLUI::Options sdlOptions;
  String solicitationAddress;
  String joinAddress;
  bool help;
  bool version;
};

/* enumeration of commands that can be entered at the tedomari prompt */

enum Command {
  command_join,
  command_leave,
  command_set,
  command_get,
  command_reconnect,
  command_resetUI,
  command_quit,
  command_help
};

/* Forward declarations */

void runTest(
    const Options& options,
    const String& homePath,
    const String& configPath
  );

void runClient(
    const Options& options,
    const String& homePath,
    const String& configPath
  );

Options getOptions(String optionsFile, int argc, char const* const* argv);

UI* newUI(const Options& o, const String& uiConfFilename, Game* game);

void usage();

int main(int argc, char const* const* argv)
{
  if (NULL == setlocale(LC_CTYPE, "")) {
    Fatal("error setting locale");
  }

  /* For the moment we simply attempt to connect to a socket where fuseki puts
   * it.
   * TODO: allow for connecting elsewhere, or not connecting at all at once */
  
  /* Seek out the home directory */
  char* homePathPtr = getenv("HOME");

  if (homePathPtr == NULL) {
    Fatal("no home directory found");
  }

  String homePath(homePathPtr);
  
  /* Construct the path to the tedomari config directory */
  String configPath = homePath + CONFIG_SUBDIR FILE_SEP "tedomari";
  cout << "Using directory " << configPath << " for tedomari configuration\n";
  struct stat tmpStat;

  if (-1 == stat(configPath.c_str(), &tmpStat)) {
    switch (errno) {
      case ENOENT:
        cout << "Directory not found, trying to create it" << endl;
        if (-1 == fileUtils_mkdirRecursive(configPath, 0777)) {
          Fatal("error " << errorUtils_parseErrno(errno) <<
              " creating directory");
        }
        break;
      default:
        Fatal("could not stat directory");
    }
  }
  
  Options options = getOptions(configPath + FILE_SEP "config", argc, argv);

  if (options.help) {
    usage();
    exit(EXIT_SUCCESS);
  }

  if (options.version) {
    cout << APPLICATION_NAME " " APPLICATION_VERSION " (revision " <<
      REVISION << ")" << endl;
    exit(EXIT_SUCCESS);
  }
  
  cout << "*******************************\n"
          "* tedomari (a Sakusen client) *\n"
          "*  Similus est circo mortis!  *\n"
          "*******************************" << endl;

  if (options.test) {
    runTest(options, homePath, configPath);
  } else {
    runClient(options, homePath, configPath);
  }
  
  return EXIT_SUCCESS;
}

void runTest(
    const Options& options,
    const String& homePath,
    const String& configPath
  ) {
  String uiConfFilename = configPath + FILE_SEP "ui.conf";
  ResourceInterface* resourceInterface =
    new FileResourceInterface(homePath + CONFIG_SUBDIR DATA_SUBDIR);
  Game* game = new Game(resourceInterface);
  UI* ui = newUI(options, uiConfFilename, game);

  struct timespec sleepTime = {0, NANO/25};
  if (options.evil) {
    sleepTime.tv_nsec = NANO/250;
  }

  while (true) {
    ui->update();
    if (ui->isQuit()) {
      break;
    }
    nanosleep(&sleepTime, NULL);
  }

  delete ui;
  ui = NULL;
  delete game;
  game = NULL;
  delete resourceInterface;
  resourceInterface = NULL;
}

void runClient(
    const Options& options,
    const String& homePath,
    const String& configPath
  ) {
  String uiConfFilename = configPath + FILE_SEP "ui.conf";
  
  String socketAddress = options.solicitationAddress;

  if (socketAddress.empty()) {
    /* Use default socket */
    socketAddress = "unix"ADDR_DELIM"concrete"ADDR_DELIM +
      homePath + CONFIG_SUBDIR SOCKET_SUBDIR FILE_SEP "fuseki-socket";
  }

  /* Construct the path to the history file */
  String historyPath = configPath + FILE_SEP "history";
  cout << "Using history file at " << historyPath << "\n";
  
  bool reconnect;

  do {
    reconnect = false;
    
    /* Connect to the socket */
    cout << "Trying to connect to socket at " << socketAddress << endl;
    
    Socket* socket = Socket::newConnectionToAddress(socketAddress);

    cout << "Connected to socket." << endl;
    
    /* TODO: the ResourceInterface actually needs to be able to access
     * resources over the network from the server as well as from disk */
    ResourceInterface* resourceInterface =
      new FileResourceInterface(homePath + CONFIG_SUBDIR DATA_SUBDIR);
    Game* game = new Game(resourceInterface);
    ServerInterface serverInterface(
        socket, options.joinAddress, options.unixSockets, options.abstract, game
      );

    cout << "Getting advertisement." << endl;
    
    AdvertiseMessageData advertisement;

    if (serverInterface.getAdvertisement(&advertisement)) {
      cout << "Failed to get advertisement.  Giving up." << endl;
      exit(1);
    }

    cout << "Advertisement:\n"
      "Server name: " << advertisement.getServerName() << "\n"
      "Game name:   " << advertisement.getGameName() << "\n"
      "\n"
      "Type 'help' for a list of commands" << endl;

    /* Create the asynchronous input handler.  Hereafter nothing else should
     * mess with stdin, because that is likely to cause problems. */
    AsynchronousIOHandler ioHandler(
        stdin, cout, historyPath, options.historyLength
      );
    UI* ui = NULL;

    bool finished = false;
    String whitespace = " \t\r\n"; /* TODO: obtain whitespace in some more
                                      general fashion.  Maybe use regexes and
                                      their associated cunningness */

    struct timespec sleepTime = {0, NANO/25};
    if (options.evil) {
      sleepTime.tv_nsec = NANO/250;
    }
    struct timespec commandSleepTime = {0, 0};

    /* Construct our commands */
    hash_map<String, ::Command, StringHash> commands;
    commands["j"] = commands["join"] = command_join;
    commands["l"] = commands["leave"] = command_leave;
    commands["g"] = commands["get"] = command_get;
    commands["s"] = commands["set"] = command_set;
    commands["q"] = commands["quit"] = command_quit;
    commands["r"] = commands["reconnect"] = command_reconnect;
    commands["u"] = commands["resetui"] = command_resetUI;
    commands["h"] = commands["?"] = commands["help"] = command_help;
    
    String helpMessage =
      "Available commands:\n"
      "  join (j)              Connect to server\n"
      "  leave (l)             Leave server\n"
      "  get (g) SETTING       Get the value of SETTING from the server\n"
      "  set (s) SETTING VALUE Submit request to change SETTING to VALUE\n"
      "  reconnect (r)         Close this connection to the server and start "
        "over\n"
      "  resetui (u)           If there is a game UI open, then close it and "
        "reopen it\n"
      "                        from scratch\n"
      "  quit (q)              Quit tedomari (closing any connection to "
        "server)\n"
      "  help (h, ?)           Display this help\n";
    
    while (!finished) {
      /* Process commands enterred through stdin */
      String command;
      String message;
      if (ioHandler.getCommand(command, &commandSleepTime)) {
        list<String> words = stringUtils_split(command, whitespace);
        if (!words.empty()) {
          String commandName = words.front();
          words.pop_front();
          if (commands.count(commandName)) {
            switch (commands[commandName]) {
              case command_quit:
                finished = true;
                break;
              case command_reconnect:
                finished = true;
                reconnect = true;
                break;
              case command_join:
                if (serverInterface.isJoined()) {
                  ioHandler.message("Already joined.\n");
                } else if ("" != (message = serverInterface.join())) {
                  ioHandler.message(message);
                } else {
                  ioHandler.message(
                      String("Joined.  Assigned client ID ") +
                        clientID_toString(serverInterface.getID()) + "\n");
                }
                break;
              case command_leave:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (serverInterface.leave(true)) {
                  ioHandler.message("Error leaving server.\n");
                } else {
                  ioHandler.message("Left.\n");
                }
                break;
              case command_get:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (words.size() != 1) {
                  ioHandler.message("Usage: get SETTING\n");
                } else {
                  String setting = words.front();
                  words.pop_front();
                  if (serverInterface.getSetting(setting)) {
                    ioHandler.message("Error setting setting\n");
                  }
                }
                break;
              case command_set:
                if (!serverInterface.isJoined()) {
                  ioHandler.message("Not joined.\n");
                } else if (words.size() != 2) {
                  ioHandler.message(
                      "Usage: set SETTING VALUE\n"
                      "(Values containing whitespace are not supported yet - "
                        "sorry!)\n"
                    );
                } else {
                  String setting = words.front();
                  words.pop_front();
                  String value = words.front();
                  words.pop_front();
                  if (serverInterface.setSetting(setting, value)) {
                    ioHandler.message("Error setting setting\n");
                  }
                }
                break;
              case command_resetUI:
                if (ui != NULL) {
                  delete ui;
                  ui = newUI(options, uiConfFilename, game);
                }
                break;
              case command_help:
                ioHandler.message(helpMessage);
                break;
              default:
                Fatal("failed to handle command " << commands[commandName]);
            }
          } else {
            ioHandler.message(
                String("Unknown command '")+commandName+
                  "'.  Type 'help' for a list of commands.\n"
              );
          }
        }
      }
      /* If we've reached end of input then stop */
      if (ioHandler.getEof()) {
        finished = true;
      }
      /* Process messages from server */
      if ("" != (message = serverInterface.flushIncoming())) {
        ioHandler.message(message);
      }
      /* Open or close UI appropriately */
      if (game->isStarted() && ui == NULL) {
        ui = newUI(options, uiConfFilename, game);
      }
      if (!game->isStarted() && ui != NULL) {
        delete ui;
        ui = NULL;
      }
      /* Allow the UI some processor time */
      if (ui != NULL) {
        ui->update();
        if (ui->isQuit()) {
          finished = true;
        }
      }
      nanosleep(&sleepTime, NULL);
    }
    
    cout << endl;
    delete ui;
    ui = NULL;
    delete game;
    game = NULL;
    delete resourceInterface;
    resourceInterface = NULL;
    delete socket;
    socket = NULL;
  } while (reconnect);
}

void usage() {
  cout << "tedomari\n"
          "\n"
          "Usage: tedomari [OPTIONS]\n"
          "\n"
          " -a-, --no-abstract,     do not use the abstract unix socket namespace\n"
          " -u-, --no-unix,         do not use any unix sockets\n"
          " -e,  --evil,            try for a higher framerate\n"
          " -l,  --history-length LENGTH, store LENGTH commands in the command history\n"
          "                         upon exiting\n"
          " -t,  --test,            don't try to connect to a server, just test the UI\n"
          "      --sdlopts OPTIONS, pass OPTIONS to the SDL UI\n"
          " -s   --solicit ADDRESS, solicit server at sakusen-style address ADDRESS\n"
          " -j   --join ADDRESS,    join server at sakusen-style address ADDRESS\n"
          " -h,  --help,            display help and exit\n"
          " -V,  --version,         display version information and exit\n"
          "" << endl;
}

/* Function to parse the command line */
Options getOptions(String optionsFile, int argc, char const* const* argv) {
  Options results = Options();
  OptionsParser parser;
  OptionsParser sdlOptionsParser = SDLUI::getParser(&results.sdlOptions);

  parser.addOption("abstract",       'a',  &results.abstract);
  parser.addOption("unix",           'u',  &results.unixSockets);
  parser.addOption("evil",           'e',  &results.evil);
  parser.addOption("history-length", 'l',  &results.historyLength);
  parser.addOption("test",           't',  &results.test);
  parser.addOption("sdlopts",        '\0', &sdlOptionsParser);
  parser.addOption("solicit",        's',  &results.solicitationAddress);
  parser.addOption("join",           'j',  &results.joinAddress);
  parser.addOption("help",           'h',  &results.help);
  parser.addOption("version",        'V',  &results.version);

  if (parser.parse(optionsFile, argc, argv)) {
    /* There was a problem */
    /* TODO: better error handling (a usage message?) */
    cout << "error(s) processing options:\n" <<
      stringUtils_join(parser.getErrors(), "\n");
    usage();
    exit(EXIT_FAILURE);
  }

  if (results.joinAddress == "" && !results.unixSockets) {
    cout << "if you disable unix sockets you must provide an explicit join "
      "address with --join" << endl;
    exit(EXIT_FAILURE);
  }

  Debug("options.sdlOptions.debug=" << results.sdlOptions.debug);
  Debug("options.unixSockets=" << results.unixSockets);

  return results;
}

UI* newUI(const Options& o, const String& uiConfFilename, Game* game)
{
  UI* ui;
  /* Hopefully this should be the only mention of SDL anywhere outside of the
   * tedomari::ui::sdl code.
   * TODO: support alternate UIs (OpenGL, DirectX) */
  ifstream uiConf(uiConfFilename.c_str());
  ui = new SDLUI(o.sdlOptions, uiConf, game);
  ui->setTitle("tedomari");
  return ui;
}


#ifndef FUSEKI__PLUGIN_H
#define FUSEKI__PLUGIN_H

#include "global.h"

#include <stack>
#include <boost/variant.hpp>

#include <sakusen/server/plugins/plugininfo.h>
#include <sakusen/server/plugins/optioncallback.h>

#include "settingstree/leaf.h"
#include "settingstree/pluginbranch.h"


namespace fuseki {

class Server;

/** \brief fuseki's implementation of PluginServerInterface
 *
 * From the perspective of fuseki, this class serves as a wrapper for the
 * PluginInfo object obtained from the plugin.  From the point of view of the
 * plugin, it serves as an interface to the server.  Thus it implements
 * PluginServerInterface, handling the plugin's options, etc. */
class Plugin :
  public sakusen::server::plugins::PluginServerInterface,
  private boost::noncopyable {
  public:
    typedef boost::shared_ptr<Plugin> Ptr;
    typedef boost::shared_ptr<const Plugin> ConstPtr;

    /** \brief Constructs plugin
     *
     * \param name This should be the full name of the plugin which is the name
     *             of the module (minus the '.la'), and the middle bit of the
     *             function which you call to get plugin info (minus the 'get_'
     *             and the '_info')
     * \param info Pointer to PluginInfo acquired from plugin
     * \param server Server which is being interfaced to
     */
    Plugin(
        const String& name,
        const sakusen::server::plugins::PluginInfo::Ptr& info,
        Server* server
      );
    ~Plugin();
  private:
    const String name;
    sakusen::server::plugins::PluginInfo::Ptr info;
    Server* server;
    settingsTree::PluginBranch::Ptr pluginBranch;
    mutable std::stack<sakusen::server::plugins::Listener::VPtr> listeners;

    typedef boost::variant<
        sakusen::server::plugins::OptionCallback<String>*,
        sakusen::server::plugins::OptionCallback<std::set<String> >*,
        sakusen::server::plugins::OptionCallback<bool>*,
        sakusen::server::plugins::OptionCallback<uint8>*,
        sakusen::server::plugins::OptionCallback<sint8>*,
        sakusen::server::plugins::OptionCallback<uint16>*,
        sakusen::server::plugins::OptionCallback<sint16>*,
        sakusen::server::plugins::OptionCallback<uint32>*,
        sakusen::server::plugins::OptionCallback<sint32>*,
        sakusen::server::plugins::OptionCallback<uint64>*,
        sakusen::server::plugins::OptionCallback<sint64>*
      > OptionCallbacks;
    mutable u_map<String, OptionCallbacks>::type optionCallbacks;
  protected:
    bool isGameStarted() const;
    sakusen::ResourceInterface::Ptr getResourceInterface() const;
    void registerListener(
        const sakusen::server::plugins::Listener::VPtr&
      ) const;
#define FUSEKI_PLUGIN_REGISTER_OPTION(type)             \
    void registerOption(                                \
        const String& name,                             \
        sakusen::server::plugins::OptionCallback<type>* callback, \
        boost::call_traits<type>::param_type initialVal \
      ) const;
    FUSEKI_PLUGIN_REGISTER_OPTION(String)
    FUSEKI_PLUGIN_REGISTER_OPTION(std::set<String>)
    FUSEKI_PLUGIN_REGISTER_OPTION(bool)
    FUSEKI_PLUGIN_REGISTER_OPTION(sint8)
    FUSEKI_PLUGIN_REGISTER_OPTION(sint16)
    FUSEKI_PLUGIN_REGISTER_OPTION(sint32)
    FUSEKI_PLUGIN_REGISTER_OPTION(sint64)
    FUSEKI_PLUGIN_REGISTER_OPTION(uint8)
    FUSEKI_PLUGIN_REGISTER_OPTION(uint16)
    FUSEKI_PLUGIN_REGISTER_OPTION(uint32)
    FUSEKI_PLUGIN_REGISTER_OPTION(uint64)
#undef FUSEKI_PLUGIN_REGISTER_OPTION
  public:
    /** \brief Returns the full internal name of the plugin */
    const String& getName() const { return name; }

    /** \brief Callback for plugin option change
     *
     * Whenever a setting in the server's settings tree corresponding to this
     * plugin is altered, it calls this callback which in turn forwards the
     * call to the appropriate OptionCallback object which it was given by the
     * plugin at initialization time. */
    template<typename T>
    String settingAlterationCallback(
        settingsTree::Leaf* altering,
        typename boost::call_traits<T>::param_type newValue
      ) {
      u_map<String, OptionCallbacks>::type::iterator optionCallback =
        optionCallbacks.find(altering->getName());
      assert(optionCallback != optionCallbacks.end());
      return boost::get<sakusen::server::plugins::OptionCallback<T>*>(
          optionCallback->second
        )->setOption(
          optionCallback->first, newValue
        );
    }
};

}

#endif // FUSEKI__PLUGIN_H


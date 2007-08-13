#ifndef UI__ACTION_H
#define UI__ACTION_H

#include <set>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "unitid.h"
#include "ui/actionargument.h"
#include "ui/uiexceptions.h"

namespace tedomari {
namespace ui {

class UI;

class Action : boost::noncopyable {
  public:
    typedef boost::shared_ptr<Action> Ptr;
    typedef boost::shared_ptr<const Action> ConstPtr;
  private:
    Action();
  protected:
    Action(ActionParameterType nextParam) : nextParameterType(nextParam) {}
  public:
    virtual ~Action() {}
  protected:
    ActionParameterType nextParameterType;

    /** \brief Internal version of supplyArgument method
     *
     * This method is reached after type-checking has been performed, so the
     * subclass may safely assume that the argument is of the correct type
     */
    virtual void internalSupplyArgument(const ActionArgument&) = 0;
    virtual void internalExecute(UI*) = 0;
    virtual const std::set<String>& internalGetStringSet() const {
      Fatal("not supported");
      return NULL;
    }
  public:
    ActionParameterType getNextParameterType() const {
      return nextParameterType;
    }
    void supplyArgument(const ActionArgument& arg) {
      if (arg.which() == actionParameterType_none ||
          arg.which() != nextParameterType) {
        throw ActionArgumentExn(
            "wrong argument type or too many arguments to Action"
          );
      }
      internalSupplyArgument(arg);
    }
    void execute(UI* ui) {
      if (nextParameterType != actionParameterType_none) {
        throw ActionExecuteExn("not all arguments supplied yet");
      }
      internalExecute(ui);
    }
    const std::set<String>& getStringSet() const {
      if (nextParameterType != actionParameterType_stringFromSet) {
        Fatal("invalid call");
      }
      return internalGetStringSet();
    }
};

Action::Ptr initializeAction(
    const String& actionName,
    const std::set<sakusen::UnitId>& selection
  );

}}

#endif // UI__ACTION_H


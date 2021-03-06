#ifndef LIBSAKUSEN__PLAYERTEMPLATE_H
#define LIBSAKUSEN__PLAYERTEMPLATE_H

#include <vector>

#include <sakusen/unittemplate.h>

namespace sakusen {

class MapTemplate;

/** \brief Defines the initial state of a player.
 *
 * PlayerTemplates appear as part of a MapPlayMode.  Each describes the type of
 * player (whether it should be client controlled, etc.) and the set of units
 * with which the player starts the game.
 *
 * For players with the option of which race to play, the choice of race can
 * affect the units.  (At time of writing it doesn't, though.)
 */
class LIBSAKUSEN_API PlayerTemplate {
  private:
    PlayerTemplate();
  public:
    PlayerTemplate(
        bool noClients,
        bool fixedRace,
        const std::vector<UnitTemplate>& units,
        const Position& startPos = Position()
      );
  private:
    bool noClients;
      /**< indicates that a player is not supposed to be controlled by
       * clients (including AI clients) */
    bool raceFixed;
      /**< indicates that there is no choice as to the race of a player */
    std::vector<UnitTemplate> units;
      /**< provides a list of all the units that a player starts with */
    Position startPos;
      /**< tells the client where to start the view */
  public:
    inline bool isNoClients() const { return noClients; }
    inline bool isRaceFixed() const { return raceFixed; }
    inline const std::vector<UnitTemplate>& getUnits() const { return units; }
    inline const Position& getStartPos() const { return startPos; }

    bool sanityCheck(
        const Universe::ConstPtr& universe,
        const MapTemplate& map
      );

    void store(OArchive&) const;
    static PlayerTemplate load(IArchive&, const DeserializationContext&);
};

}

#endif // LIBSAKUSEN__PLAYERTEMPLATE_H


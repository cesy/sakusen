#ifndef ORDER_H
#define ORDER_H

#include "ordertype.h"
#include "orderdata.h"
#include "moveorderdata.h"
#include "setvelocityorderdata.h"

class LIBSAKUSEN_API Order {
  public:
    Order();
    Order(const Order& copy);
    Order(const MoveOrderData& d);
    ~Order();
    Order& operator=(const Order& copy);
  private:
    OrderType type;
    const OrderData* data; /* Pointer to abstract class OrderData, which has
                              subclasses for the diffferent types of data
                              necessary */
  public:
    /* acccessors */
    inline bool isRealOrder(void) const {
      return getOrderType() != orderType_none;
    }
    inline OrderType getOrderType(void) const { return type; }
    inline const MoveOrderData getMoveData(void) const {
      return *dynamic_cast<const MoveOrderData*>(data);
    }
    inline const SetVelocityOrderData getSetVelocityData(void) const {
      return *dynamic_cast<const SetVelocityOrderData*>(data);
    }
};

#endif // ORDER_H


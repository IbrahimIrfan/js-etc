#include "Order.h"

Order::Order() {}
Order::Order(int id, string sym, int price, int qty, string dir, bool convert) : id{id}, symbol{sym}, price{price}, qty{qty}, dir{dir}, acked{false}, convert{convert} {}

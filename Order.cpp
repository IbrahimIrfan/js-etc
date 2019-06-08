#include "Order.h"

Order::Order() {}
Order::Order(int id, string sym, int price, int qty, string dir) : id{id}, symbol{sym}, price{price}, qty{qty}, dir{dir}, acked{false} {}

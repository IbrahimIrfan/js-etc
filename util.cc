#include "Util.h"

Utils::Utils(Connection& conn) : conn{conn}, order_id{0} {}

void Utils::buy(string sym, int price, int qty) {
    vector<string> order;
    order.push_back("ADD");
    order.push_back(to_string(order_id++));
    order.push_back(sym);
    order.push_back("BUY");
    order.push_back(to_string(price));
    order.push_back(to_string(qty));
    cout << "Sending buy order: " << order << endl;
    send_to_exchange(join(" ", order));
}

void Utils::buy(string sym, int price, int qty) {
    vector<string> order;
    order.push_back("ADD");
    order.push_back(to_string(order_id++));
    order.push_back(sym);
    order.push_back("SELL");
    order.push_back(to_string(price));
    order.push_back(to_string(qty));
    cout << "Sending sell order: " << order << endl;
    send_to_exchange(join(" ", order));
}

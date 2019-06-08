#include "Util.h"

using namespace std;

Utils::Utils(Configuration& config, Connection& conn, State& state) : order_id{0}, config{config}, conn{conn}, state{state} {}

void Utils::hello() {
    vector<string> mess;
    mess.push_back("HELLO");
    mess.push_back(config.team_name);
    string mess_str = join(" ", mess);
    cout << "Sending hello message: " << mess_str << endl;
    conn.send_to_exchange(mess_str);
}

void Utils::buy(string sym, int price, int qty) {
    if (state.open.find(sym) == state.open.end()) {
        return;
    }
    vector<string> order;
    order.push_back("ADD");
    order.push_back(to_string(order_id++));
    order.push_back(sym);
    order.push_back("BUY");
    order.push_back(to_string(price));
    order.push_back(to_string(qty));
    string order_str = join(" ", order);
    cout << "Sending buy order: " << order_str << endl;

    Order order_obj(order_id - 1, sym, price, qty, "BUY");
    state.orders[order_id - 1] = order_obj;

    conn.send_to_exchange(order_str);
}

void Utils::sell(string sym, int price, int qty) {
    if (state.open.find(sym) == state.open.end()) {
        return;
    }
    vector<string> order;
    order.push_back("ADD");
    order.push_back(to_string(order_id++));
    order.push_back(sym);
    order.push_back("SELL");
    order.push_back(to_string(price));
    order.push_back(to_string(qty));
    string order_str = join(" ", order);
    cout << "Sending sell order: " << order_str << endl;

    Order order_obj(order_id - 1, sym, price, qty, "BUY");
    state.orders[order_id - 1] = order_obj;

    conn.send_to_exchange(order_str);
}

void Utils::convert_to_stocks(string sym, int qty) {
    if (state.open.find(sym) == state.open.end()) {
        return;
    }
    vector<string> order;
    order.push_back("CONVERT");
    order.push_back(to_string(order_id++));
    order.push_back(sym);
    order.push_back("BUY");
    order.push_back(to_string(qty));
    string order_str = join(" ", order);
    cout << "Sending convert order: " << order_str << endl;
    conn.send_to_exchange(order_str);
}

void Utils::convert_to_obj(string sym, int qty) {
    if (state.open.find(sym) == state.open.end()) {
        return;
    }
    vector<string> order;
    order.push_back("CONVERT");
    order.push_back(to_string(order_id++));
    order.push_back(sym);
    order.push_back("SELL");
    order.push_back(to_string(qty));
    string order_str = join(" ", order);
    cout << "Sending convert order: " << order_str << endl;
    conn.send_to_exchange(order_str);
}

void Utils::cancel(int order_id) {
    vector<string> order;
    order.push_back("CANCEL");
    order.push_back(to_string(order_id));
    string order_str = join(" ", order);
    cout << "Cancelling order: " << order_str << endl;
    conn.send_to_exchange(order_str);
}

void Utils::read_and_parse() {
    parse_message(conn.read_from_exchange());
}

void Utils::parse_message(string resp) {
    stringstream ss(resp);

    string type;
    ss >> type;

    if (type == "HELLO") {
        cout << "Server: " << resp << endl;
        state.get_positions_from_exchange(ss);
    }
    else if (type == "OPEN") {
        cout << "Server: " << resp << endl;
        string sym;
        while (ss) {
            getline(ss, sym, ' ');
            state.open.insert(sym);
        }
    }
    else if (type == "CLOSE") {
        cout << "Server: " << resp << endl;
        string sym;
        while (ss) {
            getline(ss, sym, ' ');
            state.open.erase(sym);
        }
    }
    else if (type == "ERROR") {
        cout << "Server: " << resp << endl;
        // Nothing to do
    }
    else if (type == "BOOK") {
		// Parse the line
    	vector<string> words;
		string tmp;
    	while (ss >> tmp) {
        	words.push_back(tmp);
    	}
		string sym = words[0];

		string maxBuyVal = words[2];
		maxBuyVal = maxBuyVal.substr(0, maxBuyVal.find(":"));

		int curWord = 2;
		while (words[curWord] != "SELL") {
			curWord++;
		}
		string minSellVal = words[curWord + 1];
		minSellVal = minSellVal.substr(0, minSellVal.find(":"));

		state.book_vals[sym] = make_pair(atoi(maxBuyVal.c_str()), atoi(minSellVal.c_str()));
    }
    else if (type == "TRADE") {
        // cout << "Server: " << resp << endl;
        string stock;
        int trade_price, qty;
        ss >> stock >> trade_price >> qty;
        int former_price = state.fairvalues[stock].second;
        int n = state.fairvalues[stock].first;
        state.fairvalues[stock].second = (n * former_price + trade_price) / (n + 1);
        state.fairvalues[stock].first = n+1;
    }
    else if (type == "ACK") {
        cout << "Server: " << resp << endl;
        int order_id;
        ss >> order_id;
        state.orders[order_id].acked = true;
    }
    else if (type == "REJECT") {
        cout << "Server: " << resp << endl;
        int order_id;
        ss >> order_id;
        state.orders.erase(order_id);
    }
    else if (type == "FILL") {
        cout << "Server: " << resp << endl;
        int order_id, price, qty;
        string sym, dir;
        ss >> order_id >> sym >> dir >> price >> qty;

        if (dir == "BUY") {
            state.positions["USD"] -= price * qty;
            state.positions[sym] += qty;
        } else {
            state.positions["USD"] += price * qty;
            state.positions[sym] -= qty;
        }
    }
    else if (type == "OUT") {
        cout << "Server: " << resp << endl;
        int order_id;
        ss >> order_id;
        state.orders.erase(order_id);
    }
}

/** Join a vector of strings together, with a separator in-between
    each string. This is useful for space-separating things */
string join(string sep, vector<string> strs) {
  ostringstream stream;
  const int size = strs.size();
  for(int i = 0; i < size; ++i) {
    stream << strs[i];
    if(i != (strs.size() - 1)) {
      stream << sep;
    }
  }
  return stream.str();
}


Connection::Connection(Configuration configuration){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      throw runtime_error("Could not create socket");
    }
    string hostname = configuration.exchange_hostname;
    hostent *record = gethostbyname(hostname.c_str());
    if(!record) {
      throw invalid_argument("Could not resolve host '" + hostname + "'");
    }
    in_addr *address = reinterpret_cast<in_addr *>(record->h_addr);
    string ip_address = inet_ntoa(*address);
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(ip_address.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(configuration.exchange_port);

    int res = connect(sock, ((struct sockaddr *) &server), sizeof(server));
    if (res < 0) {
      throw runtime_error("could not connect");
    }
    FILE *exchange_in = fdopen(sock, "r");
    if (exchange_in == NULL){
      throw runtime_error("could not open socket for writing");
    }
    FILE *exchange_out = fdopen(sock, "w");
    if (exchange_out == NULL){
      throw runtime_error("could not open socket for reading");
    }

    setlinebuf(exchange_in);
    setlinebuf(exchange_out);
    this->in = exchange_in;
    this->out = exchange_out;
    this->socket_fd = res;
  }

/** Send a string to the server */
void Connection::send_to_exchange(string input) {
    string line(input);
    /* All messages must always be uppercase */
    transform(line.begin(), line.end(), line.begin(), ::toupper);
    int res = fprintf(this->out, "%s\n", line.c_str());
    if (res < 0) {
      throw runtime_error("error sending to exchange");
    }
}

string Connection::read_from_exchange() {
    /* We assume that no message from the exchange is longer
       than 10,000 chars */
    const size_t len = 10000;
    char buf[len];
    if(!fgets(buf, len, this->in)){
      throw runtime_error("reading line from socket");
    }

    int read_length = strlen(buf);
    string result(buf);
    /* Chop off the newline */
    result.resize(result.length() - 1);
    return result;
}

/* replace REPLACEME with your team name! */
Configuration::Configuration(bool test_mode) : team_name("ICY"){
    exchange_port = 20000; /* Default text based port */
    if(true == test_mode) {
      exchange_hostname = "test-exch-" + team_name;
      exchange_port += test_exchange_index;
    } else {
      exchange_hostname = "production";
    }
}

void State::get_positions_from_exchange(stringstream& ss) {
    cout << "Getting positions" << endl;
    string symbol;
    while (ss) {
        string pair;
        ss >> pair;
        if (pair == "") {
            break;
        }
        stringstream ss2(pair);
        getline(ss2, symbol, ':');

        positions[symbol] = 0;
        ss2 >> positions[symbol];
        cout << "Current position for " << symbol << ": " << positions[symbol] << endl;
    }
}


void State::print_positions() {
    for (auto position : positions) {
        cout << position.first << ": " << position.second << endl;
    }
}

void State::init() {
	book_vals["BOND"] = make_pair(998, 1002);

    fairvalues["BOND"] = make_pair(1, 1000);
    fairvalues["VALBZ"] = make_pair(0, 0);
    fairvalues["VALE"] = make_pair(0, 0);
    fairvalues["GS"] = make_pair(0, 0);
    fairvalues["MS"] = make_pair(0, 0);
    fairvalues["WFC"] = make_pair(0, 0);
    fairvalues["XLF"] = make_pair(0, 0);

    maximums["BOND"] = 100;
    maximums["VALBZ"] = 10;
    maximums["VALE"] = 10;
    maximums["GS"] = 100;
    maximums["MS"] = 100;
    maximums["WFC"] = 100;
    maximums["XLF"] = 100;

    open.insert("BOND");
    open.insert("VALBZ");
    open.insert("VALE");
    open.insert("GS");
    open.insert("MS");
    open.insert("WFC");
    open.insert("XLF");
}

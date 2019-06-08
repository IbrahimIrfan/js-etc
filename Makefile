CXX = g++
CXX_FLAGS = -Wall -O3 -std=c++14 -pthread

icy-bot: icy-bot.cpp Util.h Util.cpp Order.h Order.cpp
	$(CXX) $(CXX_FLAGS) -o icy-bot icy-bot.cpp Util.cpp Order.cpp

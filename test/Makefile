CXX=g++
CXXFLAGS=-g -Wall

scrypttest: scrypttest.cpp ../src/cryptor.cpp ../src/session.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ `pkg-config --libs openssl`

aeadtest: aeadtest.cpp ../src/cryptor.cpp ../src/session.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ `pkg-config --libs openssl`

hashertest: hashertest.cpp ../src/cryptor.cpp ../src/session.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ `pkg-config --libs openssl`

windowtest: windowtest.cpp ../src/menu.cpp
	$(CXX) $(CXXFLAGS) -lncurses $^ -o $@

clean:
	$(RM) *test

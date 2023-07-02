CXX=g++
CXXFLAGS=-g -Wall

aeadtest: aeadtest.cpp ../src/cryptor.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ `pkg-config --libs openssl`

hashertest: hashertest.cpp ../src/cryptor.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ `pkg-config --libs openssl`

windowtest: windowtest.cpp ../src/menu.cpp
	$(CXX) $(CXXFLAGS) -lncurses $^ -o $@

clean:
	$(RM) *test

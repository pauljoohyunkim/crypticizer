CXX=g++
CXXFLAGS=-g -Wall

backuptest: backuptest.cpp ../src/backup.cpp ../src/gitwrap.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ -lgit2

boxwritetest: boxwritetest.cpp ../src/session.cpp ../src/menu.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ -lncurses

tmpfiletest: tmpfiletest.cpp ../src/cryptor.cpp ../src/session.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ `pkg-config --libs openssl`

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

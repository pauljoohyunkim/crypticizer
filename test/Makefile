CXX=g++
CXXFLAGS=-g -Wall
RM=rm -rf
TEMP_GIT_REPO=temprepo

backuptest: backuptest.cpp ../src/backup.cpp ../src/gitwrap.cpp
	$(RM) $(TEMP_GIT_REPO)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lgit2
	mkdir -pv $(TEMP_GIT_REPO)
	cd $(TEMP_GIT_REPO); touch testfile{1,2,3}; git init

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
	$(RM) *test $(TEMP_GIT_REPO)

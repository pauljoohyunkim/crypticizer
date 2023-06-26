CXX=g++
CXXFLAGS=-g -Wall
BINDIR=bin
SRCDIR=src
OBJDIR=obj
BIN=crypticizer

run: $(BINDIR)/$(BIN)
	./$<

$(BINDIR)/$(BIN): $(OBJDIR)/*.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJDIR)/*.o $(BINDIR)/*

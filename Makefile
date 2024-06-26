TARGET     =  pacvim
PREFIX    ?=  /usr/local
BINDIR     =  $(PREFIX)/bin
MAPDIR     =  $(PREFIX)/share/pacvim-maps
OBJS      :=  $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))
MAPS      :=  $(wildcard maps/*)
CXX       ?=  g++
CXXFLAGS  +=  -std=c++11 -DMAPS_LOCATION='"$(MAPDIR)"'
LDLIBS    +=  -lncurses

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	install -d $(DESTDIR)$(MAPDIR)
	install -t $(DESTDIR)$(MAPDIR) $(MAPS)

install-darwin: $(TARGET)
	install $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	install -d $(DESTDIR)$(MAPDIR)
	install  $(MAPS) $(DESTDIR)$(MAPDIR)

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(TARGET)
	$(RM) -r $(DESTDIR)$(MAPDIR)

clean:
	$(RM) $(wildcard src/*.o) $(TARGET)

.PHONY: install install-darwin uninstall clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I./include -I./src -I./installers
SOURCES = src/main.cpp \
          src/SystemDetector.cpp \
          src/Installer.cpp \
          src/Uninstaller.cpp \
          installers/ArchInstaller.cpp \
          installers/DebianInstaller.cpp \
          installers/FedoraInstaller.cpp \
          installers/OpenSUSEInstaller.cpp \
          installers/VoidInstaller.cpp \
          installers/NixOSInstaller.cpp

OBJECTS = $(SOURCES:.cpp=.o)
TARGET = waydroid-installer

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/$(TARGET)

uninstall:
	rm -f /usr/local/bin/$(TARGET)

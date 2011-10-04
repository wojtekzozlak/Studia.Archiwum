SOURCES   = client_gui.cc client_interface.cc client.cc protocol.cc
HEADERS   = client_gui.h

# install
target.path = ../bin/
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS client_gui.pro
sources.path = ./
INSTALLS += target sources


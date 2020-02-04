
#
# Common installation specifications
#
#

# Remove gui dependency from everything
QT -= gui

# Path for headers - remember to add files if they should be installed
publicheaders.path = /usr/include/sensord-qt5
PLUGINPATH = /usr/lib/sensord-qt5

# Path for shared libraries
SHAREDLIBPATH = /usr/lib

INSTALLS += publicheaders

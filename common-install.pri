
#
# Common installation specifications
#
#

# Remove gui dependency from everything
QT -= gui

# Path for headers - remember to add files if they should be installed
publicheaders.path = /usr/include/sensord-qt5
PLUGINPATH = $$[QT_INSTALL_LIBS]/sensord-qt5

# Path for shared libraries
SHAREDLIBPATH = $$[QT_INSTALL_LIBS]

INSTALLS += publicheaders

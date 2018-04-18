/**
   @file loader.h
   @brief Plugin loader

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Semi Malinen <semi.malinen@nokia.com
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef LOADER_H
#define LOADER_H

#include <QString>
#include <QStringList>
#include "plugin.h"

/**
 * Utility to load plugins. Class uses singleton-pattern.
 */
class Loader
{
public:
    /**
     * Get singleton instance.
     */
    static Loader& instance();

    /**
     * Load plugin with given name. If called twice with same plugin name
     * the second call won't do anything.
     *
     * @param name plugin name.
     * @param errorMessage object to write error message if plugin loading
     *                     fails. If NULL then error message is not written.
     * @return true on success, false on failure
     */
    bool loadPlugin(const QString &name, QString *errorMessage = 0);

    /**
     * Test if a plugin is available for loading
     *
     * @param name plugin name
     * @return true if plugin is available, false if not
     */
    bool pluginAvailable(const QString &name) const;

    /**
     * Get a list of plugins available for loading
     *
     * @return Array of plugin names
     */
    QStringList availablePlugins() const;

    /**
     * Get a list of sensor plugins available for loading
     *
     * @return Array of plugin names
     */
    QStringList availableSensorPlugins() const;

private:
    Loader();
    Loader(const Loader&);
    Loader& operator=(const Loader&);

    /**
     * Resolve and load plugin with depenencies.
     *
     * @param name plugin to load.
     * @param errorString object to write error message if plugin loading fails.
     * @param stack Pending plugin load stack for detecting circular dependencies.
     */
    bool loadPluginFile(const QString &name, QString &errorString, QStringList &stack);

    void invalidatePlugin(const QString &name);

    /**
     * Resolve plugin name.
     *
     * @param pluginName plugin name.
     * @return resolved plugin name.
     */
    QString resolveRealPluginName(const QString &pluginName) const;

    QStringList loadedPluginNames_; /**< list of loaded plugins */

    QStringList availablePluginNames_; /**< list of loaded plugins */

    void scanAvailablePlugins();
};

#endif

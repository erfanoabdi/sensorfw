/**
   @file lsclient.h
   @brief LunaService signal utility

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2015-2018 Nikolay Nizov

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Lihan Guo <ext-lihan.4.guo@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
   @author Nikolay Nizov <nizovn@gmail.com>

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

#ifndef SENSORD_LUNA_SERVICE_CLIENT_H
#define SENSORD_LUNA_SERVICE_CLIENT_H

#include <QObject>
#include <luna-service2/lunaservice.h>

/**
 * Class for monitoring various LS signals.
 */
class LSClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LSClient)

public:
    /**
     * Constructor.
     *
     * @param parent Parent object.
     */
    LSClient(QObject* parent = 0);

    /**
     * Get display state.
     *
     * @return display state.
     */
    bool displayEnabled() const;

    /**
     * Get powersave-mode state.
     *
     * @return powersave-mode state.
     */
    bool PSMEnabled() const;

signals:
    /**
     * Emitted when display state has changed.
     *
     * @param displayOn \c true if display went to 'on' or 'dimmed',
     *                  \c false if 'off'.
     */
    void displayStateChanged(bool displayOn);

    /**
     * Emitted when powersave-mode has changed.
     *
     * @param PSM is powersave-mode enabled or not.
     */
    void devicePSMStateChanged(bool PSM);

private slots:
    /**
     * Slot for LS display state change signals.
     *
     * @param state name of the state.
     */
    static bool displayCallback(LSHandle *sh, LSMessage *message, void *ctx);

    /**
     * Slot for LS powersave-mode state change signals.
     *
     * @param mode is powersave-mode enabled or not.
     */
    static bool chargerCallback(LSHandle *sh, LSMessage *message, void *ctx);

private:
    bool displayState;       /**< current display state */
    bool powerSave;          /**< current powersave-mode state */

};

#endif // SENSORD_LUNA_SERVICE_CLIENT_H
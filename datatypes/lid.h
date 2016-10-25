/**
   @file lid.h
   @brief QObject based datatype for LidData

   <p>
   Copyright (C) 2016 Canonical,  Ltd.

   @author Lorn Potter <lorn.potter@canonical.com>

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

#ifndef LID_H
#define LID_H

#include <QDBusArgument>

#include <datatypes/liddata.h>

/**
 * QObject facade for #LidData.
 */
class Lid : public QObject
{
    Q_OBJECT

  //  Q_PROPERTY(uint value READ value)
    Q_PROPERTY(int type READ type)

public:
    /**
     * Default constructor.
     */
    Lid() {}

    /**
     * Constructor.
     *
     * @param LidData Source object.
     */
    Lid(const LidData& lidData);

    /**
     * Copy constructor.
     *
     * @param Lid Source object.
     */
    Lid(const Lid& lid);

    /**
     * Returns the contained #LidData.
     * @return LidData
     */
    const LidData& lidData() const { return data_; }

    /**
     * Returns Lid type.
     * @return Lid type.
     */
    LidData::Type type() const { return data_.type_; }

    unsigned value_;

private:
    LidData data_; /**< Contained Lid data */

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Lid& lid);
};

Q_DECLARE_METATYPE( Lid )

/**
 * Marshall the Lid data into a D-Bus argument
 *
 * @param argument dbus argument.
 * @param Lid data to marshall.
 * @return dbus argument.
 */
inline QDBusArgument &operator<<(QDBusArgument &argument, const Lid &lid)
{
    argument.beginStructure();
    argument << lid.lidData().timestamp_ << (int)(lid.lidData().value_) << (int)(lid.lidData().type_);
    argument.endStructure();
    return argument;
}

/**
 * Unmarshall Lid data from the D-Bus argument
 *
 * @param argument dbus argument.
 * @param Lid unmarshalled data.
 * @return dbus argument.
 */
inline const QDBusArgument &operator>>(const QDBusArgument &argument, Lid &lid)
{
    int tmp;
    argument.beginStructure();
    argument >> lid.data_.timestamp_;
    argument >> tmp;
    lid.data_.value_ = (unsigned)tmp;
    argument >> tmp;
    lid.data_.type_ = (LidData::Type)tmp;
    argument.endStructure();
    return argument;
}

#endif // LID_H

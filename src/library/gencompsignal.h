/*
 * EDA4U - Professional EDA for everyone!
 * Copyright (C) 2013 Urban Bruhin
 * http://eda4u.ubruhin.ch/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBRARY_GENCOMPSIGNAL_H
#define LIBRARY_GENCOMPSIGNAL_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/

#include <QtCore>
#include <QDomElement>
#include "../common/exceptions.h"

/*****************************************************************************************
 *  Forward Declarations
 ****************************************************************************************/

namespace library {
class GenericComponent;
}

/*****************************************************************************************
 *  Class GenCompSignal
 ****************************************************************************************/

namespace library {

/**
 * @brief The GenCompSignal class
 */
class GenCompSignal final : public QObject
{
        Q_OBJECT

    public:

        // Types
        enum SignalRole_t {
            Role_Power,     ///< Power Pins (GND, VCC, VSS,... of Devices)
            Role_Input,     ///< Input Pins
            Role_Output,    ///< Output Pins
            Role_InOut,     ///< Input/Output Pins
            Role_OpenDrain, ///< Open Collector / Open Drain Pins
            Role_Passive,   ///< Passive Pins (R, C, L)
            Role_Supply,    ///< all supply signals with the same name are automatically connected together (circuit-wide)!
        };


        // Constructors / Destructor
        explicit GenCompSignal(GenericComponent& genComp,
                               const QDomElement& domElement) throw (Exception);
        ~GenCompSignal() noexcept;

        // Getters
        const QUuid& getUuid() const noexcept {return mUuid;}
        SignalRole_t getRole() const noexcept {return mRole;}
        bool isRequired() const noexcept {return mIsRequired;}
        bool isNegated() const noexcept {return mIsNegated;}
        bool isClock() const noexcept {return mIsClock;}
        QString getName(const QString& locale = QString()) const noexcept;
        QString getDescription(const QString& locale = QString()) const noexcept;
        const QHash<QString, QString>& getNames() const noexcept {return mNames;}
        const QHash<QString, QString>& getDescriptions() const noexcept {return mDescriptions;}


    private:

        // make some methods inaccessible...
        GenCompSignal();
        GenCompSignal(const GenCompSignal& other);
        GenCompSignal& operator=(const GenCompSignal& rhs);

        // Private Static Methods
        static SignalRole_t stringToSignalRole(const QString& role) throw (Exception);
        static QString signalRoleToString(SignalRole_t role) noexcept;

        // General Attributes
        GenericComponent& mGenericComponent;
        QDomElement mDomElement;

        // Signal Attributes
        QUuid mUuid;
        SignalRole_t mRole;
        bool mIsRequired;
        bool mIsNegated;
        bool mIsClock;
        QHash<QString, QString> mNames;
        QHash<QString, QString> mDescriptions;
};

} // namespace library

#endif // LIBRARY_GENCOMPSIGNAL_H
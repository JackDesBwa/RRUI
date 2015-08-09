#    This file is part of RRUI.
#
#    RRUI is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    RRUI is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with RRUI.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE = app

QT += qml quick

SOURCES += main.cpp \
    simplesettings.cpp \
    rruidata.cpp

RESOURCES += qml.qrc

HEADERS += \
    simplesettings.h \
    rruidata.h

QMAKE_CXXFLAGS += -std=c++11

include(deployment.pri)

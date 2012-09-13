/*
    colorbutton.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton {

  /*
   * We need to subclass the QPushButton in order to make a nice
   * colorful button for Settings Window.
   */

  Q_OBJECT

public:
  ColorButton(QWidget* = 0);

  void setColor(const QColor&);
  void updateColor();

  inline const QColor&
  getColor() { return __current; }

private:
  QColor  __current;

private slots:
  /* Opens the color dialog in order to let the user pick the color */
  void __pickColor();

};

#endif // COLORBUTTON_H

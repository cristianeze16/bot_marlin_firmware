/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 * BOT Printer Firmware Mod by Cristian Hermosa cristian.ezequiel.hermosa@gmail.com| 3dlab@eant.tech | www.3dlab.eant.tech  
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

//
//  Menu speed
//
#include "src/lcd/dogm/ultralcd_DOGM.h"

#include "../../inc/MarlinConfigPre.h"

#if HAS_LCD_MENU

#include "menu_item.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../module/temperature.h"
#include "../../MarlinCore.h"

int16_t encoder_position16;




void menu_speed() {

   u8g.drawBox(0, 0, 128, 20);
   
    u8g.setColorIndex(0);
    lcd_put_u8str_P(33,8,GET_TEXT(MSG_SELECT));
   
    lcd_put_u8str_P(38,17,GET_TEXT(MSG_SPEED));
    
    u8g.setColorIndex(1);
    lcd_put_u8str_P(20,44,GET_TEXT(MSG_SPEED));

   encoder_position16 = ui.encoderPosition & 0xFFFF;
   encoder_position16 = feedrate_percentage + encoder_position16;
   
   u8g.setPrintPos(84, 44);
   u8g.print( encoder_position16 );
  
  if (ui.lcd_clicked) {
      if (ui.use_click()) {

    feedrate_percentage = encoder_position16;
    ui.goto_screen(menu_main,true);
    
      }
  // Speed:
  //
//   EDIT_ITEM(int3, MSG_SPEED, &feedrate_percentage, 10, 999);

  //
  // Manual bed leveling, Bed Z:
  //
 
  }
}

#endif // HAS_LCD_MENU

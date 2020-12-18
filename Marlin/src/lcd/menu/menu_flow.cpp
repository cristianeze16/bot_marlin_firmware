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


#include "../lcdprint.h"


#include <U8glib.h>
#include "src/lcd/dogm/ultralcd_DOGM.h"

#include "../ultralcd.h"
//
// Tune Menu
//

#include "../../inc/MarlinConfigPre.h"
// #include <U8glib.h>
#if HAS_LCD_MENU

#include "menu_item.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#include "../../module/temperature.h"
#include "../../MarlinCore.h"

int16_t new_encoder_position16;
uint16_t new_value_flow;
void menu_flow() {
// START_MENU();

    u8g.drawBox(0, 0, 128, 20);
   
    u8g.setColorIndex(0);
    lcd_put_u8str_P(33,8,GET_TEXT(MSG_SELECT));
   
    lcd_put_u8str_P(50,17,GET_TEXT(MSG_FLOW));
    
    u8g.setColorIndex(1);
    lcd_put_u8str_P(24,44,GET_TEXT(MSG_FLOW));


  //
  // Flow:

  
   new_encoder_position16 = ui.encoderPosition & 0xFFFF;

   new_encoder_position16 = planner.flow_percentage[active_extruder] + new_encoder_position16; 

   u8g.setPrintPos(84, 44);
   u8g.print( new_encoder_position16 );
   


  if (ui.lcd_clicked) {
      if (ui.use_click()) {
    planner.flow_percentage[active_extruder] = new_encoder_position16;      
    planner.refresh_e_factor(active_extruder);
  //  u8g.print(ui8tostr3rj(int(ui.encoderPosition)));
 
   ui.goto_screen(menu_main,true);
      }
   }
  // #if EXTRUDERS
  
  //   // Flow En:
  //   #if HAS_MULTI_EXTRUDER
  //     LOOP_L_N(n, EXTRUDERS)
  //       EDIT_ITEM_N(int3, n, MSG_FLOW_N, &planner.flow_percentage[n], 10, 999, []{ planner.refresh_e_factor(MenuItemBase::itemIndex); });
  //   #endif
  // #endif



  // END_MENU();
}

#endif // HAS_LCD_MENU

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
// Main Menu
//

#include "../../inc/MarlinConfigPre.h"


#if HAS_LCD_MENU

#include "menu_item.h"
#include "../../module/temperature.h"
#include "../../gcode/queue.h"
#include "../../module/printcounter.h"
#include "../../module/stepper.h"
#include "../../sd/cardreader.h"
#if ENABLED(HAS_MAIN_MENU_ICON)
#include "../dogm/icons_menu.h"
#include "../ultralcd.h"
#endif
#if HAS_GAMES && DISABLED(LCD_INFO_MENU)
  #include "game/game.h"
#endif

#if EITHER(SDSUPPORT, HOST_PROMPT_SUPPORT) || defined(ACTION_ON_CANCEL)
  #define MACHINE_CAN_STOP 1
#endif
#if ANY(SDSUPPORT, HOST_PROMPT_SUPPORT, PARK_HEAD_ON_PAUSE) || defined(ACTION_ON_PAUSE)
  #define MACHINE_CAN_PAUSE 1
#endif

#if ENABLED(PRUSA_MMU2)
  #include "../../lcd/menu/menu_mmu2.h"
#endif

#if ENABLED(PASSWORD_FEATURE)
  #include "../../feature/password/password.h"
#endif

#if ENABLED(HOST_START_MENU_ITEM) && defined(ACTION_ON_START)
  #include "../../feature/host_actions.h"
#endif
#if ENABLED(CASE_LIGHT_MENU)
  #include "../../feature/caselight.h"
#endif
#if ENABLED(HAS_MAIN_MENU_ICON) 
  #define ICONS_HEIGHT 23
  #define X_SPACER 2
  #define Y_SPACER 7
  #define SPACER_DOWN 2
  #define SPACER_UP 3
  #define ICONS_BYTES 3
  #define ICONS_BITS 23
  #define NUMBER_OF_ICONS 9
#endif

void menu_tune();
void menu_cancelobject();
void menu_motion();
void menu_temperature();
void menu_configuration();
void menu_bed_level();
void return_to_status();
void lcd_cooldown();
void menu_flow();
void menu_speed();
#if ENABLED(CUSTOM_USER_MENUS)
  void menu_user();
#endif

#if HAS_POWER_MONITOR
  void menu_power_monitor();
#endif

#if ENABLED(MIXING_EXTRUDER)
  void menu_mixer();
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  void _menu_temp_filament_op(const PauseMode, const int8_t);
  void menu_change_filament();
#endif

#if ENABLED(LCD_INFO_MENU)
  void menu_info();
#endif

#if EITHER(LED_CONTROL_MENU, CASE_LIGHT_MENU)
  void menu_led();
#endif

#if HAS_CUTTER
  void menu_spindle_laser();
#endif

extern const char M21_STR[];

void pausa_printer(){
  ui.pause_print;

}

void resumir_printer(){
  ui.resume_print;
}


void menu_main() {
  const bool busy = printingIsActive()
    #if ENABLED(SDSUPPORT)
      , card_detected = card.isMounted()
      , card_open = card_detected && card.isFileOpen()
    #endif
  ;
  
  

  if (ui.encoderPosition>=(NUMBER_OF_ICONS+1)){
    ui.encoderPosition=0;
  }
  if(busy){
    switch(ui.encoderPosition){
      case 0:
     if (ui.lcd_clicked) {
     if (ui.use_click()) return ui.goto_previous_screen_no_defer();
     }
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_b);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
      if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);
   
   u8g.setColorIndex(0);
   lcd_put_u8str_P(25,63,GET_TEXT(MSG_MAIN));
   u8g.setColorIndex(1);
   break;
    case 1:
     if (ui.lcd_clicked) {
     if (ui.use_click())ui.goto_screen(menu_temperature);}
   
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_b);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
 
      if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);
   
   u8g.setColorIndex(0);         
   lcd_put_u8str_P(30,63,GET_TEXT(MSG_TEMPERATURE));    
   u8g.setColorIndex(1);
   break;

   case 2:
    if (ui.lcd_clicked) {
     if (ui.use_click())ui.goto_screen( lcd_babystep_zoffset);}
    
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_b);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
   
      if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);
   
   u8g.setColorIndex(0);         
   lcd_put_u8str_P(30,63,GET_TEXT(MSG_BABYSTEP_Z));    
   u8g.setColorIndex(1);
   break;
   case 3:
 
   #if ENABLED(ADVANCED_PAUSE_FEATURE)
     #if E_STEPPERS == 1 && DISABLED(FILAMENT_LOAD_UNLOAD_GCODES)
      if (thermalManager.targetHotEnoughToExtrude(active_extruder))
       queue.inject_P(PSTR("M600 B0"));
  //       // GCODES_ITEM(MSG_FILAMENTCHANGE, PSTR("M600 B0"));
       else
       if (ui.lcd_clicked) {
    if (ui.use_click())  _menu_temp_filament_op(PAUSE_MODE_CHANGE_FILAMENT, 0);}
  //       SUBMENU(MSG_FILAMENTCHANGE, []{ _menu_temp_filament_op(PAUSE_MODE_CHANGE_FILAMENT, 0); });
     #else
     if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(menu_change_filament);
     }
  //     SUBMENU(MSG_FILAMENTCHANGE, menu_change_filament);
     #endif
   #endif
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_b);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
   
      if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
   
   
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);
   
   u8g.setColorIndex(0);         
   lcd_put_u8str_P(30,63,GET_TEXT(MSG_FILAMENTCHANGE));    
   u8g.setColorIndex(1);
   break;
   case 4:
   
   if (ui.lcd_clicked) {
     if (ui.use_click()) (bool*)&caselight.on, caselight.update_enabled ;
     }
    
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_b);
  
     if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
   
  
  
  
  
  
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);

   u8g.setColorIndex(0);
   lcd_put_u8str_P(25,63,GET_TEXT(MSG_LED_CONTROL));
   u8g.setColorIndex(1);  
   
   break;
   
   case 5:

   if(ui.lcd_clicked){
    if(ui.use_click())  
      #if MACHINE_CAN_PAUSE
       pausa_printer();
       
     if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused()))
      resumir_printer();
      // ui.pause_print;
  //     ACTION_ITEM(MSG_PAUSE_PRINT, ui.pause_print);
     #endif
   }
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);
 
    
    if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_b); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_b);
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
  break;
  
  case 6:
   
   if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(menu_flow,true,planner.flow_percentage[active_extruder]);
     ui.encoderPosition = 0;
     }
    
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
  
     if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_b);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);

   u8g.setColorIndex(0);
   lcd_put_u8str_P(25,63,GET_TEXT(MSG_FLOW));
   u8g.setColorIndex(1);  
  break;
  case 7:
   
    if (ui.lcd_clicked) {
     if (ui.use_click())ui.goto_screen(menu_speed,true,feedrate_percentage);
     ui.encoderPosition = 0;
     }
    
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
   
      if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_b);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_a);
   
   u8g.setColorIndex(0);         
   lcd_put_u8str_P(30,63,GET_TEXT(MSG_VELOCITY));    
   u8g.setColorIndex(1);


  break;
  
  case 8:

   if (ui.lcd_clicked) {
     if (ui.use_click())
    #if MACHINE_CAN_STOP
      ui.abort_print;
      #endif
     }
    
       



   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,led_control_a);
   
      if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused())){
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,play_a); 
   u8g.setColorIndex(0);
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_RESUME_PRINT));
   u8g.setColorIndex(1);  
    }else{
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.setColorIndex(0);
  //  lcd_put_u8str_P(10,63,GET_TEXT(MSG_PAUSE_PRINT));
   u8g.setColorIndex(1); 
    }
  //  u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,pause_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,velocidad_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cancelar_b);
   
   u8g.setColorIndex(0);         
   lcd_put_u8str_P(10,63,GET_TEXT(MSG_STOP_PRINT));    
   u8g.setColorIndex(1);





  }
  }else{

  switch(ui.encoderPosition){
    case 0:
     if (ui.lcd_clicked) {
     if (ui.use_click()) return ui.goto_previous_screen_no_defer();
     }
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_b);
 
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }



  
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);
    

   u8g.setColorIndex(0);
   lcd_put_u8str_P(25,63,GET_TEXT(MSG_MAIN));
   u8g.setColorIndex(1);  

   
    
    break;
    case 1:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
  
    if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_b);
          u8g.setColorIndex(0);         
          lcd_put_u8str_P(4,63,GET_TEXT(MSG_MEDIA_MENU));    
          u8g.setColorIndex(1);
          if(ui.lcd_clicked){
           ui.goto_screen(menu_media);
          }
         
          // SUBMENU(MSG_MEDIA_MENU, TERN(PASSWORD_ON_SD_PRINT_MENU, password.media_gatekeeper, menu_media));
          // #if PIN_EXISTS(SD_DETECT)
          //   GCODES_ITEM(MSG_CHANGE_MEDIA, M21_STR);
          // #else
          //   GCODES_ITEM(MSG_RELEASE_MEDIA, PSTR("M22"));
          // #endif
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_b);
          u8g.setColorIndex(0);
          lcd_put_u8str_P(10,63,GET_TEXT(MSG_NO_MEDIA));    
          u8g.setColorIndex(1);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }

   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a);

  //  u8g.setColorIndex(0);         
  //  lcd_put_u8str_P(4,63,GET_TEXT(MSG_MEDIA_MENU));    
  //  u8g.setColorIndex(1);
  
    //  if (card_detected) {
    //     if (!card_open) {
    //       SUBMENU(MSG_MEDIA_MENU, TERN(PASSWORD_ON_SD_PRINT_MENU, password.media_gatekeeper, menu_media));
    //       #if PIN_EXISTS(SD_DETECT)
    //         GCODES_ITEM(MSG_CHANGE_MEDIA, M21_STR);
    //       #else
    //         GCODES_ITEM(MSG_RELEASE_MEDIA, PSTR("M22"));
    //       #endif
    //     }
    //   }
    //   else {
    //     #if PIN_EXISTS(SD_DETECT)
    //       ACTION_ITEM(MSG_NO_MEDIA, nullptr);
    //     #else
    //       GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
    //     #endif
    //    }
  
    break;
    case 2:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_b);
  
   if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(menu_change_filament);
     }
   

   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }

   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a); 

   u8g.setColorIndex(0);         
   lcd_put_u8str_P(14,63,GET_TEXT(MSG_FILAMENTCHANGE));    
   u8g.setColorIndex(1); 
  
   

    break;
    case 3:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_b);

   if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(menu_temperature);
     }
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }


   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a); 

   u8g.setColorIndex(0);         
   lcd_put_u8str_P(30,63,GET_TEXT(MSG_TEMPERATURE));    
   u8g.setColorIndex(1); 


    break;
    case 4:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_b);
   
   if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(menu_bed_level);
     }

   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a); 
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a); 
 
   u8g.setColorIndex(0);         
   lcd_put_u8str_P(30,63,GET_TEXT(MSG_LEVEL_BED));    
   u8g.setColorIndex(1);
    break;
    case 5:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_b);
   if (ui.lcd_clicked) {
     if (ui.use_click()) (bool*)&caselight.on, caselight.update_enabled ;
     }
   
  


   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }

   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a); 
   
   u8g.setColorIndex(0);
   lcd_put_u8str_P(25,63,GET_TEXT(MSG_LED_CONTROL));
   u8g.setColorIndex(1); 
    break;
    case 6:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_b);
   if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(menu_motion);
     }

   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a); 
   

   u8g.setColorIndex(0);
   lcd_put_u8str_P(35,63,GET_TEXT(MSG_MOVE_AXIS));
   u8g.setColorIndex(1);
    break;
    case 7:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_b);
  
     if (ui.lcd_clicked) {
     if (ui.use_click()) 
      queue.inject_P(PSTR("M84"));
      queue.inject_P(PSTR("M117 MOTOR APAGADO "));
     }
  

   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a); 
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a); 
   

   u8g.setColorIndex(0);
   lcd_put_u8str_P(25,63,GET_TEXT(MSG_DISABLE_STEPPERS));
   u8g.setColorIndex(1);
    break;
    case 8:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);     
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_b);

   if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(lcd_cooldown);
     }
      
    
     
     
   
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }

   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a); 
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_a); 
   

   u8g.setColorIndex(0);
   lcd_put_u8str_P(45,63,GET_TEXT(MSG_COOLDOWN));
   u8g.setColorIndex(1);
    break;
    case 9:
   u8g.drawBox(0, 55, LCD_PIXEL_WIDTH, 9);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER+icon_config_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_config_b);
   if (ui.lcd_clicked) {
     if (ui.use_click()) ui.goto_screen(menu_configuration);
     }
      
   
   u8g.drawBitmapP(X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_main_a); 
   if (card_detected) {
        if (!card_open) {
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,print_from_sd_a);
        }
      }
      else {
        #if PIN_EXISTS(SD_DETECT)
          u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,no_sd_a);
        #else
          GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
        #endif
       }
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,change_filament_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,icon_temperature_a);
   u8g.drawBitmapP(X_SPACER+print_from_sd_a_width+X_SPACER+change_filament_b_width+X_SPACER+icon_temperature_a_with+X_SPACER+calibracion_a_width+X_SPACER,SPACER_UP,ICONS_BYTES,ICONS_BITS,calibracion_a);
   u8g.drawBitmapP(X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,led_control_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,icon_move_a); 
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,motor_off_a);
   u8g.drawBitmapP(X_SPACER+icon_move_a_width+X_SPACER+motor_off_a_width+X_SPACER+cooldown_b_width+X_SPACER,Y_SPACER+ICONS_HEIGHT,ICONS_BYTES,ICONS_BITS,cooldown_a);
   

   u8g.setColorIndex(0);
   lcd_put_u8str_P(22,63,GET_TEXT(MSG_CONFIGURATION));
   u8g.setColorIndex(1);  
    break;
  
  }
  }
        
    
     

  //  START_MENU();
  // // BACK_ITEM(MSG_INFO_SCREEN);

  // if (busy) {
  //   #if MACHINE_CAN_PAUSE
  //     ACTION_ITEM(MSG_PAUSE_PRINT, ui.pause_print);
  //   #endif
  //   #if MACHINE_CAN_STOP
  //     SUBMENU(MSG_STOP_PRINT, []{
  //       MenuItem_confirm::select_screen(
  //         GET_TEXT(MSG_BUTTON_STOP), GET_TEXT(MSG_BACK),
  //         ui.abort_print, ui.goto_previous_screen,
  //         GET_TEXT(MSG_STOP_PRINT), (const char *)nullptr, PSTR("?")
  //       );
  //     });
  //   #endif

  //   SUBMENU(MSG_TUNE, menu_tune);

  //   #if ENABLED(CANCEL_OBJECTS) && DISABLED(SLIM_LCD_MENUS)
  //     SUBMENU(MSG_CANCEL_OBJECT, []{ editable.int8 = -1; ui.goto_screen(menu_cancelobject); });
  //   #endif
  // }
  // else {

  //   #if !HAS_ENCODER_WHEEL && ENABLED(SDSUPPORT)

  //     // *** IF THIS SECTION IS CHANGED, REPRODUCE BELOW ***

  //     //
  //     // Autostart
  //     //
  //     #if ENABLED(MENU_ADDAUTOSTART)
  //       ACTION_ITEM(MSG_AUTOSTART, card.beginautostart);
  //     #endif

  //     if (card_detected) {
  //       if (!card_open) {
  //         SUBMENU(MSG_MEDIA_MENU, TERN(PASSWORD_ON_SD_PRINT_MENU, password.media_gatekeeper, menu_media));
  //         #if PIN_EXISTS(SD_DETECT)
  //           GCODES_ITEM(MSG_CHANGE_MEDIA, M21_STR);
  //         #else
  //           GCODES_ITEM(MSG_RELEASE_MEDIA, PSTR("M22"));
  //         #endif
  //       }
  //     }
  //     else {
  //       #if PIN_EXISTS(SD_DETECT)
  //         ACTION_ITEM(MSG_NO_MEDIA, nullptr);
  //       #else
  //         GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
  //       #endif
  //     }

  //   #endif // !HAS_ENCODER_WHEEL && SDSUPPORT

  //   if (TERN0(MACHINE_CAN_PAUSE, printingIsPaused()))
  //     ACTION_ITEM(MSG_RESUME_PRINT, ui.resume_print);

  //   #if ENABLED(HOST_START_MENU_ITEM) && defined(ACTION_ON_START)
  //     ACTION_ITEM(MSG_HOST_START_PRINT, host_action_start);
  //   #endif

  //   SUBMENU(MSG_MOTION, menu_motion);
  // }

  // #if HAS_CUTTER
  //   SUBMENU(MSG_CUTTER(MENU), menu_spindle_laser);
  // #endif

  // #if HAS_TEMPERATURE
  //   SUBMENU(MSG_TEMPERATURE, menu_temperature);
  // #endif

  // #if HAS_POWER_MONITOR
  //   SUBMENU(MSG_POWER_MONITOR, menu_power_monitor);
  // #endif

  // #if ENABLED(MIXING_EXTRUDER)
  //   SUBMENU(MSG_MIXER, menu_mixer);
  // #endif

  // #if ENABLED(MMU2_MENUS)
  //   if (!busy) SUBMENU(MSG_MMU2_MENU, menu_mmu2);
  // #endif

  // SUBMENU(MSG_CONFIGURATION, menu_configuration);

  // #if ENABLED(CUSTOM_USER_MENUS)
  //   #ifdef CUSTOM_USER_MENU_TITLE
  //     SUBMENU_P(PSTR(CUSTOM_USER_MENU_TITLE), menu_user);
  //   #else
  //     SUBMENU(MSG_USER_MENU, menu_user);
  //   #endif
  // #endif

  // #if ENABLED(ADVANCED_PAUSE_FEATURE)
  //   #if E_STEPPERS == 1 && DISABLED(FILAMENT_LOAD_UNLOAD_GCODES)
  //     if (thermalManager.targetHotEnoughToExtrude(active_extruder))
  //       GCODES_ITEM(MSG_FILAMENTCHANGE, PSTR("M600 B0"));
  //     else
  //       SUBMENU(MSG_FILAMENTCHANGE, []{ _menu_temp_filament_op(PAUSE_MODE_CHANGE_FILAMENT, 0); });
  //   #else
  //     SUBMENU(MSG_FILAMENTCHANGE, menu_change_filament);
  //   #endif
  // #endif

  // #if ENABLED(LCD_INFO_MENU)
  //   SUBMENU(MSG_INFO_MENU, menu_info);
  // #endif

  // #if EITHER(LED_CONTROL_MENU, CASE_LIGHT_MENU)
  //   SUBMENU(MSG_LEDS, menu_led);
  // #endif

  // //
  // // Switch power on/off
  // //
  // #if ENABLED(PSU_CONTROL)
  //   if (powersupply_on)
  //     GCODES_ITEM(MSG_SWITCH_PS_OFF, PSTR("M81"));
  //   else
  //     GCODES_ITEM(MSG_SWITCH_PS_ON, PSTR("M80"));
  // #endif

  // #if BOTH(HAS_ENCODER_WHEEL, SDSUPPORT)

  //   if (!busy) {

  //     // *** IF THIS SECTION IS CHANGED, REPRODUCE ABOVE ***

  //     //
  //     // Autostart
  //     //
  //     #if ENABLED(MENU_ADDAUTOSTART)
  //       ACTION_ITEM(MSG_AUTOSTART, card.beginautostart);
  //     #endif

  //     if (card_detected) {
  //       if (!card_open) {
  //         #if PIN_EXISTS(SD_DETECT)
  //           GCODES_ITEM(MSG_CHANGE_MEDIA, M21_STR);
  //         #else
  //           GCODES_ITEM(MSG_RELEASE_MEDIA, PSTR("M22"));
  //         #endif
  //         SUBMENU(MSG_MEDIA_MENU, TERN(PASSWORD_ON_SD_PRINT_MENU, password.media_gatekeeper, menu_media));
  //       }
  //     }
  //     else {
  //       #if PIN_EXISTS(SD_DETECT)
  //         ACTION_ITEM(MSG_NO_MEDIA, nullptr);
  //       #else
  //         GCODES_ITEM(MSG_ATTACH_MEDIA, M21_STR);
  //       #endif
  //     }
  //   }

  // #endif // HAS_ENCODER_WHEEL && SDSUPPORT

  // #if HAS_SERVICE_INTERVALS
  //   static auto _service_reset = [](const int index) {
  //     print_job_timer.resetServiceInterval(index);
  //     ui.completion_feedback();
  //     ui.reset_status();
  //     ui.return_to_status();
  //   };
  //   #if SERVICE_INTERVAL_1 > 0
  //     CONFIRM_ITEM_P(PSTR(SERVICE_NAME_1),
  //       MSG_BUTTON_RESET, MSG_BUTTON_CANCEL,
  //       []{ _service_reset(1); }, ui.goto_previous_screen,
  //       GET_TEXT(MSG_SERVICE_RESET), F(SERVICE_NAME_1), PSTR("?")
  //     );
  //   #endif
  //   #if SERVICE_INTERVAL_2 > 0
  //     CONFIRM_ITEM_P(PSTR(SERVICE_NAME_2),
  //       MSG_BUTTON_RESET, MSG_BUTTON_CANCEL,
  //       []{ _service_reset(2); }, ui.goto_previous_screen,
  //       GET_TEXT(MSG_SERVICE_RESET), F(SERVICE_NAME_2), PSTR("?")
  //     );
  //   #endif
  //   #if SERVICE_INTERVAL_3 > 0
  //     CONFIRM_ITEM_P(PSTR(SERVICE_NAME_3),
  //       MSG_BUTTON_RESET, MSG_BUTTON_CANCEL,
  //       []{ _service_reset(3); }, ui.goto_previous_screen,
  //       GET_TEXT(MSG_SERVICE_RESET), F(SERVICE_NAME_3), PSTR("?")
  //     );
  //   #endif
  // #endif

  // #if HAS_GAMES && DISABLED(LCD_INFO_MENU)
  //   #if ENABLED(GAMES_EASTER_EGG)
  //     SKIP_ITEM();
  //     SKIP_ITEM();
  //     SKIP_ITEM();
  //   #endif
  //   // Game sub-menu or the individual game
  //   {
  //     SUBMENU(
  //       #if HAS_GAME_MENU
  //         MSG_GAMES, menu_game
  //       #elif ENABLED(MARLIN_BRICKOUT)
  //         MSG_BRICKOUT, brickout.enter_game
  //       #elif ENABLED(MARLIN_INVADERS)
  //         MSG_INVADERS, invaders.enter_game
  //       #elif ENABLED(MARLIN_SNAKE)
  //         MSG_SNAKE, snake.enter_game
  //       #elif ENABLED(MARLIN_MAZE)
  //         MSG_MAZE, maze.enter_game
  //       #endif
  //     );
  //   }
  // #endif

  // END_MENU();
}

#endif // HAS_LCD_MENU

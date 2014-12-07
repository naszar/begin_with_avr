/* ***** BEGIN LICENSE BLOCK *****
 * Version: GPL 2.0/LGPL 2.1
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is Robert Dragutoiu.
 *
 * Portions created by the Initial Developer are Copyright (C) 2012-2013 by
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Robert Dragutoiu <contact@robertdragutoiu.ro>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2.0 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
 
#include <avr/io.h>
#include <util/delay.h>

//-----------------------------------------------------------------------------
// DATA PORT
//-----------------------------------------------------------------------------
#define LCD_DATA_PORT           PORTA   // actual data port
#define LCD_DATA_DIR            DDRA    // the direction of the com
#define LCD_DATA_PIN            PINA

    // the data pin
                                        // port
//-----------------------------------------------------------------------------
// CONTROL PORT
//-----------------------------------------------------------------------------
#define LCD_CTRL_PORT           PORTD   // control port
#define LCD_CTRL_DIR            DDRD    // control port direction
#define LCD_E                   PD7     // pos of enable in the control port
#define LCD_RS                  PD6     // pos of RS (data/command) in the 
                                        // control port
#define LCD_RW                  PD5     // pos of RW (read/write) in the
                                        // control port
//-----------------------------------------------------------------------------
// LCD config
//-----------------------------------------------------------------------------
#define LCD_MODE 		4
#define LCD_LINES 		2
#define LCD_DISPLAY             1 // 1=on, 0=off
#define LCD_CURSOR 		0 // 1=on, 0=off
#define LCD_CURSOR_BLINK        0 // 1=on, 0=off
//-----------------------------------------------------------------------------
// LCD commands
//-----------------------------------------------------------------------------
#define INIT_8BIT_1_LINE_5b7            0b00110000 // H:0x30 D:48
#define INIT_8BIT_2_LINE_5b7            0b00111000 // H:0x38 D:56
#define INIT_4BIT_1_LINE_5b7            0b00100000 // H:0x20 D:32
#define INIT_4BIT_2_LINE_5b7            0b00101000 // H:0x28 D:40
#define DISPLAY_OFF_CURSOR_OFF          0b00001000 // H:0x08 D:08
#define DISPLAY_ON_CURSOR_ON            0b00001110 // H:0x0E D:14
#define DISPLAY_ON_CURSOR_OFF           0b00001100 // H:0x0C D:12
#define DISPLAY_ON_CURSOR_BLINK         0b00001111 // H:0x0F D:15
#define SHIFT_DISPLAY_LEFT              0b00011000 // H:0x18 D:24
#define SHIFT_DISPLAY_RIGHT             0b00011110 // H:0x1C D:30
#define MOVE_CURSOR_LEFT                0b00010000 // H:0x10 D:16
#define MOVE_CURSOR_RIGHT               0b00010100 // H:0x14 D:20
#define CLEAR_DISPLAY                   0b00000001 // H:0x01 D:01
#define GOTO_LINE1                   	0b10000000 // H:0x80 D:128
#define GOTO_LINE2 		                0b11000000 // H:0xc0 D:192

/*
Set DDRAM address or coursor position on display 0x80+add*
128+add* 17
Set CGRAM address or set pointer to CGRAM location
0x40+add** 64+add**
 */

//-----------------------------------------------------------------------------
// MACRO ZONE
//************
// this zone will have the macros according to the pins for the command port
//-----------------------------------------------------------------------------
#define SET_E()                 (LCD_CTRL_PORT|=(1<<LCD_E))
#define CLEAR_E()               (LCD_CTRL_PORT&=(~(1<<LCD_E)))
#define SET_RW_READ_MODE()      (LCD_CTRL_PORT|=(1<<LCD_RW))
#define SET_RW_WRITE_MODE()     (LCD_CTRL_PORT&=(~(1<<LCD_RW)))
#define SET_RS_COMMAND()        (LCD_CTRL_PORT&=(~(1<<LCD_RS)))
#define SET_RS_DATA()           (LCD_CTRL_PORT|=(1<<LCD_RS))
//-----------------------------------------------------------------------------
// MACRO ZONE
//************
// some macros that will help us
//-----------------------------------------------------------------------------
#define commandLCD(data)        (sendLCD(data, 1)) 
#define characterLCD(data)      (sendLCD(data, 0)) 
#define clearLCD()              (commandLCD(1))


void initLCD(void);
void waitForIdleLCD(void);
void processDataLCD();
void sendLCD(unsigned char data, uint8_t isCommand);
void gotoLCD(uint8_t column, uint8_t row);
void printLCDXY(char* string, uint8_t column, uint8_t row);
void printLCD(char* string);
void uploadCustomCharacterAt(uint8_t location, unsigned char *character);

void gotoLCD(uint8_t column, uint8_t row){
	if(row == 0){
	commandLCD(GOTO_LINE1);
	}
	
	if(row == 1){
	commandLCD(GOTO_LINE2);
	}
	
	for(uint8_t i=0;i<column;i++){
	commandLCD(MOVE_CURSOR_RIGHT);
	}
}

void waitForIdleLCD() {
    //This function waits till lcd is BUSY

    uint8_t busy, status = 0x00;

    // make data port of MCU->LCD input mode
    LCD_DATA_DIR = 0b00000000;

    //change LCD mode
    SET_RW_READ_MODE();
    SET_RS_COMMAND();


    do {
        SET_E();
        _delay_us(5);

        status = LCD_DATA_PIN;
    
        processDataLCD();

        busy = status & 0b10000000;

    } while (busy);

    SET_RW_WRITE_MODE(); //write mode
    //Change Port to output

    // make data port of MCU->LCD output mode
    LCD_DATA_DIR = 0b11111111;

}

void initLCD() {
    _delay_ms(50);
    // make sure we will set the direction on the CTRL port for E RS and RW
    LCD_CTRL_DIR |= 1 << LCD_E | 1 << LCD_RS | 1 << LCD_RW;
    // empty the data port at least half of it starting with the configured index
    LCD_DATA_PORT &= ~0xFF;
    // small delay
    _delay_us(5);
    // get first byte part by truncating half of the data shifting it right and pussing it to the data index
    // use either INIT_8BIT_1_LINE_5b7 or INIT_8BIT_2_LINE_5b7 as they have the same upper byte part
    LCD_DATA_PORT |= (INIT_8BIT_1_LINE_5b7 >> 4);
    // small delay to let the LCD set itself
    _delay_us(50);

    // set the lcd lines number
    if (LCD_LINES == 2) {
        commandLCD(INIT_8BIT_2_LINE_5b7);
    } else {
        commandLCD(INIT_8BIT_1_LINE_5b7);
    }
    _delay_us(50);

    // set lcd display and cursor
    if (LCD_DISPLAY == 0) {
        commandLCD(DISPLAY_OFF_CURSOR_OFF);
        _delay_us(5);
        return;
    } else {
        if (LCD_CURSOR == 0) {
            commandLCD(DISPLAY_ON_CURSOR_OFF);
            _delay_us(5);
        } else {
            if (LCD_CURSOR_BLINK == 0) {
                commandLCD(DISPLAY_ON_CURSOR_ON);
                _delay_us(5);
            } else {
                commandLCD(DISPLAY_ON_CURSOR_BLINK);
                _delay_us(5);
            }
        }
    }

    clearLCD();

}

void processDataLCD() {
    // turn light on
    SET_E();
    // make a small delay so the lcd can process the data
    _delay_us(100);
    // turn light off
    CLEAR_E();
}

void sendLCD(unsigned char data, uint8_t isCommand) {
    waitForIdleLCD();

    // check if we have a command or not and set the RS accordingly
    if (isCommand == 1)
        SET_RS_COMMAND();
    else
        SET_RS_DATA();
		
    // delay
    SET_E();
	// send the data to the port
    LCD_DATA_PORT = data;
    CLEAR_E();

    // wait a bit between packages
    _delay_us(1);
}

void printLCDXY(char* string, uint8_t column, uint8_t row){
	gotoLCD(column, row);
	printLCD(string);
}


void printLCD(char* string){
	int stringLength = strlen(string);
	
	for(uint8_t i=0;i<stringLength;i++){
		characterLCD(string[i]);
	}
}

void uploadCustomCharacterAt(uint8_t location, unsigned char *character){
      uint8_t i;
      if(location<8){
          commandLCD(0x40+(location*8));
          for(i=0;i<8;i++)
             characterLCD(character[i]);
     }
}
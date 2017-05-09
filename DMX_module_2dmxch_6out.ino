
/* CVO De Verdieping: Elektronica Software-ontwikkelaar Arduino 1
   <Titel>        Automatisatie Voordoek en Zaallicht in Theaterzaal
   <Beschrijving> Om op een bestaande lichtsturing (DMX) een electrisch
                  voordoek en zaallicht te bedienen wat nu separaat gebeurt.
                  De bedoeling is dat het doek en het zaallicht elk op een
                  apart dmx-kanaal reageren. bij een lage waarde van het
                  communicerende dmx-kanaal gaat het doek dicht of wordt
                  het licht gedimd. bij een hoge waarde op het dmx kanaal
                  gaat het doek open of gaat het licht feller branden.
                  in de zone ertussen wordt de uitgang bevroren (lees doek
                  stopt, lichtstand wordt niet verder gedimd of feller gezet).
                  Toevoeging 1:
                  Het ingestelde DMX-adres wordt opgeslagen in het geheugen van de arduino
                  en het blijft bestaan na spanningsonderbreking.
                  Toevoeging 2:
                  De achtergrondverlichting van het LCD display gaat uit 30 seconden na
                  opstart of na 30 seconden zonder bediening drukknoppen
                  bij het drukken op een van de knoppen gaat de achtergrondverlichting
                  weer aan.
                  Toevoeging 3:
                  Opstartinformatie
                  Toevoeging 4:
                  statusaanduiding van uitgangen op lcd display
                  H voor hoog
                  S voor stop
                  L voor laag.
                  Toevoeging 5:
                  Statusaanduiding van DMX
                  Als dmx niet aanwezig is, dan worden alle uitgangen (lees relais) uitgezet.
                  verder wordt het woordje "SET" DMX vervangen door " NO" DMX
                  De uitgangsstatus wordt in plaats van H, S, L aangeduid door "X"
                  Toevoeging 6:
                  2 extra kanalen (omdat ik toch een 8ch relay board heb en vrije pins)
                  relais 7 en 8. schakelen op dmx kanaal 3 en 4 vanaf adres en schakelen vanaf 50% dmx waarde 
   <used stuff>   1x arduino MEGA
                  1x DMX Shield conceptinetics CTC-DRA-10-R2
                  1x 8ch Relay Board
                  1x Arduino LCD keypad shield
                  1x voeding 5V voor arduino en relay board
                  een 20-tal stekkertjes
                  1 dmx-sturing
                  hersenen en geduld
   <auteur>       Jasper Hendriks
   <datum>        Mei 2017.
   
   This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
// BIBLIOTHEKEN



#include <EEPROM.h>                 // om dmx adres vast te houden na stroomuitval
#include <Conceptinetics.h>         // bibilitheek van DMX 
#include <LiquidCrystal.h>          // bib voor LCD
#define DMX_SLAVE_CHANNELS   4      // het aantal dmx-kanalen dat deze opstelling in beslag neemt



DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS );

// CONSTANTEN

const int DOEKOPEN = 22;              // als deze uitgang hoog is gaat het voordoek open
const int DOEKDICHT = 24;             // als deze uitgang hoog is gaat het voordoek dicht
const int DOEKSTOP = 26;              // als deze uitgang hoog is wordt het voordoek gestopt, ongeacht positie
const int LICHTOPEN = 28;             // als deze uitgang hoog is wordt het zaallicht versterkt
const int LICHTDICHT = 30;            // als deze uitgang hoog is wordt het zaallicht gedimd
const int LICHTSTOP = 32;             // als deze uitgang hoog is wordt het zaallicht bevroren, ongeacht helderheid
const int RELAIS7 = 34;               // reserve uitgang 3de dmxkanaal boven 50%
const int RELAIS8 = 36;               // reserve uitgang 4de dmxkanaal boven 50%

// (GLOBALE) VARIABELEN
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // LCD verbinding met Arduino
int dmxAvailable = 0;                 // om aan te geven of dmx aanwezig is
int BACKLIGHTPIN = 10;                // achtergrondverlichting LCD
int dmxAdres = 001;                   // het beginadres waarop unit is ingesteld

int dmxByte1 = 0;                     // waarde van 1-255 uit het geheugen
int dmxByte2 = 0;                     // waarde van 0 of 255 uit het geheugen
int addr1 = 001;                      // adres van byte 1 in het geheugen
int addr2 = 002;                      // adres van byte 2 in het geheugen
// drukknoppen uitlezen van LCD keypad shield
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

bool                    backlightState = false;
uint32_t                backlightStartTime = 0;
unsigned long now =     millis();
unsigned long           lastFrameReceivedTime;
const unsigned long     dmxTimeoutMillis = 1000UL;
unsigned short          channelsReceived;
// VOORBEREIDING
int read_LCD_buttons()
{
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;
  return btnNONE;
}

void setup()
{
  dmx_slave.onReceiveComplete ( OnFrameReceiveComplete );
  dmxByte1 = EEPROM.read(addr1);
  dmxByte2 = EEPROM.read(addr2);

  if (dmxByte2 == 1)
  {
    dmxAdres = (dmxByte1 + 255);
  }
  else
  {
    dmxAdres = (dmxByte1);
  }

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("VOOR SERVICE");
  lcd.setCursor(0, 1);
  lcd.print(" EN GARANTIE");
  delay (2000);
  lcd.setCursor(0, 0);
  lcd.print("ZIJN WE MET ");
  lcd.setCursor(0, 1);
  lcd.print("    VAKANTIE");
  delay (2000);
  lcd.setCursor(0, 0);
  lcd.print("starlightjasper");
  lcd.setCursor(0, 1);
  lcd.print("@hotmail.com");
  dmx_slave.enable ();
  dmx_slave.setStartAddress (dmxAdres);
  pinMode ( DOEKOPEN, OUTPUT );
  pinMode ( DOEKDICHT, OUTPUT );
  pinMode ( DOEKSTOP, OUTPUT );

  pinMode ( LICHTOPEN, OUTPUT );
  pinMode ( LICHTDICHT, OUTPUT );
  pinMode ( LICHTSTOP, OUTPUT );
  pinMode ( BACKLIGHTPIN, OUTPUT );
  pinMode ( RELAIS7, OUTPUT );
  pinMode ( RELAIS8, OUTPUT );
  backlightState = true;
  backlightStartTime = millis();
  digitalWrite( BACKLIGHTPIN, backlightState );
  delay(3000);
  lcd.setCursor(0, 0);
  lcd.print("DMX SWITCH PACK ");
  lcd.setCursor(0, 1);
  lcd.print("SET DMX         ");
  unsigned long now = millis();


}

// UITVOERING
void loop()
{
  unsigned long now = millis();

  if ( now - lastFrameReceivedTime > dmxTimeoutMillis )
  {
    dmx_slave.getBuffer().clear();
    lcd.setCursor(0, 1);
    lcd.print(" NO ");
    digitalWrite ( LICHTSTOP, LOW );
    digitalWrite ( DOEKSTOP, LOW );
    digitalWrite ( LICHTOPEN, LOW );
    digitalWrite ( DOEKOPEN, LOW );
    digitalWrite ( LICHTDICHT, LOW );
    digitalWrite ( DOEKDICHT, LOW );
    digitalWrite ( RELAIS7, LOW );
    digitalWrite ( RELAIS8, LOW );
    lcd.setCursor(12, 1);
    lcd.print("XXXX");
    dmxAvailable = 0;
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("SET ");
    dmxAvailable = 1;
  }
  dmx_slave.setStartAddress (dmxAdres);
  lcd.setCursor(8, 1);
  lcd.print(dmxAdres);

  lcd.setCursor(8, 1);
  lcd_key = read_LCD_buttons();  // knoppen uitlezen

  switch (lcd_key)               // vervolgacties na uitlezen knoppen
  {
    case btnRIGHT:
      {
        if (dmxAdres > (512 - 50 - (DMX_SLAVE_CHANNELS)))
          dmxAdres = (513 - (DMX_SLAVE_CHANNELS));
        else
          dmxAdres = dmxAdres + 50;

        lcd.print("          ");
        backlightState = true;
        backlightStartTime = millis();
        digitalWrite( BACKLIGHTPIN, backlightState );
        delay(200);
        break;
      }
    case btnLEFT:
      {
        if (dmxAdres < 50)
          dmxAdres = 1;
        else
          dmxAdres = dmxAdres - 50;
        //lcd.print("          ");
        backlightState = true;
        backlightStartTime = millis();
        digitalWrite( BACKLIGHTPIN, backlightState );
        delay(200);
        break;
      }
    case btnUP:
      {
        if (dmxAdres > (512 - (DMX_SLAVE_CHANNELS)))
        {
          dmxAdres = 1;
        }
        else
        {
          dmxAdres = dmxAdres + 1;
        }
        lcd.print("          ");
        backlightState = true;
        backlightStartTime = millis();
        digitalWrite( BACKLIGHTPIN, backlightState );
        delay(200);
        break;

      }
    case btnDOWN:
      {
        if (dmxAdres == 1 )
        {
          dmxAdres = (513 - (DMX_SLAVE_CHANNELS));
        }
        else
        {
          dmxAdres = dmxAdres - 1;
        }
        lcd.print("          ");
        backlightState = true;
        backlightStartTime = millis();
        digitalWrite( BACKLIGHTPIN, backlightState );
        delay(200);
        break;

      }
    case btnSELECT:
      {

        lcd.print(dmxAdres);


        if (dmxAdres > 255)
        {
          dmxByte2 = 1;
          dmxByte1 = (dmxAdres - 255);
        }
        else
        {
          dmxByte1 = (dmxAdres);
          dmxByte2 = 0;

        }
        backlightState = true;
        backlightStartTime = millis();
        digitalWrite( BACKLIGHTPIN, backlightState );
        EEPROM.write(addr2, dmxByte2);
        EEPROM.write(addr1, dmxByte1);

        delay(200);
        break;

      }

    case btnNONE:
      {

        // lcd.print("NONE  ");
        break;
      }
  }

  if (dmxAvailable == 1)                        // als er geen dmx is moeten we geen uitgangen aansturen, dus slaan we een gedeelte over
  {
    if ( dmx_slave.getChannelValue (1) > 240 )
    {
      digitalWrite ( DOEKOPEN, HIGH );
      lcd.setCursor(12, 1);
      lcd.print("H");
    }
    else
    {
      digitalWrite ( DOEKOPEN, LOW );
    }
    if ( dmx_slave.getChannelValue (1) < 10 )
    {
      digitalWrite ( DOEKDICHT, HIGH );
      lcd.setCursor(12, 1);
      lcd.print("L");
    }
    else
    {
      digitalWrite ( DOEKDICHT, LOW );
    }

    if (( dmx_slave.getChannelValue (1) > 9 ) and ( dmx_slave.getChannelValue (1) < 241 ))
    {
      digitalWrite ( DOEKSTOP, HIGH );
      lcd.setCursor(12, 1);
      lcd.print("S");
    }
    else
    {
      digitalWrite ( DOEKSTOP, LOW );
    }



    if ( dmx_slave.getChannelValue (2) > 240 )
    {
      digitalWrite ( LICHTOPEN, HIGH );
      lcd.setCursor(13, 1);
      lcd.print("H");
    }
    else
    {
      digitalWrite ( LICHTOPEN, LOW );
    }

    if ( dmx_slave.getChannelValue (2) < 10 )
    {
      digitalWrite ( LICHTDICHT, HIGH );
      lcd.setCursor(13, 1);
      lcd.print("L");
    }
    else
    {
      digitalWrite ( LICHTDICHT, LOW );
    }

    if (( dmx_slave.getChannelValue (2) > 9 ) and ( dmx_slave.getChannelValue (2) < 241 ))
    {
      digitalWrite ( LICHTSTOP, HIGH );
      lcd.setCursor(13, 1);
      lcd.print("S");
    }
    else
    {
      digitalWrite ( LICHTSTOP, LOW );
    }

    if ( dmx_slave.getChannelValue (3) > 127 )
    {
      digitalWrite ( RELAIS7, HIGH );
      lcd.setCursor(14, 1);
      lcd.print("H");
    }
    else
    {
      digitalWrite ( RELAIS7, LOW );
      lcd.setCursor(14, 1);
      lcd.print("L");
    }

    if ( dmx_slave.getChannelValue (3) > 127 )
    {
      digitalWrite ( RELAIS8, HIGH );
      lcd.setCursor(15, 1);
      lcd.print("H");
    }
    else
    {
      digitalWrite ( RELAIS8, LOW );
      lcd.setCursor(15, 1);
      lcd.print("L");
    }

  } // if dmx available == 1

  if ( backlightState == true && millis() - backlightStartTime >= 30000 )
  {
    backlightState = false;
    digitalWrite( BACKLIGHTPIN, backlightState );
  }


}

void OnFrameReceiveComplete (unsigned short channelsReceived)
{
  if ( channelsReceived == DMX_SLAVE_CHANNELS)
  {
    // All slave channels have been received
  }
  else
  {
    // We have received a frame but not all channels we where
    // waiting for, master might have transmitted less
    // channels
  }

  // Update receive time to determine signal timeout
  lastFrameReceivedTime = millis ();
}


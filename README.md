# dmx-switchpack
a 4 channel dmx switchpack with a 8 ch relay board using arduino
DUTCH AND ENGLISH LANGUAGE MIXED BELOW

<Titel>        Automatisatie Voordoek en Zaallicht in Theaterzaal
<title>       Automation project of curtain and hall light to dmx in a theater
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
   <Description> using 4 channels of dmx to control 8 relays
                  first dmx channel is controlling 3 relays. When value is above 240, relay 1 will turn on
                  when value is far low... , relay 2 will turn on, when the dmx value is in between, relay 3 will be on. 
                  Only one of the 3 relays will be on.
                  the second dmx channel is corresponding to relay 4-5-6
                  the 3rd and 4th channel corresponds to relay 7 and 8. here when the dmx value is above 50%, the relay will turn on.
                  Addendum 1:
                  the dmx adres is settable and will be stored in the memory of the arduino by pushing select button
                  addendum 2:
                  the background light of the LCD will turn off after 30seconds not giving any input.
                  addendum 3:
                  startup information
                  addendum 4:
                  status indication for each relay dmx channel
                  H for High level
                  L for Low Level
                  S for Stop (between high and low for channel 1 and 2 only)
                  addendum 5:
                  turn off all relays when no dmx is there and indicate this on the LCD display with "no dmx"
                 
   <used stuff>   1x arduino MEGA (anywhere in this world)
                  1x DMX Shield conceptinetics CTC-DRA-10-R2 (banggood)
                  1x 8ch Relay Board (banggood)
                  1x Arduino LCD keypad shield (banggood)
                  1x powersupply 5V (old cpu)
                  connection wires (anywhere in this world)
                  1 dmx-controller (could be any)
                  hersenen en geduld (brains and patience)
   <auteur>       Jasper Hendriks
   <datum>        Mei 2017. May 2017
   
   additional information
   DMX shield on top of arduino mega
   then connector cable towards LCD keypad
   relay board connected with connector cables on the arduino mega

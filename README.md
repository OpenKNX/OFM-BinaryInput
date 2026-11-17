# OFM-BinaryInput

Dies ist eine Library zur Integration von Binäreingängen. Das Featureset ist auf das wesentliche beschränkt, da eine Verwendung als Taster über OFM-VirtualButton abgebildet wird.
  
Features
- Senden eines DPT1 bei "Offen" oder "Geschlossen"
- Sperre
- Entprellung
- Zyklisches senden 

## Laufzeit-Pinzuordnung

Mit `OPENKNX_BI_DYNAMIC_PINS` kann die Applikation vor `setup()` über
`configureChannel(channel, gpio, mode, activeHigh)` jeden Kanal zuordnen.
GPIO 255 deaktiviert den Kanal. `mode` ist INPUT, INPUT_PULLUP oder INPUT_PULLDOWN.
Nach setup wird eine Änderung abgelehnt. Die Applikation prüft zuvor Hardwareauswahl,
elektrische Eignung, reservierte GPIOs und Überschneidungen mit anderen Nutzern.
Ohne das Define bleiben die statische Pinliste und OPENKNX_BI_ONLEVEL erhalten.

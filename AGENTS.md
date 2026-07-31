# AGENTS für OFM-BinaryInput

## Ziel

Dieses Modul liest einfache Binäreingänge (z. B. Fenster-/Türkontakte)
über GPIO-Pins ein und sendet den Zustand als DPT1 auf den KNX-Bus. Das
Featureset ist bewusst minimal — Taster-Funktionalität (kurz/lang, Klick
etc.) wird nicht hier, sondern über OFM-VirtualButton abgebildet.

## Basis: OGM-Common

Dieses OFM baut auf `../OGM-Common/AGENTS.md` auf (Modulsystem, LED-/
Flash-/Zeit-API, Logging, Embedded-Rahmenbedingungen, Code-Konventionen)
— dort beschriebene Regeln und vorhandene Claude-Skills/Agents (z. B.
das Kanalauswahl-Muster) gelten auch hier.

## Prefix und Namenskonventionen

- **Modulprefix**: `BI`
- **C++-Klassen**: `GpioBinaryInputModule` (Modul), `BinaryInputChannel` (Kanal)
- **ETS-Anzeigename des Moduls**: `Binäreingänge`, **`name()`**: `BasicBinaryInput`

## Aktivierung und Kanalanzahl

Das Modul existiert nur, wenn das einbindende Projekt (OAM) in
`hardware.h` `OPENKNX_BI_GPIO_PINS` und `OPENKNX_BI_GPIO_COUNT` (> 0)
definiert und `BI_ChannelCount` (> 0) aus `knxprod.h` kommt — sonst wird
`GpioBinaryInputModule.h/.cpp` komplett wegcompiliert
(`#if defined(OPENKNX_BI_GPIO_PINS) && OPENKNX_BI_GPIO_COUNT > 0 && BI_ChannelCount > 0`).
Tatsächlich verwendet wird `MIN(BI_ChannelCount, OPENKNX_BI_GPIO_COUNT)`
Kanäle — die kleinere der beiden Zahlen.

Jeder Kanal ist `ParamBI_ChannelActive`-gesteuert; inaktive Kanäle werden
zwar angelegt (kein Lazy-Allocation-Pattern), aber `setup()`/`loop()`/
`processHardwareInputs()` überspringen sie über `isActive()`.

## Hardware-Anbindung

Die physischen Pins liegen in `_gpioPins[OPENKNX_BI_GPIO_COUNT]`
(aus `OPENKNX_BI_GPIO_PINS`) und werden je nach `OPENKNX_BI_ONLEVEL`
(Default `LOW`) als `INPUT_PULLUP` oder `INPUT_PULLDOWN` konfiguriert.
Optional gibt es einen Pulse-Pin (`OPENKNX_BI_PULSE`, aktiv wenn `!= -1`),
der vor jedem Abfragezyklus kurz auf HIGH gesetzt wird
(`OPENKNX_BI_PULSE_WAIT_TIME`) und danach wieder LOW — z. B. für
Eingänge, die eine kurze Versorgung vor dem Lesen brauchen. Zwischen zwei
Abfragezyklen liegt `OPENKNX_BI_PULSE_PAUSE_TIME`.

`GpioBinaryInputModule::loop()` liest in jedem Zyklus alle aktiven Kanäle
über `openknx.gpio.digitalRead()` und reicht den Rohwert per
`setHardwareState()` an den jeweiligen Kanal weiter.

## Kanal-Logik (`BinaryInputChannel`)

- **Entprellung** (`debounce()`): Nur aktiv, wenn `ParamBI_ChannelDebouncing > 0`.
  Ein neuer Hardware-Zustand muss für die konfigurierte Zeit stabil
  bleiben, bevor er übernommen wird.
- **Sendelogik** (`sendState()`): `ParamBI_ChannelOpen`/`ParamBI_ChannelClose`
  legen pro Flanke unabhängig fest, ob und mit welchem Wert (0/1) gesendet
  wird — beide Parameter können also unterschiedliche DPT1-Werte für
  Öffnen/Schließen konfigurieren, oder das Senden für eine Richtung ganz
  unterdrücken.
- **Zyklisches Senden** (`processPeriodicSend()`): Nur aktiv, wenn
  `ParamBI_ChannelPeriodicTimeMS > 0`; der erste Zyklus nach dem Start
  wird übersprungen (nur zum Setzen des Referenzzeitpunkts).
- **Status-LEDs**: Bei jedem gesendeten Zustand wird sowohl die
  kanalbezogene (`OPENKNX_LEDFUNC_BI_CHANNEL_ACT`/`_STATUS`,
  Basis-IDs 601/701 + Kanalindex) als auch die zentrale Aktivitäts-LED
  (`OPENKNX_LEDFUNC_BI_ACT`, ID 600) angesteuert.

## Regeln für Weiterentwicklung

1. Taster-artige Auswertung (kurz/lang, Doppelklick) gehört nicht in
   dieses Modul — das ist Aufgabe von OFM-VirtualButton.
2. `GpioBinaryInputModule` liest nur Hardware und verwaltet das
   Channel-Array — keine Open/Close-Logik dort.
3. Neue Sende-Bedingungen gehören in `BinaryInputChannel::sendState()`,
   nicht in `processInput()`.

## Dokumentation und Hilfe

- Dokumentation liegt in `doc/Applikationsbeschreibung-Binaereingang.md`
- Jede sichtbare `ParameterRefRef` bekommt einen `HelpContext` mit Prefix
  `BI-` (Ausnahme: `Empty` für Parameter ohne eigene Hilfeseite)
- Baggages werden über VS Code Task "OpenKNXproducer Documentation"
  erzeugt (`.vscode/tasks.json`)
- Deutsche Texte mit echten Umlauten (ä, ö, ü, ß) schreiben

## Referenzen

- [README.md](README.md) — Kurzüberblick über die Features
- [doc/Applikationsbeschreibung-Binaereingang.md](doc/Applikationsbeschreibung-Binaereingang.md) — vollständige Parameterbeschreibung

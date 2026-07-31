# Changes

## 0.4.0

- Status-LEDs für Modul und Kanäle ergänzt.
- Kanalauswahl in der ETS überarbeitet (Aktiv/Inaktiv).

## 0.3.1

- Ein GPIO-Pin wird nur noch als Eingang konfiguriert, wenn der
  zugehörige Kanal auch tatsächlich aktiv ist.
- Überlappenden Parameter bei der Zykluszeit behoben (konnte die
  Konfiguration verfälschen) — dadurch war ein vollständiges
  Herunterladen der Applikation nötig.

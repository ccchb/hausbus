<meta charset="utf-8">

Layer 1 - Framing und Bitsicherung
==================================

Dieser Layer verarbeitet den Bytestrom aus der Seriellen.

Die Nachrichten sind wie folgt aufgebaut:

    ├─ n Bytes ─┼─ 1 Byte ─┼─ 1 Byte ─┤
    ┏━━━━━━━━━━━┳━━━━━━━━━━┳━━━━━━━━━━┓
    ┃  payload  ┃   0xff   ┃   crc    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛

* **payload** ist der Payload der Nachricht. Dabei wird jedes Byte `0xff` durch die Bytefolge
	`0xff 0xff` ersetzt.
* **crc** ist eine CRC über die Bytefolge des Payloads. Die CRC wird vor dem Escaping der
	`0xff`-Bytes berechnet. Falls die CRC der Wert `0xff` ist, wird in diesem Feld stattdessen der
	Wert `0x00` abgelegt. (Um das Ende der Nachricht von einem Byte `0xff` im Payload unterscheiden
	zu können)


Vorgehensweise beim Enkodieren
------------------------------

Der Pseudocode verwendet die folgenden Resourcen:

* **Input**: Eingabebytestream. Enthält die Ausgabe des nächsthöheren Protokolllayers.
* **Output**: Ausgabebytestream. Wird z.B. per serieller Übertragung versendet.
* **EOM**: End-Of-Message-Flag. Signalisiert, dass die aktuelle Nachricht zu Ende ist.

<!-- stupid markdown -->

	while True:
		crc.init()
		while not EOM.get():
			b = Input.read_byte()
			crc.update(b)
			if b == 0xff:
				Output.write(0xff)
				Output.write(0xff)
			else:
				Output.write(b)
		crc.finish()
		Output.write(0xff)
		if crc.value == 0xff:
			Output.write(0x00)
		else:
			Output.write(crc.value)


Vorgehensweise beim Dekodieren
------------------------------

Der Pseudocode verwendet die folgenden Resourcen:

* **Input**: Eingabebytestream. Wird z.B. durch seriell empfangene Übertragungen gefüllt.
* **Output**: Ausgabebytestream. Wird weitergereicht an nächsthöheren Protokolllayer.
* **EOM**: End-Of-Message-Flag. Signalisiert, dass die aktuelle Nachricht zu Ende ist.
* **CAN**: Cancel-Flag. Signalisiert, dass die aktuelle Nachricht fehlerhaft übertragen wurde und
	nicht verarbeitet werden darf.

<!-- stupid markdown -->

	while True:
		crc.init()
		while True:
			b = Input.read_byte()
			if b == 0xff:
				b = Input.read_byte()
				if b != 0xff:
					crc.finish()
					if (crc.value == 0xff and b == 0x00) or crc.value == b:
						EOM.set()
					else:
						CAN.set()
					break
			crc.update(b)
			Output.write(b)


Details der CRC-Berechnung
--------------------------

Verwendet wird die 8-Bit-CRC von Dallas/MAXIM. Die genaue Spezifikation der CRC in der "Rocksoft™
Model CRC Algorithm"-Notation ist wie folgt:

	width=8 poly=0x31 init=0x00 refin=true refout=true xorout=0x00 check=0xa1 name="CRC-8/MAXIM"

Siehe auch:

* [Eintrag im Reveng CRC catalogue](http://reveng.sourceforge.net/crc-catalogue/all.htm#crc.cat.crc-8-maxim)
* [CRC painless guide: Rocksoft Model](http://www.repairfaq.org/filipg/LINK/F_crc_v34.html#CRCV_005)
* [MAXIMs Application Note zur CRC-8/MAXIM](http://www.maximintegrated.com/en/app-notes/index.mvp/id/27)

Die folgenden Testvektoren ergeben alle die CRC 0:

	242BC5FB00000040
	102BC5FB000000A0
	212BC5FB00000089
	142BC5FB00000054
	14B3D8FB000000D4
	232BC5FB000000F3
	23B3D8FB00000073
	092BC5FB00000097
	09B3D8FB00000017
	0B2BC5FB000000ED
	0BB3D8FB0000006D
	0F2BC5FB00000019
	0FB3D8FB00000099
	0A2BC5FB000000D0
	0C2BC5FB0000005E


Beispiele
---------

	Payload                  Nachricht                 Kommentar

	00 01 02 03 04           00 01 02 03 04 FF F4      Keine Spezialfälle
	42 23 FF                 42 23 FF FF FF 16         Escaping
	30 31 F2                 30 31 F2 FF 00            CRC == 0xff
	
	Ungültige Nachrichten:
	00 01 02 03 14 FF F4
	00 80 FF 00


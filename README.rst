RRUI
====

Presentation
------------

RRUI is a software based on QT5/QML which aim is primary to display data from embbeded system. It is based on a really simple fixed-length protocol.

Protocol
--------

Transport layer
```````````````

This protocol can be used with different transport layers.
It is a *very* simple protocol that relies on the underlying layer integrity.

Messages are grouped into packets. On IP layer, a packet is the network packet.
On serial link, a packet is assumed for example when a 100ms pause is detected ; or with a checksumed frame with delimiters and length if needed.
The packet entity helps to synchronize reception.

Messages content
````````````````

All messages are 64 bits long (8 bytes).
There can be several messages in each packet.

Byte 0 carries the type of data

 - 0x00: Binary/boolean
 - 0x01: int8_t
 - 0x02: uint8_t
 - 0x03: int16_t
 - 0x04: uint16_t
 - 0x05: int32_t
 - 0x06: uint32_t
 - 0x07: float (32 bits)

Byte 1 carries wanted action

 - 0x00: Report a value (report)
	This one is used to report a value in its argument
 - 0x01: Ask for a value (get)
	Ask for a on-shot value of a specific parameter (useful for sporadic information)
 - 0x02: Set a value temporary (set)
	Set a value temporary, that is the new value is read by the application until a new value is updated by the application itself or another action. It also cancels the forced status
 - 0x03: Force value (force)
	Force a value, that is the new value is read by the application whatever the application itself tries to update. A release action is needed to return to the normal operation mode.
 - 0x04: Release a value (release)
	Cancels the force action. The forced value remains the one the application reads until a new value is updated by the application itself or another action.
 - 0x05: Reload reset value (reload)
	Reload the value the variable had at boot or last one saved, ie the saved value.
 - 0x06: Save value (save)
	Save value so that it will be loaded by default at next boot. It affects the reload command and cancels the forced status.
 - 0x07: Export value periodically (export)
	Export value periodically. The period is set by the argument of the message read as an integer which sets the number of ignored loops, ie a pediod of rougly (arg+1)*10ms.
 - 0x08: Arrêter l'export (unexport)
	Stop export of a value

Bytes 2 and 3 carry parameter number read as an integer in little-endian endianess.

Bytes 4 to 7 carry argument (often parameter value) read in little-endian endianess.
If it is shorter than 32 bits, only lower significant bits are relevant, other are ignored.
The entire field can be meaningless depending on the action, eg when a get action is used.

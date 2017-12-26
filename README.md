LPC810/INA219B Power Monitor
============================

This is a simple DC power monitor based around the NXP LPC810 and the INA219B.

Circuit description
-------------------

The pins on the LPC810 are allocated as follows:

1. `nRESET`: Resets the MCU, used for programming.  2k2 pull-up.
2. `TX`: UART Transmit
3. `SCL`: to INA219B, 2k2 pull-up.
4. `SDA`: to INA219B, 2k2 pull-up.
5. `nISP`: Puts MCU into ISP mode for flashing.  2k2 pull-up.
6. `3V`: MCU 3V supply.
7. `0V`: MCU 0V reference.
8. `RX`: UART Receive

The board has 100nF capacitors across the 0V and 3V pins of both the LPC810 and
the INA219B.  Also connected across the power rails is a 330µF electrolytic.

The signals `nRESET`, `TX`, `nISP`, `3V`, `0V` and `RX` are exposed on a header.

The INA219B has its address pins tied to 0V, which means it should appear at I²C
address 0x40 (read 0x81, write 0x80).

Protocol
--------

This is not yet figured out, but Modbus/RTU is looking tempting as it only needs
a half-duplex serial link, and I've implemented a Modbus client already on ARM
at work, so I'm familiar with how it works over the wire.

A preliminary register map (addresses numbered from 1 as per Modbus spec, all
not-listed addresses read as zero):

* Input registers
  * 3/0001 `int16_t` Voltage
  * 3/0002 `int16_t` Current
  * 3/0003 `int16_t` Power
  * 3/0005…0006 `int32_t` Energy (big-endian format)

* Holding registers
  * 4/0001 `uint16_t` INA219B configuration
  * 4/0002 `uint16_t` INA219B calibration
  * 4/0003 `int16_t` Voltage pre-offset
  * 4/0004 `int16_t` Voltage numerator
  * 4/0005 `int16_t` Voltage denominator
  * 4/0006 `int16_t` Voltage post-offset
  * 4/0007 `int16_t` Current pre-offset
  * 4/0008 `int16_t` Current numerator
  * 4/0009 `int16_t` Current denominator
  * 4/0010 `int16_t` Current post-offset
  * 4/0011 `int16_t` Power numerator
  * 4/0012 `int16_t` Power denominator
  * 4/0013 `int16_t` Energy numerator
  * 4/0014 `int16_t` Energy denominator
  * 4/0015 `uint16_t` Energy sampling interval in msec.  (0-65.535s)
  * 4/0016 `uint16_t` Modbus ID

LICENSE
-------

Excluding code in the `third_party` directory; all code in this repository is
licensed under the GPLv2.  See [COPYING](COPYING) for details.

This code uses the ROM functions in the LPC series MCUs licensed by NXP.  NXP
licenses this code *only* for use on its MCUs and ships that code in mask-ROM
on the dies of the LPC81xM MCUs, the code is not present in binaries produced
from this code.

To use binaries built from this code on non-NXP MCUs, you will need to provide
compatible implementations of these routines at the same addresses as the NXP
mask-ROM.

I suggest that patches for the source to support an alternative library will be
more productive, and such patches will be considered for inclusion so long as
they don't break existing functionality.

# What is CMCBL

CMCBL (Computotronics MakroControler BootLoader) is a minimal bootloader to read and write the ROM of the CMC32A128 single board computer with the companion application CMCUpload.

## What does it do

CMCBL is a simple semi human readable command shell that can take 16kB chunks of data and write it to ROM. It can overwrite itself to allow self updating, tho this can be dangerous and can brick the CMC.

## How to use it

To use CMCBL compile it using the Makefile and write the resulting binary file to the ROM chip with a chipprogrammer of your choice.

After CMCBL is installed the CMCUpload tool can be used to Upload a binary file to the ROM or Download the ROM into a file.

If you wish you can also invoke CMCBL without CMCUpload by opening serial terminal with the parameters:
* 115200 baudrate
* no parity
* RTS/CTS flowcontroll

CMCBL will react to these commands:
| Command | Paramters | Action | Result |
|---------|-----------|--------|--------|
|'r'      | Hexchar   | Read ROM page {Hexchar} into internal buffer | '.' == OK |
|'w'      | Hexchar   | Write the internal buffer to ROM page {Hexchar} | '.' == OK <br /> '!' == ERROR |
|'g'      | None      | Outputs internal buffer to Serial | 16kB data |
|'p'      | 16kB data | Loads internal buffer from Serial | '.' == OK |
|'c'      | Hexchar   | Compares the ROM page {Hexchar} to the internal buffer | '.' == OK <br/> '!' == ERROR |
|'s'      | None      | Calculated the checksum of the internal buffer | 1 byte checksum |
|'?'      | None      | Output version Information | {Version string}'.' |
|'b'      | None      | Executes ROM from address 0x4000 | None |
|'t'      | None      | Communication test | allways '.' |

## Compiling

#### CMCBL
To compile CMCBL you need these things:
* make
* vasm (vasmz80_oldstyle specifically)

Run `make all`

#### CMCUpload
**Right now CMCUpload will only compile and run under linux!**
To compile CMCUpload you need these things:
* make
* gcc

Run `cd CMCUpload; make all`
To install CMCUpload system wide (recomended):
Run `cd CMCUpload; sudo make install`

## License

This project is linsensed under GPL3.
This means:
* I dont take any Liability or Warranty for my code
* You can share and modify my code for private or comercial use (as if someone would use this commercially...)
* If you fork this project it has to stay under GPL3 and the original source has to be disclosed

Have fun
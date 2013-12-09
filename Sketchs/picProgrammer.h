/*
    Arduimmer. Using Arduino as a PIC18F2XXX and PIC18F4XXX programmer
    Copyright (C) 2013 Benito Palacios (benito356@gmail.com)
  
    This file is part of Arduimmer.

    Arduimmer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Arduimmer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Arduimmer.  If not, see <http://www.gnu.org/licenses/>. 
*/
#define pinPGM 5
#define pinPGC 6
#define pinPGD 7
#define pinVPP 2

#define InstCore                 0x0
#define InstShiftOut             0x2
#define InstTblRead              0x8
#define InstTblReadPostIncr      0x9
#define InstTblReadPostDecr      0xA
#define InstTblReadPreIncr       0xB
#define InstTblWrite             0xC
#define InstTblWritePostIncr     0xD
#define InstTblWritePostIncrProg 0xE
#define InstTblWriteProg         0xF


@echo off

REM scan all inputs, all inputs must be toggled once - testing dig[n] registers
REM once all inputs have level, then done.
REM if keypress then indicate errors and quit.
idi din test value

REM scan all inputs rising edge detection - testing edge detection
REM once all inputs have edge-change, then done.
REM if keypress then indicate errors and quit.
idi din test re

REM scan all inputs falling edge detection - testing edge detection
REM once all inputs have edge-change, then done.
REM if keypress then indicate errors and quit.
idi din test fe

REM scan all inputs rising edge detection - testing edge detection
REM once all inputs have edge-change, and irq detected then done.
idi loop din all test rising_edge interrupt




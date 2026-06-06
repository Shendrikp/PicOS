@echo ------------- Компиляция программы -------------
@echo off
rem   Data: 5/01/2024   Code_Page: CP866

if exist target\target.bin (del target\target.bin)

call :comp System\startup.c
call :comp main.c
rem mafunc.c maginit.c - эти файлы идут как вставка текста

arm-none-eabi-gcc -Xlinker -Map=target.map -Wl,--gc-sections ^
-mcpu=cortex-m3 -mthumb -static -T System/stm32f103c6t6.lsf ^
startup.o main.o -o main.elf

arm-none-eabi-objcopy -Obinary main.elf target/target.bin

arm-none-eabi-size main.elf
arm-none-eabi-objdump -h -S -z main.elf > main.lss

rem pause

del *.o *.elf 
del *.map *.lss
@echo on
if exist target\target.bin goto PROG
@echo ----- Ошибки при компиляции -----
pause
goto EXIT

rem pause
timeout 3

:PROG
@echo ------------------------------------------------ 

if exist "C:/Program Files (x86)/STMicroelectronics/STM32 ST-Link Utility/ST-Link Utility/ST-LINK_CLI.exe" "C:/Program Files (x86)/STMicroelectronics/STM32 ST-Link Utility/ST-Link Utility/ST-LINK_CLI.exe" -c SWD -ME -P "target/target.bin" 0x08000000 -v -Rst

if exist "C:/Program Files/STMicroelectronics/STM32 ST-Link Utility/ST-Link Utility/ST-LINK_CLI.exe" "C:/Program Files/STMicroelectronics/STM32 ST-Link Utility/ST-Link Utility/ST-LINK_CLI.exe" -c SWD -ME -P "target/target.bin" 0x08000000 -v -Rst

pause
goto EXIT

REM ===== Подпрограммы =====
:comp
arm-none-eabi-gcc -ISystem ^
-Os -Wall -c -fno-builtin -ffunction-sections -fdata-sections ^
-msoft-float -fno-hosted -mtune=cortex-m3 -mcpu=cortex-m3 -mthumb -std=c99 ^
-gdwarf-2 -Wdouble-promotion  -fsingle-precision-constant %1
exit /b

:EXIT

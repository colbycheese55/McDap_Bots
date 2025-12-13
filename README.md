# McDap Bots Pedagogical Robot

This project was created as a capstone project for the University of Virginia Department of Electrical and Computer Engineering. Our goal was to create a small robot which could be programmed by students, for use as a teaching aide in an embedded systems course. This repository contains the software component of this, both drivers and application code.

**Authors**
- Colby Wise
- Pratik Pandit
- Austen Yun 
- David Merino
- Marc Rosenthal

**Advisors**: Todd DeLong and Adam Barnes 

**Features at a glance**
- 2 motors (PWM controlled), OLED screen (I2C), reflectance sensor (I2C), bump switches (GPIO), Bluetooth (UART), and infrared sensors (ADC)
- battery charging port and battery voltage display (press button to enable)
- support for TI and STM microcontrollers 


## What is contained here

**Low Level Drivers**
These files are hardware-specific and interact directly with the physical hardware. TI drivers are in `LowLevelDrivers/src` and STM drivers are in `LowLevelDrivers/STM_src`. 

**High Level Drivers**
These files represent a specific peripheral (ex. a motor) and are hardware-agnostic. They use hardware-specific low-level drivers to control the hardware

**Application**
These files are different applications. The exception is `Application/ApplicationSwitcher.c`, which is used to switch between applications when the user button is pressed. 


## How to use this code

You can copy and paste code from this repo as needed. You can also directory create a project from this repo by...

**TI**
*Intended for a LP-MSPM0G3507*
1. Import this repo as a project in [TI Code Composer Studio](https://www.ti.com/tool/CCSTUDIO) (CCS)
2. Disable `LowLevelDrivers/STM_src/` from compiling by right clicking on the folder in CCS and selecting `Exclude from build`
3. Build the project and flash it

**STM**
*Intended for a Nucleo-G071RB*
1. Create a new project in [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)
2. Use the STM transfer script to copy files over from this repo into the STM project
    - a. Run `python3 stm_transfer.py {path}` where path is the path of the STM32CubeIDE project
    - b. Example: `python3 stm_transfer.py /Users/user/STM32CubeIDE/workspace_1.19.0/McDapBots_STM2`
    - c. Note that on Windows, Python is commonly installed as `python` not `python3`
3. Open the IOC file, and save it to generate hardware initialization code
4. `main.c` is the only essential file that was not copied over. Make sure to call `hardware_init()` and `run_application_switcher()` (blocking) to setup hardware and run our applications  
5. Add `Application/inc`, `HighLevelDrivers/inc`, and `LowLevelDriver/inc` to the include path
    - In CubeIDE: Project >> Properties >> C/C++ General >> Paths and Symbols >> Includes 
6. Add `Application/`, `HighLevelDrivers/`, and `LowLevelDriver/` to the source path
    - In CubeIDE: Project >> Properties >> C/C++ General >> Paths and Symbols >> Source Location 

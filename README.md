# mx3000control

Unofficial tool to configure the Perixx MX-3000 mouse on Linux (the official driver only supports Windows).

* Uses [HIDAPI](https://github.com/signal11/hidapi) to send HID feature report commands obtained by analyzing the Windows driver.
* Supported features:
	* Setting DPI and X/Y sensivity
	* Setting RGB LED color, light intensity and pulsation effect
	* Configuring the mouse buttons by assigning them mouse functions (like left/right click, back/forward, DPI up/down, etc), single keyboard keys (you need to know the scancode; a list can be found, for example,
[here](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2)) or macros (same features available in the original Windows software; a macro can send keyboard key up/down events and wait delays)

### Supported devices
As the name implies, this program is designed to be used with the Perixx MX-3000 mouse. Other mice with the same USB Vendor ID (0x04D9) and Product ID (0xA067) exist; I guess they use the same microcontroller and firmware since
they have the same VID:PID pair, but I can't guarantee compatibility. You can test and let me know if you want. **However**, please always remember (even if using MX-3000) that these devices have an on-board non-volatile memory so
there might be some chances to brick them (never happened during my tests with the MX-3000, though; when sending malformed commands, a power cycle always fixed everything). I take no responsibility for any damage you might cause
to your devices.

### Development notes
If you are interested in the process behind the development of this program, you may find [DEVLOG.md](https://github.com/gianluca-nitti/mx3000control/blob/master/DEVLOG.md) interesting.

### Building
* You need a C compiler, `pkg-config` and the HIDAPI library (should be available from your distribution package manager).
* Just run `make` to build; you can override the compiler by setting the `CC` environment variable (e.g. `CC=clang make` or `CC=gcc make`
* The output is a single binary: `./mx3000control`

### Notice
By default, a regular user won't have access to the device. So you need to either run the program with `sudo`, or configure udev to grant the proper permissions to your user account for the USB device with Vendor ID 0x04D9 and Product ID 0xA067).

### Usage examples
* Set RGB LED color to orange: `./mx3000control setledcolor 255 127 0`
* Set led pulsation effect speed to slow: `./mx3000control setledpulsation slow` (supported values: steady, slow, middle, fast, or numbers 1 through 4)
* Set sensor DPIs to 8200 (the maximum value) `./mx3000control setdpi 8200` (supported values: 600, 1200, 2000, 3000, 4800, 6400, 8200) or `./mx3000control setdpi 7` (min: 1, max: 7)
* Remap buttons: `mx3000control setbuttons click menu middle-button forward backward dpi-up dpi-down led-color-switch` (example configuration; see the in-program help for all the supported possibilities)
* Remap buttons, and assign a macro to one of them: `mx3000control setbuttons click menu middle-button volume-up volume-down macro-repeat-2-down-0x04-delay-500ms-up-0x04 key-0x05 media-play-pause` (again, this is just an example);
keyboard keys are identified by their [scancode](https://en.wikipedia.org/wiki/Scancode) (lists [here](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2) and [here](https://source.android.com/devices/input/keyboard-devices))
* Run `./mx3000control help` for more information.

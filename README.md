# mx3000control

* Attempt at building an unofficial tool to configure the Perixx MX-3000 mouse on Linux (the official driver only supports Windows) and possibly other mice based on similar Holtek chips/firmwares.
* Uses [HIDAPI](https://github.com/signal11/hidapi) to send HID feature report commands obtained by analyzing the Windows driver.
* Currently supported features are:
	* Setting DPI and X/Y sensivity
	* Setting RGB LED color, light intensity and pulsation effect
	* Configuring the mouse buttons by assigning them mouse functions (like left/right click, back/forward, DPI up/down, etc), single keyboard keys (you need to know the scancode; a list can be found, for example, [here](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2)) or macros (same features available in the original Windows software; a macro can send keyboard key up/down events and wait delays)

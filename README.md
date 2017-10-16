# mx3000control

* Attempt at building an unofficial tool to configure the Perixx MX-3000 mouse on Linux (the official driver only supports Windows) and possibly other mice based on similar Holtek chips/firmwares.
* Uses [HIDAPI](https://github.com/signal11/hidapi) to send HID feature report commands obtained by analyzing the Windows driver.
* Just an experiment at the moment (only allows to to set the RGB led's color and only works if running as root).

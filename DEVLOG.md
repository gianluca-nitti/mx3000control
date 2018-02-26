## Development of a Linux configuration software for a gaming mouse

The Perixx MX-3000 is a computer mouse with some features that can be configured through a Windows sofware which comes with a CD in the mouse's box
(alternatively, it can also be [downloaded from the Perixx website](http://perixx.eu/en/service/MX-3000-V1_2.zip)).
This software can be used to set the DPIs of the sensor, change the color and blinking effect of the RGB LED on the mouse,
and to configure the buttons (the mouse has a total of 8 buttons, and each of them can be assigned a mouse function like right click/left click,
a keyboard key, or a macro, which is a sequence of keyboard keys).

For this device, there is no official Linux support from the manufacturer; you can use the mouse with a computer running Linux as a regular mouse,
thanks to [this kernel module](https://github.com/torvalds/linux/blob/master/drivers/hid/hid-holtek-mouse.c#L27) (the comment mentions
Sharkoon Drakonia and Perixx MX-2000 mouse models, but according to `lsusb` the Perixx MX-3000 has Vendor ID 0x04D9 and Product ID 0xA067 as well),
but you can't run the configuration program supplied by the manufacturer, so you can't customize the DPI/buttons/etc settings.
All these settings are stored on a non-volatile flash memory on the mouse, so a possibility is to unplug the mouse every time you want to change the settings,
plug it in a computer running Windows, apply the changes using the software, and move the mouse back to the Linux computer.

Anyway, this isn't very handy, so I made an unofficial program which mimics the original one, but can run on Linux.
This document describes the steps taken to analyze the communication protocol used by the mouse and to implement it in a C program.
You may find it useful if for any reason you want to write an alternative/unofficial driver for a device which uses some sort of undocumented communication protocol.

### Gathering information
Obviously, the first step I took to achieve my goal was to search the Web for information;
I didn't find anyone which tried to achieve the same with this specific mouse, but I found some articles describing a similar process with other mice/devices, and,
in particular, I found the post [Reverse Engineering a USB mouse](https://bytepunk.wordpress.com/2017/03/25/reverse-engineering-a-usb-mouse/)
on the blog *bytepunk.wordpress.com* very useful.
As you can see, the article describes how to reverse engineer the protocol used by a similar mouse which uses a similar Windows-only application.
There are a lot of differences between the protocol of the mouse used by the author of the post and my Perixx MX-3000,
but reading the post gave me some ideas about how to proceed.

### Taking a look at what the official software does
In a very similar way to the steps described in the blog post mentioned in the previous section,
I set up a Windows 10 virtual machine using VirtualBox on my Arch Linux installation,
set up the USB pass-through to directly connect the mouse to the VM (by the way - when doing this,
you won't be able to use the mouse on the host system while the VM is running,
so it's handy to have another mouse or a touchpad) and installed the software provided by Perixx on the virtual machine;
at this point, I was able to configure the mouse from within the virtual machine, using the official software.

I had a look at using Wireshark to sniff USB traffic, but I found it a bit confusing and in the meantime I had a different idea -
the software must be using some Windows system calls or library functions to send data/commands to the mouse,
so maybe I could intercept these calls and look at the arguments they are called with to see what the program is sending to the mouse.
So I searched for programs which can show which library functions a Windows program calls,
and I found [API Monitor](http://www.rohitab.com/apimonitor) which does exactly what I was looking for.

With a small amount of trial and error, I found out that the process which actually sends data to the mouse is *hid.exe*
(there are 3 processes from the mouse software install folder which are running when you have the mouse configuration program open -
*hid.exe*, *trayicon.exe* and *config.exe*).
I hooked *API Monitor* into *hid.exe* and set the API capture filter to *"Windows Driver Kit > Device and Driver Technologies > Human Input Devices"*
(since a mouse is a HID device), then tried to change something in the configuration software and apply the new settings;
as expected, API Monitor captured something: a bunch of calls to [HidD_SetFeature](https://msdn.microsoft.com/en-us/library/windows/hardware/ff539684(v=vs.85).aspx).

Here is a screenshot of API Monitor and the mouse software running in the virtual machine and showing the captured function calls.

![Screenshot](https://i.imgur.com/qsqyLSJ.png)

At this point, I noticed I had two tasks to work on:

* Determining how to send the same data to the mouse from a Linux program - this was the easy one.
I had a look at the MSDN documentation for `HidD_SetFeature`, and found out that feature reports are a part of the HID standard, then found
[HIDAPI](https://github.com/signal11/hidapi), which is a cross-platform (and Linux is one of the supported OSs) library to interact with HID devices.
[Here](http://www.signal11.us/oss/hidapi/hidapi/doxygen/html/group__API.html) is the documentation; as expected, there is a handy `hid_send_feature_report`
function which is more or less equivalent to the `HidD_SetFeature` function from the Windows libraries.
* Figuring out how the data sent from the software to the mouse through HID feature reports relates to the settings chosen in the program.
This took way more time and effort, and is discussed in the next section.

### An unfriendly protocol
At this point, I had found the communication channel used by the software to send something to the mouse (HID feature reports);
what I could see was that when clicking "Apply" in the software, nine HID feature reports are sent to the mouse; each of them is 8 bytes long.
The next step is trying to make sense of said bytes, to learn how to make my Linux program translate user input
(e.g. "set sensor DPIs to 2000" or "set RGB LED to red") to these feature reports to send to the mouse.

In the beginning, I focused on the RGB LED color setting. To find out which one of the nine feature reports is related to this setting,
I first set one color in the software and applied it, then set a different color without touching any other setting and applied it as well;
all while API Monitor was recording the function calls. So at this point I had captured 18 calls to `HidD_SetFeature` - two sequences of nine;
I compared the contents of the byte buffer passed as argument to each of the calls in order (e.g. #1 with #10, #2 with #11) and found that the only one
which was different was number #6. So, this must be the "set led color" command and should contain values which identify the selected color.

The program's GUI allows to select a quite large range of colors, so I assumed that a color is encoded as 3 bytes - one for red, one for green and one for blue.
The author of the post I mentioned before finds that in the protocol used by his mouse,
the application sends a feature report in which 3 of the bytes are exactly the 3 color channels.

Unfortunately, in my case things looked more complex.
In fact, for example, the feature report sent by the application to set the mouse to `#FF6B01` is `07 42 7b b0 02 33 a7 aa`.
Something sounds strange, right? There is no trace of the three bytes which identify the color (FF, 6B, 01) in the buffer sent as feature report.

I tried to write down everything in binary instead of hexadecimal to see if the values weren't byte-aligned, but I still found no trace of those numbers.
I double-checked, and as said before this report (number #6 in the sequence) was the only one which was changing when setting different colors.
So, I realized there must be some kind of encoding/encryption scheme which is not so obvious as directly putting the values in the feature report.

To learn more about this mysterious encoding scheme, I had to dig deeper in the software's internals; at this point,
I felt API Monitor wasn't enough anymore and I needed a "real" debugger. I chose [x64dbg](https://x64dbg.com/#start)
(by the way, the mouse's software is an x86 - or 32-bit - binary and runs under [WOW64](https://en.wikipedia.org/wiki/WoW64)
on 64-bit Windows, but despite the name, x64dbg supports both x86 and x64 architectures).
So I hooked x64dbg into the running *hid.exe* process, and had a look at the Symbols tab.
I was lucky enough to find the function I was looking for was in an external DLL - *HIDApi.dll*, which despite the similar name,
has nothing to do with the open source HIDAPI library I mentioned before - instead of the main exe, because since this and other functions are exported
(to be used by *hid.exe* which is linked with *HIDApi.dll*), they have (somewhat) descriptive names.

The function I'm talking about is named `EncodeSecrecy_V11`.
I have no idea about what the "secret" could be in commands to configure a mouse,
but anyway the fact that this function *encodes* something sounded very related to what I was looking for;
so, I put a breakpoint on the first instruction of `EncodeSecrecy_V11` and clicked Apply in the mouse configuration software again.
I was hoping that there was one single call to `EncodeSecrecy_V11` before each call to `HidD_SetFeature` (encode command, then send it),
and at the 6th call I should have found something related to the color I was setting.

When the execution hit the breakpoint, I inspected the stack to see the arguments passed to the function.
As expected, the first one (at esp+4) was a pointer to a memory region; so, I clicked "Run" in the debugger 5 times to reach the 6th call
(and the execution flow kept hitting that breakpoint, as expected), and this time inspected the memory region pointed by the first argument passed to the function.
However, I didn't found any trace of the R, G and B color values yet: by reading 8 bytes (length of a feature report as passed to `HidD_SetFeature`;
I was assuming that unencoded and encoded reports had the same length) starting at that pointer pointer, I got `00 00 F2 00 00 00 F4 00`.
I kept clicking "Run" in `x64dbg` and I noticed that `EncodeSecrecy_V11` was being called way more times than nine,
so my assumption that there was one call for each feature report was wrong.

### Using a proxy DLL to dump useful information
I didn't want to manually inspect the memory for each call to the function, so I decided to automate the process.
Basically, I wanted to log a trace of all the calls to `EncodeSecrecy_V11` made during a mouse configuration transaction
(in other words, a click of "Apply" in the software) by writing the byte buffers passed as arguments to a file, one per line in hexadecimal.
This way, I could log two transaction in which I set two different colors to two different files, then compute a diff of them,
and hopefully one line would differ and contain the relevant values.

The fact that `EncodeSecrecy_V11` is in a DLL made this task quite simple (if it would have been in the executable,
I'd have needed to patch it and I'm not even sure it's possible). The technique I used is called *Proxy DLL*.
It consists in the following steps:

* Renaming the DLL which contains the function(s) to spy; for example, in this case I renamed *HIDApi.dll* to *HIDApi_original.dll*
* Building a new DLL (the proxy) which loads the original one and exports functions with the same names, implemented as jumps to the original DLL;
this DLL file needs to be put in place of the original one in the program's directory
* At this point, the proxy DLL can be customized to do something before jumping to the original functions (in this case, dump the arguments to a file)

Reading [this very interesting article](https://www.codeproject.com/Articles/16541/Create-your-Proxy-DLLs-automatically) about proxy DLLs and using
the attached tools has been a great help in achieving what I had in mind.
Getting a working compiler on the Windows VM required to download the entire [Visual C++ Build Tools Package](http://landinghub.visualstudio.com/visual-cpp-build-tools);
after installing it, I managed to compile the program attached to the article (the zip contains a single file, `wrappit.cpp`) and use it to generate the source
of the proxy DLL. I then customized the implementation of `EncodeSecrecy_V11` to print the byte buffer to a string (in hexadecimal) and write it to a file.
The source file of the proxy DLL is [`analysis-tools/hidapi.cpp`](https://github.com/gianluca-nitti/mx3000control/blob/master/analysis-tools/hidapi.cpp)
in this repository.

By compiling it (for convenience I made a small batch script to automate the task,
[`analysis-tools/compile.bat`](https://github.com/gianluca-nitti/mx3000control/blob/master/analysis-tools/compile.bat))
I got my customized *HIDApi.dll* to put in the software's installation folder, next to *hid.exe* and *HIDApi_original.dll*.
I then stopped the running instance of *hid.exe* (the installer sets it to automatically start at boot) using the Task Manager and started a new one;
now *hid.exe* loads *HIDApi.dll* thinking it's the original version, but it's actually loading my proxy version, which in turns loads the original one,
and dispatches the function calls to it - but in case of a call to `EncodeSecrecy_V11`, before doing it, it appends the argument to a MOUSE-LOG.txt file on the Desktop.

For convenience, I also made a small Powershell script to copy all the data related to a single transaction (a click on *Apply*) to a dedicated file.
You can find it at [`analysis-tools/record.ps1`](https://github.com/gianluca-nitti/mx3000control/blob/master/analysis-tools/record.ps1).
When it's started, it counts the existing lines in the MOUSE-LOG.txt file, then prompts the user to apply the changes using the mouse software and press Enter;
when Enter is pressed, it copies the new lines to the file specified as command line argument.

With these tools set up, the reverse engineering process became quite trivial.
These are the steps I took to understand how to build the feature report which sets the RGB LED color:

* In the mouse configuration program, go to the *Color control* tab and select a color, then take note of it's RGB value
(the software doesn't show the values, so I used [`grabc`](https://www.muquit.com/muquit/software/grabc/grabc.html)
on the Linux host to grab the color shown in the software from the VirtualBox window); for example, the color I chose (orange) is #ff6b01
* At the Windows command prompt:
```
C:\Users\test\Desktop>powershell ./record.ps1 E:\mouse-ff6b01
Recording calls to EncodeSecrecy_V11, press a key when done...
Press Enter to continue...:
```
* Now, click *Apply* in the software
* When the mouse's RGB LED stops blinking (configuration transaction completed), go back to the command prompt and press Enter;
now `E:\mouse-ff6b01` contains a log of all the calls to `EncodeSecrecy_V11` made during this transaction
* Then, repeat the process with a different color, for example #0e0fff
* Now, I have two files - `E:\mouse-ff6b01` and `E:\mouse-0e0fff` which should be almost identical because I didn't touch any of the other mouse configuration parameters,
but should differ in the line which contains the color values; I previously set up `E:\` on the Windows VM to be a shared folder with `/tmp` on the Linux host, so
I just needed to run `diff` at a terminal in the Linux host:
```
[gianluca@archlinux ~]$ diff /tmp/mouse-ff6b01 /tmp/mouse-0e0fff
206c206
< 00 07 0A 6B FF 01 00 00 00
---
> 00 07 0A 0F 0E FF 00 00 00
```
And **FINALLY**, I found how the R, G and B color values are related to the feature reports which are sent!
From the output of `diff`, it's quite clear that the format is `00 07 0A <G> <R> <B> 00 00 00`.
But, let's not forget that this has to go through `EncodeSecrecy_V11` before being sent, otherwise the mouse wouldn't understand it.

### Reimplementing the encoding function
At this point, I had managed to capture byte strings which can easily be constructed from user input;
for example, to continue with the RGB LED color example, the user could enter the three red, green and blue
values in my Linux program and ask it to send the command to the mouse.
As discovered in the previous section, it's very easy for the program to build the "raw", unencoded command:
the first bytes (07, 0A) are fixed, the three following bytes are the color values, then pad with zeroes.

However, this is not the "language" the mouse talks: as said before, the official Windows program doesn't send the commands in this format,
but passes them through the encoding function. So obviously, I needed to implement this encoding scheme in my program as well to make the mouse understand the commands.

Again, the x64dbg debugger/disassembler has been very helpful in the process of finding out what `EncodeSecrecy_V11` actually does.
Here is the disassembled function (exported from x86dbg *graph* tab):

![Screenshot](https://i.imgur.com/NjDRLfU.png)

To get a first working implementation, I just "mechanically" translated the assembly to C by using some variables which directly map to the registers.
You can see this version in [encoding.c at commit 157c48a](https://github.com/gianluca-nitti/mx3000control/blob/157c48a1d53c5f4fb5fb00d9773c820cfdacf6ec/encoding.c).
Very ugly, but it worked! At this point, the program was able to change the mouse's RGB LED color.
You can see the rest of the code by browsing the repository at that commit. It's quite simple; the other relevant parts are in
[main.c](https://github.com/gianluca-nitti/mx3000control/blob/157c48a1d53c5f4fb5fb00d9773c820cfdacf6ec/main.c), which enumerates the USB devices with the mouse's
Vendor ID and Product ID (which I found out with `lsusb`), and in
[command_setrgbled.c](https://github.com/gianluca-nitti/mx3000control/blob/157c48a1d53c5f4fb5fb00d9773c820cfdacf6ec/command_setrgbled.c),
which builds the raw command according to user input values (which need to be passed to the program as command line arguments), then encodes and sends it.

To spend some more words on this now demystified `EncodeSecrecy_V11` - here is a rough description of what it does:

* The first part takes a 7-byte string (in the graph, x64dbg is showing it's ASCII representation - "oNgtEng") which is statically stored in the program's `.data` section,
and modifies it by swapping the two nibbles in each byte; the result is placed on the stack. These are the first two blocks in the graph of the disassembled original program.
In my implementation, this part is absent because there is no reason to do this at runtime since the string is always the same; I just put the pre-computed byte array in the program
(it's the `key` variable; if you look at an ASCII table - [Here](http://www.asciitable.com/) or type `man ascii` at a Linux shell -
you'll notice that the values I put in the array are in fact the bytes which make up the original string - "oNgtEng" - with the nibbles swapped, e.g. `o` is `0x6F`
and the first entry in the array is `0xF6`; same process for the other bytes).
* It then takes the input byte string, skips the first two bytes, and sums to each byte of the input the corresponding byte in the "key" - the byte string built at the previous step.
This is in the 3rd and 4th blocks in the graph, and the for loop in my implementation.
* Then, it reassigns each byte of the input - still skipping the first two - to itself shifted left of 3 bits put in a binary OR with the next byte
in the string shifted right of 5 bits. Sorry for the contorted explanation, but it's easier to write it in code.
This is the 5th block - the large one - of the graph (I suppose it was a loop in the original code and the compiler unrolled it).
In my first implementation, it's the ugly part which mimics the assembly code; as I'll later explain, I then rewrote the function in a more readable way and this is implemented
as a `for (int i = 2; i < 8; i++) buf[i] = (buf[i + 1] >> 5) | (buf[i] << 3);`.
* Then, in the assembly code there is a loop (the 6th block) which XORs the byte string with another string from the `.data` section; however, at that address there are just zeroes
and XORing with zeroes is a no-op. So, I just ignored this (didn't implement it in my program) and everything works correctly.
* Finally (7th block), some of the bytes in the string are swapped - with 0-indexing, the byte at index 2 is swapped with the one at index 5, 4 with 8 and 6 with 7.

After understanding in detail what the function was doing, I rewrote my implementation in a more readable way using loops; you can see the current implementation
[here](https://github.com/gianluca-nitti/mx3000control/blob/master/encoding.c).

#### Wait, what?
On a side note, I do aknowledge that what I just wrote - swapping nibbles in a byte, moving around the order of bytes in an array, etc - probably
sounds like a bunch of nonsense, but I'm just describing what I saw in the disassembled binary (and then reimplemented it, because  this is the protocol
the mouse understands - maybe the firmware applies the reverse function - and in fact, everything is working). If you want to have a look yourself,
[here](http://perixx.eu/en/service/MX-3000-V1_2.zip) is the download link for the official mouse software; it needs to be installed on a Windows machine, then you can hook
a debugger in `hid.exe` (or statically disassemble `HIDApi.dll`) and look for the `EncodeSecrecy_V11` symbol.

The only reason which comes to my mind that developers may have had to implement such an "encryption" scheme is to achieve some level of
[security through obscurity](https://en.wikipedia.org/wiki/Security_through_obscurity), but I don't really see why you should "secure" some
control messages going from a computer to a mouse through an USB connection. Maybe it was just to discourage people to do what I did (writing
an alternative driver)...

### Implementing other simple features
After finding out the command format and the encoding scheme and implementing the "set led color" feature, implementing the commands to set other features of the mouse
not related to the buttons has been a very simple task. With the help of the previously built proxy DLL and Powershell script, I used the same process (capture two transactions
which differ only in the value assigned to the interested setting, then use diff) to find out the format of the feature reports to set the sensor's DPI, the X/Y sensivity,
and the RGB LED's light intensity and pulsation effect speed.
The following is the output of `diff` on the dumps of two transactions which differ in the DPI setting; to generate the first one, in the software I set the sensor DPIs to
600 (the lowest value), and to generate the second one I set them to 8200 (the highest allowed).
```
[gianluca@archlinux ~]$ diff /tmp/dpi-600 /tmp/dpi-8200
209c209
< 00 07 09 01 00 00 00 00 00
---
> 00 07 09 07 00 00 00 00 00
```
I also noticed that in the program there are 7 possible DPI values to set, so everything makes sense. This made it easy to add a "set DPI" command to the Linux program, which
builds the 9-byte array A where A[1]=0x07, A[2]=0x09, A[3] is set according to the value selected by the user (after checking that it's in the 1 to 7 range) and the other
entries are set to zero, passes it through the `encode` function previously developed, then sends the result as a HID feature report. I tested the program with all the allowed
values (1 to 7) and it worked - the cursor speed increased when setting the DPIs to higher values; moreover, there are four blue LEDs on the mouse which light up according to the
current DPI value (only the 1st LED is on when DPI=600, only the 2nd when DPI=1200 and so on; then at the 4th value - 4800 DPI - two LEDs light up, until all of them are on
when the maximum DPI value is set) and they reacted as expected.

Setting the RGB LED's light intensity and pulsation effect works exactly with the same logic, with the difference that instead of `0x09` the 3rd byte is `0x0C` for intensity
and `0x0B` for pulsation effect, and the range of the allowed values for the 4th byte is 1 through 4 instead of 1 through 7 (in both cases; for light intensity 1 means LED off
and 4 means full brightness, while for effect 1 means steady and 4 means fast pulsation). The relevant code for this features is mainly in
[util.c](https://github.com/gianluca-nitti/mx3000control/blob/master/util.c#L70); the `execute_simple_command` function provides a generalized implementation of these features
which use the same feature report format (DPI, light intensity and effect), and is called in the
[commands/setdpi.c](https://github.com/gianluca-nitti/mx3000control/blob/master/commands/setdpi.c),
[commands/setledintensity.c](https://github.com/gianluca-nitti/mx3000control/blob/master/commands/setledintensity.c) and
[commands/setledpulsation.c](https://github.com/gianluca-nitti/mx3000control/blob/master/commands/setledpulsation.c) files.

The format of the feature report to set the X and Y sensivity of the mouse is a bit different because a single report carries two values,
but still easy to figure out using the same tools. The original software's GUI has two sliders which allow to set X and Y sensivity in values from 1 through 10,
so as usual I recorded the calls to `EncodeSecrecy_V11` of two transactions; in the first one I set X sensivity to 1 and Y to 2, in the second one X to 9 and Y to 10;
here is the output of `diff` on the transactions dumps:
```
[gianluca@archlinux ~]$ diff /tmp/sensivity-x-1-y-2 /tmp/sensivity-x-9-y-10
3c3
< 00 07 12 02 01 00 00 00 00
---
> 00 07 12 0A 09 00 00 00 00
```
So as you can see, 3rd byte (which probably serves as a sort of "command identifier") is `0x12`, 4th byte is Y sensivity and 5th byte is X sensivity. The feature for setting
sensivity is implemented in [commands/setsensivity.c](https://github.com/gianluca-nitti/mx3000control/blob/master/commands/setsensivity.c).

### Figuring out how to configure the buttons
At this point, the Linux program was able to configure various features of the mouse, but not to change the button mapping (which is possible through the official
Windows software). It took me a while to understand how this worked: by setting different button functions in the application and capturing the calls to the encoding
function in the usual way I could find some differences using `diff`, but by looking at the calls to `HidD_SetFeature` in *API Monitor*, I couldn't find any call which
argument was the byte array obtained by applying the encoding function to the differing lines. Moreover, none of the arguments passed to the nine calls to `HidD_SetFeature`
changed between two transactions with different button mappings; thus, the button configuration isn't sent to the mouse through HID feature reports but through some other channel.
This is also the reason why, as I noticed during the first attempts, the number of calls to `EncodeSecrecy_V11` is higher than the number of calls to `HidD_SetFeature`
during a configuration transaction: all the values which will be sent to the mouse are encoded, but only some of them are sent as feature reports.

I had no idea about which other Windows library function calls the program was using, so I used *Wireshark* with the *usbmon* kernel module as explained
[here](https://wiki.wireshark.org/CaptureSetup/USB) to sniff all the USB traffic. With `lsusb` I found the bus and device numbers (respectively 3 and 7 in this case)
so that I could set a filter in Wireshark  - by typing `usb.dst contains 3.7` in the "view filter" bar - to see only the packets sent to the mouse.
Here is a screenshot showing part of the capture of the Windows program setting the mouse to the default factory settings:

![Screenshot](https://i.imgur.com/79LGFEQ.png)

Here are the first lines of the file produced by capturing the byte arrays passed to `EncodeSecrecy_V11` as described above during the same transaction:
```
00 07 03 01 00 00 00 00 00
00 07 02 01 00 00 00 00 00
00 07 12 0A 0A 00 00 00 00
00 07 04 01 40 00 00 00 00
00 00 F0 00 00 00 F1 00 00
```
By passing each line to the encoding function (I added an `encode` command to the program for these testing purpose) we get this:
```
00 07 3A 2B B7 CF 33 A7 B2
00 07 3A 2B B7 C7 33 A7 B2
00 07 3A 74 B0 47 33 A7 02
00 07 3A 2D B7 D7 33 A7 B2
00 00 3A 23 B7 37 33 2F B2
```
As you may have noticed, the last 2 of these 5 lines are exactly the two packets shown in the Wireshark screenshot. Also, with some tests I found that the 5th line was
changing if the mapping assigned to certain buttons changed. So, the packet shown on the right carries information related to the button map and isn't a feature report
because I couldn't see it in *API Monitor*, while the packet shown on the left is a feature report because in *API Monitor* I saw a call (#4 in the sequence)
to `HidD_SetFeature` with the same byte array of the *Data Fragment* shown in Wireshark passed as argument.

I noticed that these two USB frames were different not only in *Data Fragment* but also in *wValue* and *wIndex*; after doing some research about USB and HID, I found in
the [USB HID specification](http://www.usb.org/developers/hidpage/HID1_11.pdf) (pages 51 and 52) that a in a *Get_Report* or *Set_Report* request, *wValue* indicates
the *report type*: *wValue=3*, as expected, identifies a feature report, while *wValue=2* identifies an **output report**. This is what I have to send to replicate the
behavior of the original program - an HID output report; with the HIDAPI library this can easily be done with the
[`hid_write`](http://www.signal11.us/oss/hidapi/hidapi/doxygen/html/group__API.html#gad14ea48e440cf5066df87cc6488493af) function.

After figuring out the transport channel, I still had to understand the relationship between those bytes and the button mappings configured in the application,
but this was an easy task to accomplish with the usual `diff`-based method. There are, though, some relevent differences between the button configuration procedure
and the previously implemented configuration commands: the button configuration consists of multiple output reports that must be sent in sequence, preceeded by
three "fixed" (meaning that they don't depend on user input) feature reports as "preamble" (I think they more or less tell the mouse firmware to start accepting
the next output reports as button mapping data); you can't just remap a single button, you have to rewrite the entire configuration for all of them.
Also, the button configuration sequence must be followed by the macro configuration sequence (see next section for more information), even if none of the buttons
is configured to use a macro (in this case, the macro configuration will just be a long sequence of output reports full of zeroes).
I tried sending just one of the output reports and the mouse "hanged" (if I moved it, the on-screen cursor didn't move); probably the firmware and microcontroller
are not "multi-tasked" and are just sitting waiting for another command to configure the next button. Fortunately, this didn't corrupt or bring to an inconsistent state
the on-board flash memory so after power-cycling it (unplug USB and plug it back in) the mouse was back working correctly.

Each of the 5 output reports which change when remapping buttons in the official program carries information about two buttons; the configuration of a button consists of 4 bytes.
The mouse has 8 buttons (including scroll wheel), so two of these entries - the 2nd half of the 3rd output report and the 1st half of the 4th report are not used
(the official software always sets them to zeroes, which is also the sequence to disable a button; this can be verified by setting a button to the "Disabled" option
and noticing an additional group of 4 zeroes). Moreover, there are other 3 reports which are sent before the macro configuration reports which are always all zeroes;
so there are a total of 8 reports sent in the button configuration phase. As we'll see later there is also space for 16 macros, so my guess is that the microcontroller-firmware
pair present in this mouse is the same of other mouse models which can support up to sixteen buttons, but only 8 of them are actually present on the Perixx MX-3000.

The following list describes the different types of actions which can be assigned to a button from the official software's GUI and how they map to the 4-byte value.

* A button can be mapped to an action chosen from a set of pre-defined functions, like browser back/forward, DPI up/down, LED color switch, etc. This was the easiest
type to figure out as it requires no additional parameters - the 4-byte array just consists of a fixed value for each function; the results of the analysis have been used
to write [`keymap.c`](https://github.com/gianluca-nitti/mx3000control/blob/master/keymap.c).
* Alternatively, a button can be assigned a keyboard key. I noticed that when assigning different keys to the same buttons, only the 2nd byte was changing, while
the other 3 were always zeroes; after observing the values associated with various keyboard keys and doing a bit of research, I figured out that the value is the key
[scancode](https://en.wikipedia.org/wiki/Scancode). Lists of USB HID scancodes can be found for example [here](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2)
and [here](https://source.android.com/devices/input/keyboard-devices#code-tables).
* Finally, a button can also be assigned a macro, which is a sequence of keyboard events (key up or down). This required a bit more work, so it's described in the next section.

By the way, the number of pre-defined functions in `keymap.c` is inferior to the pre-defined functions that can be found in the official software because some of them, like
cut/copy/paste, close window, etc are just encoded as macros; the only difference is that you don't have to manually record the macro with the keyboard because it's already
in the program. I embedded the same macros - exluding the ones which are OS-specific -
in [`default-macros.c`](https://github.com/gianluca-nitti/mx3000control/blob/master/default_macros.c).

The code which implements the button configuration feature is in [`commands/setbuttons.c`](https://github.com/gianluca-nitti/mx3000control/blob/master/commands/setbuttons.c).

### Macros
As mentioned in the previous section, to reconfigure buttons the PC first sends a sequence of HID output reports to the mouse, then sends a fixed (independent from
user input) feature report - `00 07 05 01 00 06 00 00 00` encoded with the function described before - and finally a sequence of 176 output reports, which through
the usual logging method, I noticed were changing when assigning different macros to the mouse buttons.

I didn't need to completely reverse engineer the macro format, because I found some documentation. When searching for information about the mouse, I noticed that
according to the USB Vendor ID (which, on Linux, you can see with the `lsusb` command) it's based on a microcontroller manufactured by [Holtek](http://www.holtek.com),
so I started browsing this company's website hoping to find some useful information. I downloaded some of the development kits for their MCUs, and I found useful
material in Holtek USB Workshop, which can be downloaded from [this page](http://www.holtek.com/development-platform-software)
(or [direct link](http://www.holtek.com/documents/10179/108746/HTUSBWorkshopV152B0131112_Setup.exe)). On a side note: the file is a .exe installer for Windows, but
it's built with [InnoSetup](http://www.jrsoftware.org/isinfo.php) and the contents can easily be extracted with [innoextract](http://constexpr.org/innoextract/)
without actually running the installer, even if you don't have a Windows machine.

By exploring the files extracted from the archive, I found an interesting document in the `app/apnote/mouse/device/Holtek HT68FB550 Gaming Mouse/` folder.
This folder contains PDFs and source code which, as you can probably guess by the name, show an usage example of an Holtek MCU for a USB gaming mouse design.
I found this before starting all the work, but I only mention it here because it's for macros that it becomes useful. In fact, the mouse described in the document
has similar features to mine, and the settings are sent from the PC to the mouse through HID reports, but the data format for the commands I described in the
previous settings is different, and, most importantly, doesn't involve any sort of "encryption", while my Perixx MX-3000 mouse and its official software
use that weird `EncodeSecrecy_V11` which has been the most time-consuming part to reverse-engineer (mostly because I didn't expect it, and it took me a while
to find out why there wasn't an obvious correlation between the values selected in the program and the captured reports).

It turned out, however, that the macro format described in the Holtek document is almost the same of the one used in my mouse.
The document describes that a macro is encoded in the following way:

* The first two bytes, most significant byte first, are the number of repetitions of the macro (when binding a macro to a button, you can specify how many times the
sequence must be repeated when the button is pressed: up to 65535 times)
* The following bytes, in pairs, define the keyboard actions associated with the macro:
	* The first byte of a pair has the following meaning:
		* Most significant bit determines if the event to trigger is key down (0) or key up (1)
		* The other 7 bits determine the delay between this and the next event; the range is 0 to 127 (since it's 7 bits), which maps to zero delay up to 6350ms,
with each increment mapping to a 50ms delay increment; thus, the delays between macro entries can be choosen within multiples of 50ms in the 0-6350ms range
	* The second byte of each pair is the scancode of the keyboard key to emulate

Finding this document made it very easy to figure out how to send macros to the mouse, because the format used by the Perixx MX-3000 is based on the one described
in the PDF. After some observation (using the tool to capture the arguments to the encoding function described in previous sections, and Wireshark with usbmon to see
that the output of each of these calls was being sent as an HID output report), I noticed that after sending the `00 07 05 01 00 06 00 00 00` feature report mentioned
at the beginning of this section (which probably puts the mouse in a sort of "macro programming mode"), each macro is sent as a sequence of 11 output reports.
Each raw report ("raw" as in "before going through the encoding function") is 9 bytes long, and the first byte is always zero; the other 8 are filled according to the
macro format described above. Thus, with 11 reports each carrying 8 bytes of information, a macro can be up to 8\*11=88 bytes long; first two bytes are for the
repetition count, so there are 86 bytes for the key entries, and since each of them is 2 bytes long, a macro can simulate a sequence of up to 86/2=43 key press/release events.

A total of 16 macros are sent in sequence (without any feature or output report between them); this is where the 176 output reports mentioned at the beginning of this
section comes from (11\*16=176). The order is important, because to bind a macro to a button, the 4 bytes which configure the button (as described in the previous section)
need to be set to `53 00 <macro index> <macro length>` (in hex, as always), where the 1st sent to the mouse has index 0, the 2nd one has index 1 and so on, and the length
is expressed in bytes (including the two ones for repetition count).

The relevant code for macro handling is in [`macro.c`](https://github.com/gianluca-nitti/mx3000control/blob/master/macro.c),
[`macro.h`](https://github.com/gianluca-nitti/mx3000control/blob/master/macro.h) and
[`commands/setbuttons.c`](https://github.com/gianluca-nitti/mx3000control/blob/master/commands/setbuttons.c).
Note that a large part of `macro.c` is just a parser for (*almost*) user friendly strings to describe macros (which are passed to the program on the command line); the part
which actually encodes and sends macro data to the mouse is in the `send_macro` function which is about ten lines long.

### Conclusion
Thank you for reading. This program probably won't be useful to much people since the Perixx MX-3000 doesn't seem to be a popular product, but it has been a nice learning
experience for me, and it might save you some time if you want to provide Linux support for some similar mouse/device.

Any comment is welcome in the [issue tracker](https://github.com/gianluca-nitti/mx3000control/issues) of this repository.

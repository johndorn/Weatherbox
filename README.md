Weatherbox
==========

An Arduino project that can get a visual representation of the current temperature.

This project was created to give to my dad as a birthday gift. 

![alt tag](https://dl.dropboxusercontent.com/u/24982912/IMG_20130711_210012.jpg)

Here's a parts list:

-Arduino Uno (I'm using R2, even though the current model is R3)
-Arduino WiFi Shield
-Radio Shack RGB True Color LED (276-0028)
-(3) 68 ohm resistors
-Protoboard


Quick note about the hardware:

The LED has a red, blue, green cathode and a common anode. When soldering to the protoboard, each of the RGB cathodes have a resistor between the light and the input. I'm using inputs 3, 5, and 6. The common anode connects to the 5V power.

Once the WiFi connection is established, it runs an HTTP GET against weather.gov, which returns XML data. (the location I'm using is KNAK, which is the weather code of the Annapolis Naval Academy). You can reference http://w1.weather.gov/xml/current_obs/ to find your local weather code.

When the XML is returned, it parses the data and looks for the 'weather' and 'temperature' properties. At this point, it applies an RGB value based on temperature in farenheit. Hotter = red/orange, Mild = yellow/green, Cold = Blue.

Future plans include adding a bright white flicker of the light (to simulate lightning/storms) when the 'weather' property indicates percipitation.

If you have any questions, please reach out. I'll also add more pictures when I'm not so tired :)

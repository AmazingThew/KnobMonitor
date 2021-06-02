# KNOB MONITOR
## GUI frontend for [Knob Sock](https://github.com/AmazingThew/KnobSock)

![the konbs with the arcade feeling](https://i.imgur.com/mV7Il6D.png)

Knob Sock is a lightweight tool to send MIDI knob data over sockets, typically to aid in tuning values for gamedev/creative-code projects.
Knob Monitor is a Windows-native GUI frontend, which shows the controller state and allows easy access to knob values.

# USE CASE:
- Assume your game has a Knob Sock client, and you are using knobs to control various shader parameters
- Once you've tuned the parameters to your liking, you need to assign the knob values as constants somewhere in your code/data
- Typically, you'd do this by copying the values out of the Knob Sock console window, but this is unpleasant
- With Knob Monitor running, simply click on the desired knob in the GUI and its value will be copied to your clipboard

# SETUP:
[Grab the release binary](https://github.com/AmazingThew/KnobMonitor/releases/download/1.1/KnobMonitor-v1.1.zip) because compiling C++ yourself is for psychopaths.
You'll need to edit config.json to describe the layout of your MIDI controller. Each element in the KnobPages list describes the positions and indices of all the knobs that you want to see on one screen.
Typically, if your controller has multiple banks, or you have connected multiple controllers, each bank or controller would get its own page (pages do not need to have the same shape or number of knobs)
Each page consists of a list of rows of knobs. Each row entry is the index of the knob you want to appear onscreen (these are the indices you configured in Knob Sock).
The elements on each page are laid out as you see them visually. For example:

```
{
    "KnobPages": [
        [
            [ 4, 5, 6, 7 ],
            [ 0, 1, 2, 3 ]
        ]
	]
}
```
would appear thusly:

![the picture for eight konbs](https://i.imgur.com/iG7UF9P.png)

# CONTROLS:
- Clicking on any knob will copy its value to your clipboard
- Left and right arrow keys will change pages

# NOTES:
- Performance impact of running the monitor is low; it only redraws itself in response to input events and thus does almost nothing when the knobs aren't moving
- It's currently configured to connect using Knob Sock's default settings and a maximum of 64 knobs. For stupid reasons this is not currently configurable
- I made a cool icon so you can tell it's a REAL program

# Contact
[Go yell at me on twitter](https://twitter.com/AmazingThew)

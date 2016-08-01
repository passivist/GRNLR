# GRNLR
### developing a VST/AU plugin in the JUCE Framework
**GRNLR** is a quasisyncronous sample based granular synthesizer. It is an adaptation of
a synthesizer I wrote in SuperCollider and the approach is inspired by Curtis Roads book
*"Microsound"*.

GRNLR is the practical part of my bachelors thesis. This readme being the theoretical part
and also a tutorial on how to port a SuperCollider program to VST/AU using the JUCE Framework.

## Tutorial
The tutorial assumes that you have not wrote any "serious" C++ before but have like me programmed
some in artist oriented programming languages like SuperCollider, Processing, OpenFrameworks or PD/MAX.
It documents my journey from a relatively finished concept and a working prototype in SuperCollider
so I will often reference the process of "porting" the program from SuperCollider to JUCE. <doppelt?>  

Some knowledge in digital audio might come in handy but I'll try to keep the technical details
as sparse and simple as possible.

### Getting started
JUCE can be obtained from the [JUCE](http://juce.com) Website or from the JUCE
[github project](https://github.com/julianstorer/JUCE).
JUCE is free for open source projects but if you want to keep your source closed you will have to get a commercial
license.
Before starting with this Tutorial I recommend following at least the
["Getting started with the Projucer"](https://www.juce.com/doc/tutorial_new_projucer_project)
tutorial and the
["Create a basic Audio/MIDI plugin Part 1"](https://www.juce.com/doc/tutorial_create_projucer_basic_plugin)
tutorial. For Linux users there is also a handy
[list of all the dependencies you'll need](https://forum.juce.com/t/list-of-juce-dependencies-under-ubuntu-linux/15121).

Once you are comfortable with your IDE of choice and the Projucer as well as the Plugin-Host you can start by

### Making Plans  

openframeworks-audiounit
========================

This lets you make an audiounit using openframeworks. There are a number of things that don't work, like setting the size in main. You need to set this in resource.h.

In order to rename the audio unit, you have to change every instance of the name ofxAudioUnitPluginExample - including in class names, in the .exp file, the actual file names, in the settings an in the Info.plist.

It's based on WDL-OL which is a cross-platform wrapper for writing audio plugins. I based it off the IPlugOpenGL demo that comes with it. Essentially, it creates an opengl context, draws everything to an FBO, reads that back from the GPU and passes it into the WDL-OL system as an array of RGBA pixels.


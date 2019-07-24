# lkjb Open Source Plugins

This repository contains all lkjb open source plugins which are:
- PitchedDelay (previously a separate repository)
- Luftikus
- Refine
- SuperSpread

A JUCE fork is included as submodule which contains the JUCE 5.4.3 sources with the following modifications:
- GPL mode in Projucer enabled
- Disabled analytics (usage reporting) in sources as well as in Projucer
- Disabled update checking in Projucer as it should be possible to build the plugins without any further modifications from the repository
- Changed Projucer settings directory to "Projucer (lkjb mod)". This allows to use the lkjb fork Projucer in parallel to the normal Projucer.

# Building

The following steps should suffice to build the plugins:
1. Build Projucer in `juce/extras/Projucer/Builds`. Choose the fitting IDE's subfolder.
2. Run Projucer and save the plugin's `.jucer` files in their respective plugins directories. If you want to also build VST2 plugins copy the VST2.4 SDK inside the `vstsdk2.4` directory. Only the `extras\Projucer\Builds` and `public.sdk` folders are needed. Currently (07/2019) the SDK can be download from [archive.org](https://archive.org/details/VST2SDK).
3. After saving the `.jucer` files open the created IDE projects and compile.

The build process was tested using VisualStudio 2017 on Windows 10 and XCode 8.2.1 on macOS 10.11 (El Capitan).

# License
JUCE is licensed under the GPL v3 or a commercial license. See juce.com for more details.
The lkjb part of the code is now licensed under the MIT license. Feel free to use the code in other products or create (hopefully freeware) AAX plugins.
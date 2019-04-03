# Sunfire Instruments

This is the repository for the development of **Sunfire Instruments Modules** for use in VCV Rack.

## Modules

In VCV Rack you may find multiple modules listed under one brand. For example you may open the context menu to find `Sunfire Instruments` and when clicking on that, all of its modules.

The idea of this entire repository is to have multiple modules nested under one brand. If you want to create a new branch, you'd have to make a separate repository to facilitate this.

## Setting up

Refer to [the template module](https://vcvrack.com/manual/PluginDevelopmentTutorial.html#) on the VCV Rack Manual website. It actually explains a lot on module development in general.

Main differences with this repo include:

* The way the makefile looks up the SDK folder
* The way we deploy the module via the dedicated script

In order to fully use this build script, you will have to create two private files that point to the correct folders on your system.

* `.rack-plugins-folder` is a file containing the path string to the `plugins` folder of your VCV Rack installation
* `.rack-sdk-path` is a file containing the path string to the SDK folder. You may download the latest SDK or build Rack yourself. But this should point to that root.

## Building

Use `./deploy.sh` and everything will be updated automatically for you. Please open and close Rack, updating plugins does not seem to refresh them.

If you're using Windows, then you might run into trouble with the makefile. You're best to develop on Ubuntu/Linux or OSX, or propose a change in a PR that allows both operating systems to run the makefile properly.

## Resources

Sunfire Instruments aims to make free/payware modules. This implies that we **cannot** use resources that are provided by the Rack Team or Grayscale. Please refer to [the Licenses page](https://github.com/VCVRack/Rack#licenses) for a detailed list of resources we cannot use.

Instead we'll aim to define our own custom knobs and behavior. @rafael-mindreau will take care of adding new SVG's styles and defining the Sunfire Identity onto them. If you want to propose a new component/button/control, you can open a PR with an RFC or similar.

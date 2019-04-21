#include "rack.hpp"


using namespace rack;

// Forward-declare the Plugin, defined in Template.cpp
extern Plugin *plugin;

// Forward-declare each Model, defined in each module source file
extern Model *modelMIDICCInterface;


// Declare custom controls
struct PushButtonSmall : SVGSwitch, MomentarySwitch {
	PushButtonSmall() {
		addFrame(SVG::load(assetPlugin(plugin, "res/PushButtonSmall.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/PushButtonSmallPressed.svg")));
	}
};

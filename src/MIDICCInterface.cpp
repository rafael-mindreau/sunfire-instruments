#include "Sunfire.hpp"
#include "midi.hpp"
#include "dsp/filter.hpp"
#include "dsp/digital.hpp"
#include "window.hpp"


struct MIDICCFavourite {
  uint8_t channel;
  uint8_t cc;

  MIDICCFavourite() {
    this->channel = 0;
    this->cc = 0;
  }

  MIDICCFavourite(uint8_t channel, uint8_t cc) {
    this->channel = channel;
    this->cc = cc;
  }
};


struct MIDICCInterface : Module {
  enum ParamIds {
    ENUMS(LEARN_BUTTON, 32),
    NUM_PARAMS
  };
  enum InputIds {
    NUM_INPUTS
  };
  enum OutputIds {
    ENUMS(CC_OUTPUT, 32),
    NUM_OUTPUTS
  };
  enum LightIds {
    ENUMS(LEARN_LIGHT, 32),
    NUM_LIGHTS
  };

  MidiInputQueue midiInput;
  int8_t ccs[16][128];

  int learningFavouriteId = -1;
  MIDICCFavourite favourites[32];
  SchmittTrigger learnTriggers[32];

	MIDICCInterface() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

  void onReset() override {
    // Reset all CC values to 0 per channel
    for (int channel = 0; channel < 16; channel++) {
      for (int cc = 0; cc < 128; cc++) {
        ccs[channel][cc] = 0;
      }
    }

    // Initialise grid with the first 32 CCs on channel 0
    for (int favouriteId = 0; favouriteId < 32; favouriteId++) {
      favourites[favouriteId] = MIDICCFavourite(0, favouriteId);
    }

    learningFavouriteId = -1;
  }

  void step() override {
    MidiMessage msg;
    // Process any queued midi messages
    while (midiInput.shift(&msg)) {
      processMessage(msg);
    }

    // Process any button presses

    // For all learned MIDI CCs, get their midi values from the backing ccs list
    float lambda = 100.f * engineGetSampleTime();
    for (int favouriteId = 0; favouriteId < 32; favouriteId++) {
      // Process any learn-button presses
      if (learnTriggers[favouriteId].process(params[LEARN_BUTTON + favouriteId].value)) {
        // Turn off any previous learn lights
        if (learningFavouriteId > -1) {
          lights[LEARN_LIGHT + learningFavouriteId].value = 0.0;
        }

        learningFavouriteId = favouriteId;
        lights[LEARN_LIGHT + favouriteId].value = 1.0;
      }

      // Set outputs with their corresponding learned CC values
      int learnedCc = favourites[favouriteId].cc;
      int channel = favourites[favouriteId].channel;
      float value = rescale(clamp(ccs[channel][learnedCc], -127, 127), 0, 127, 0.f, 10.f);
      // ccFilters[favouriteId].lambda = lambda;
      outputs[CC_OUTPUT + favouriteId].value = value;
    }
  }

  void processMessage(MidiMessage msg) {
    // Only listen to CC type message
    // http://www.onicos.com/staff/iz/formats/midi-event.html
    if (msg.status() == 0xb) {
      uint8_t cc = msg.note();
      uint8_t channel = msg.channel();

      // If we're in learn-mode, and the msg is not 0x00, apply the CC to the listening grid item per channel
      if (learningFavouriteId >= 0 && ccs[channel][cc] != msg.data2) {
        favourites[learningFavouriteId].cc = cc;
        favourites[learningFavouriteId].channel = channel;

        // Set led to dim to indicated a learned state
        lights[LEARN_LIGHT + learningFavouriteId].value = 0.2;
        learningFavouriteId = -1;
      }

      // Set CV
      // Allow CC to be negative if the 8th bit is set
      ccs[channel][cc] = msg.data2;
    }
  }

  json_t *toJson() override {
    return json_object();

  }

  void fromJson(json_t *rootJ) override {

  }
};


struct MIDICCInterfaceWidget : ModuleWidget {
	MIDICCInterfaceWidget(MIDICCInterface *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/MIDICCNew.svg")));

    // Buttons
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 120), module, MIDICCInterface::LEARN_BUTTON + 0, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 120), module, MIDICCInterface::LEARN_BUTTON + 1, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 120), module, MIDICCInterface::LEARN_BUTTON + 2, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 120), module, MIDICCInterface::LEARN_BUTTON + 3, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 150), module, MIDICCInterface::LEARN_BUTTON + 4, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 150), module, MIDICCInterface::LEARN_BUTTON + 5, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 150), module, MIDICCInterface::LEARN_BUTTON + 6, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 150), module, MIDICCInterface::LEARN_BUTTON + 7, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 180), module, MIDICCInterface::LEARN_BUTTON + 8, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 180), module, MIDICCInterface::LEARN_BUTTON + 9, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 180), module, MIDICCInterface::LEARN_BUTTON + 10, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 180), module, MIDICCInterface::LEARN_BUTTON + 11, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 210), module, MIDICCInterface::LEARN_BUTTON + 12, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 210), module, MIDICCInterface::LEARN_BUTTON + 13, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 210), module, MIDICCInterface::LEARN_BUTTON + 14, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 210), module, MIDICCInterface::LEARN_BUTTON + 15, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 240), module, MIDICCInterface::LEARN_BUTTON + 16, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 240), module, MIDICCInterface::LEARN_BUTTON + 17, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 240), module, MIDICCInterface::LEARN_BUTTON + 18, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 240), module, MIDICCInterface::LEARN_BUTTON + 19, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 270), module, MIDICCInterface::LEARN_BUTTON + 20, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 270), module, MIDICCInterface::LEARN_BUTTON + 21, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 270), module, MIDICCInterface::LEARN_BUTTON + 22, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 270), module, MIDICCInterface::LEARN_BUTTON + 23, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 300), module, MIDICCInterface::LEARN_BUTTON + 24, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 300), module, MIDICCInterface::LEARN_BUTTON + 25, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 300), module, MIDICCInterface::LEARN_BUTTON + 26, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 300), module, MIDICCInterface::LEARN_BUTTON + 27, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(30, 330), module, MIDICCInterface::LEARN_BUTTON + 28, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(65, 330), module, MIDICCInterface::LEARN_BUTTON + 29, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(100, 330), module, MIDICCInterface::LEARN_BUTTON + 30, 0.0, 1.0, 0.0));
    addParam(ParamWidget::create<PushButtonSmall>(Vec(135, 330), module, MIDICCInterface::LEARN_BUTTON + 31, 0.0, 1.0, 0.0));

    // Screws
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    // Outputs
    addOutput(Port::create<PJ301MPort>(Vec(5, 120), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 0));
    addOutput(Port::create<PJ301MPort>(Vec(40, 120), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 1));
    addOutput(Port::create<PJ301MPort>(Vec(75, 120), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 2));
    addOutput(Port::create<PJ301MPort>(Vec(110, 120), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 3));
    addOutput(Port::create<PJ301MPort>(Vec(5, 150), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 4));
    addOutput(Port::create<PJ301MPort>(Vec(40, 150), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 5));
    addOutput(Port::create<PJ301MPort>(Vec(75, 150), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 6));
    addOutput(Port::create<PJ301MPort>(Vec(110, 150), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 7));
    addOutput(Port::create<PJ301MPort>(Vec(5, 180), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 8));
    addOutput(Port::create<PJ301MPort>(Vec(40, 180), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 9));
    addOutput(Port::create<PJ301MPort>(Vec(75, 180), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 10));
    addOutput(Port::create<PJ301MPort>(Vec(110, 180), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 11));
    addOutput(Port::create<PJ301MPort>(Vec(5, 210), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 12));
    addOutput(Port::create<PJ301MPort>(Vec(40, 210), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 13));
    addOutput(Port::create<PJ301MPort>(Vec(75, 210), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 14));
    addOutput(Port::create<PJ301MPort>(Vec(110, 210), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 15));
    addOutput(Port::create<PJ301MPort>(Vec(5, 240), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 16));
    addOutput(Port::create<PJ301MPort>(Vec(40, 240), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 17));
    addOutput(Port::create<PJ301MPort>(Vec(75, 240), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 18));
    addOutput(Port::create<PJ301MPort>(Vec(110, 240), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 19));
    addOutput(Port::create<PJ301MPort>(Vec(5, 270), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 20));
    addOutput(Port::create<PJ301MPort>(Vec(40, 270), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 21));
    addOutput(Port::create<PJ301MPort>(Vec(75, 270), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 22));
    addOutput(Port::create<PJ301MPort>(Vec(110, 270), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 23));
    addOutput(Port::create<PJ301MPort>(Vec(5, 300), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 24));
    addOutput(Port::create<PJ301MPort>(Vec(40, 300), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 25));
    addOutput(Port::create<PJ301MPort>(Vec(75, 300), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 26));
    addOutput(Port::create<PJ301MPort>(Vec(110, 300), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 27));
    addOutput(Port::create<PJ301MPort>(Vec(5, 330), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 28));
    addOutput(Port::create<PJ301MPort>(Vec(40, 330), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 29));
    addOutput(Port::create<PJ301MPort>(Vec(75, 330), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 30));
    addOutput(Port::create<PJ301MPort>(Vec(110, 330), Port::OUTPUT, module, MIDICCInterface::CC_OUTPUT + 31));

    // Lights
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 132), module, MIDICCInterface::LEARN_LIGHT + 0));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 132), module, MIDICCInterface::LEARN_LIGHT + 1));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 132), module, MIDICCInterface::LEARN_LIGHT + 2));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 132), module, MIDICCInterface::LEARN_LIGHT + 3));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 162), module, MIDICCInterface::LEARN_LIGHT + 4));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 162), module, MIDICCInterface::LEARN_LIGHT + 5));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 162), module, MIDICCInterface::LEARN_LIGHT + 6));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 162), module, MIDICCInterface::LEARN_LIGHT + 7));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 192), module, MIDICCInterface::LEARN_LIGHT + 8));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 192), module, MIDICCInterface::LEARN_LIGHT + 9));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 192), module, MIDICCInterface::LEARN_LIGHT + 10));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 192), module, MIDICCInterface::LEARN_LIGHT + 11));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 222), module, MIDICCInterface::LEARN_LIGHT + 12));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 222), module, MIDICCInterface::LEARN_LIGHT + 13));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 222), module, MIDICCInterface::LEARN_LIGHT + 14));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 222), module, MIDICCInterface::LEARN_LIGHT + 15));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 252), module, MIDICCInterface::LEARN_LIGHT + 16));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 252), module, MIDICCInterface::LEARN_LIGHT + 17));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 252), module, MIDICCInterface::LEARN_LIGHT + 18));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 252), module, MIDICCInterface::LEARN_LIGHT + 19));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 282), module, MIDICCInterface::LEARN_LIGHT + 20));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 282), module, MIDICCInterface::LEARN_LIGHT + 21));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 282), module, MIDICCInterface::LEARN_LIGHT + 22));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 282), module, MIDICCInterface::LEARN_LIGHT + 23));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 312), module, MIDICCInterface::LEARN_LIGHT + 24));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 312), module, MIDICCInterface::LEARN_LIGHT + 25));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 312), module, MIDICCInterface::LEARN_LIGHT + 26));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 312), module, MIDICCInterface::LEARN_LIGHT + 27));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(30, 342), module, MIDICCInterface::LEARN_LIGHT + 28));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(65, 342), module, MIDICCInterface::LEARN_LIGHT + 29));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(100, 342), module, MIDICCInterface::LEARN_LIGHT + 30));
    addChild(ModuleLightWidget::create<MediumLight<GreenLight>>(Vec(135, 342), module, MIDICCInterface::LEARN_LIGHT + 31));

		MidiWidget *midiWidget = Widget::create<MidiWidget>(Vec(5, 30));
		midiWidget->box.size = Vec(140, 85);
		midiWidget->midiIO = &module->midiInput;
		addChild(midiWidget);
	}
};


Model *modelMIDICCInterface = Model::create<MIDICCInterface, MIDICCInterfaceWidget>("Sunfire Instruments", "MIDICCInterface", "MIDI-CC", MIDI_TAG, EXTERNAL_TAG);

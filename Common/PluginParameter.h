/*
 * Created On September 5th 2020.
 * By Khalil Kabara.
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
using Parameter = AudioProcessorValueTreeState::Parameter;

//==============================================================================

class PluginParametersManager
{
public:
    PluginParametersManager (AudioProcessor& p) : valueTreeState (p, nullptr)
    {
    }

    AudioProcessorValueTreeState valueTreeState;
    StringArray parameterTypes;
    Array<StringArray> comboBoxItemLists;
};

//==============================================================================

class PluginParameter
    : public LinearSmoothedValue<float>
    , public AudioProcessorValueTreeState::Listener
{
protected:
    PluginParameter (PluginParametersManager& parametersManager,
                     const std::function<float (float)> callback = nullptr)
        : parametersManager (parametersManager)
        , callback (callback)
    {
    }

public:

    void updateValue (float value)
    {
        if (callback != nullptr)
            setCurrentAndTargetValue (callback (value));
        else
            setCurrentAndTargetValue (value);
    }

    void parameterChanged (const String& parameterID, float newValue) override
    {
        updateValue (newValue);
    }

    PluginParametersManager& parametersManager;
    std::function<float (float)> callback;
    String paramID;
};

//==============================================================================

class PluginParameterSlider : public PluginParameter
{
protected:
    PluginParameterSlider (PluginParametersManager& parametersManager,
                           const String& paramName,
                           const String& labelText,
                           const float minValue,
                           const float maxValue,
                           const float defaultValue,
                           const std::function<float (float)> callback,
                           const bool logarithmic)
        : PluginParameter (parametersManager, callback)
        , paramName (paramName)
        , labelText (labelText)
        , minValue (minValue)
        , maxValue (maxValue)
        , defaultValue (defaultValue)
    {
        paramID = paramName.removeCharacters (" ").toLowerCase();
        parametersManager.parameterTypes.add ("Slider");

        NormalisableRange<float> range (minValue, maxValue);
        if (logarithmic)
            range.setSkewForCentre (sqrt (minValue * maxValue));

        parametersManager.valueTreeState.createAndAddParameter (std::make_unique<Parameter>
            (paramID, paramName, labelText, range, defaultValue,
				[](float val) { return String(val, 2); },
             [](const String& text){ return text.getFloatValue(); })
        );

        parametersManager.valueTreeState.addParameterListener (paramID, this);
        updateValue (defaultValue);
    }

public:
    const String& paramName;
    const String& labelText;
    const float minValue;
    const float maxValue;
    const float defaultValue;
};


//======================================

class PluginParameterLinSlider : public PluginParameterSlider
{
public:
    PluginParameterLinSlider (PluginParametersManager& parametersManager,
                              const String& paramName,
                              const String& labelText,
                              const float minValue,
                              const float maxValue,
                              const float defaultValue,
                              const std::function<float (float)> callback = nullptr)
        : PluginParameterSlider (parametersManager,
                                 paramName,
                                 labelText,
                                 minValue,
                                 maxValue,
                                 defaultValue,
                                 callback,
                                 false)
    {
    }
};

//======================================

class PluginParameterLogSlider : public PluginParameterSlider
{
public:
    PluginParameterLogSlider (PluginParametersManager& parametersManager,
                              const String& paramName,
                              const String& labelText,
                              const float minValue,
                              const float maxValue,
                              const float defaultValue,
                              const std::function<float (float)> callback = nullptr)
        : PluginParameterSlider (parametersManager,
                                 paramName,
                                 labelText,
                                 minValue,
                                 maxValue,
                                 defaultValue,
                                 callback,
                                 true)
    {
    }
};

//==============================================================================

class PluginParameterToggle : public PluginParameter
{
public:
    PluginParameterToggle (PluginParametersManager& parametersManager,
                           const String& paramName,
                           const bool defaultState = false,
                           const std::function<float (float)> callback = nullptr)
        : PluginParameter (parametersManager, callback)
        , paramName (paramName)
        , defaultState (defaultState)
    {
        paramID = paramName.removeCharacters (" ").toLowerCase();
        parametersManager.parameterTypes.add ("ToggleButton");

        const StringArray toggleStates = {"False", "True"};
        NormalisableRange<float> range (0.0f, 1.0f, 1.0f);

        parametersManager.valueTreeState.createAndAddParameter (std::make_unique<Parameter>
            (paramID, paramName, "", range, (float)defaultState,
             [toggleStates](float value){ return toggleStates[(int)value]; },
             [toggleStates](const String& text){ return toggleStates.indexOf (text); })
        );

        parametersManager.valueTreeState.addParameterListener (paramID, this);
        updateValue ((float)defaultState);
    }

    const String& paramName;
    const bool defaultState;
};

//==============================================================================

class PluginParameterComboBox : public PluginParameter
{
public:
    PluginParameterComboBox (PluginParametersManager& parametersManager,
                             const String& paramName,
                             const StringArray items,
                             const int defaultChoice = 0,
                             const std::function<float (const float)> callback = nullptr)
        : PluginParameter (parametersManager, callback)
        , paramName (paramName)
        , items (items)
        , defaultChoice (defaultChoice)
    {
        paramID = paramName.removeCharacters (" ").toLowerCase();
        parametersManager.parameterTypes.add ("ComboBox");

        parametersManager.comboBoxItemLists.add (items);
        NormalisableRange<float> range (0.0f, (float)items.size() - 1.0f, 1.0f);

        parametersManager.valueTreeState.createAndAddParameter (std::make_unique<Parameter>
            (paramID, paramName, "", range, (float)defaultChoice,
             [items](float value){ return items[(int)value]; },
             [items](const String& text){ return items.indexOf (text); })
        );

        parametersManager.valueTreeState.addParameterListener (paramID, this);
        updateValue ((float)defaultChoice);
    }

    const String& paramName;
    const StringArray items;
    const int defaultChoice;
};

//==============================================================================

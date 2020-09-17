/*
  ==============================================================================

    DisplayComponent.cpp
    Created: 15 Sep 2020 4:48:32pm
    Author:  Khalil Kabara

  ==============================================================================
*/

#include "DisplayComponent.h"

String DisplayComponent::displayText = "Worq";
bool DisplayComponent::durationExpired = false;
bool DisplayComponent::pluginJustLoaded = true;
int DisplayComponent::currentDisplayDuration = 0;

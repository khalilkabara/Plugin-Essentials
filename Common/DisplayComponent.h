/*
  ==============================================================================

    DisplayComponent.h
    Created: 15 Sep 2020 4:43:56pm
    Author:  Khalil Kabara

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class DisplayComponent : public juce::Component, public Timer
{
public:
	DisplayComponent()
	{
		startTimerHz(fps);

		displaySectionImage = ImageCache::getFromMemory(BinaryData::display_area_png,
		                                                BinaryData::display_area_pngSize);
	}

	~DisplayComponent() override
	{
	}

	void paint(juce::Graphics& g) override
	{
		g.drawImageWithin(*&displaySectionImage,
		                  getLocalBounds().getX(),
		                  getLocalBounds().getY(),
		                  getLocalBounds().getWidth(),
		                  getLocalBounds().getHeight(), RectanglePlacement::fillDestination);

		g.setColour(juce::Colours::whitesmoke);
		g.drawFittedText(displayText, getLocalBounds(), juce::Justification::centred, 1);
	}

	void timerCallback() override
	{
		if (!durationExpired)
			currentDisplayDuration += 1;

		if (pluginJustLoaded) currentPluginLifetime += 1;

		if (pluginJustLoaded && currentPluginLifetime >= maxLoadDuration * fps) pluginJustLoaded = false;

		if (!durationExpired && currentDisplayDuration >= maxDisplayDuration * fps)
		{
			durationExpired = true;
			currentDisplayDuration = 0;
			displayText = defaultDisplayText;
		}

		repaint();
	}

	void resized() override
	{
	}

	static void setDisplayText(String& text)
	{
		if (pluginJustLoaded) return;

		displayText = text;

		durationExpired = false;
		currentDisplayDuration = 0;
	}

	void about(String text)
	{
		std::unique_ptr<AlertWindow> alertWindow{ new AlertWindow("About", text + extraAboutText, AlertWindow::NoIcon) };
		// ScopedPointer<AlertWindow> alertWindow = new AlertWindow("About", text, AlertWindow::NoIcon);
		alertWindow->addButton("Close", 0);
		alertWindow->setVisible(true);
		alertWindow->setUsingNativeTitleBar(true);
		// alertWindow->setColour(AlertWindow::backgroundColourId, Colours::lightslategrey);

		int result = alertWindow->runModalLoop();
	}
	
	const bool debugRects = false;

private:

	Image displaySectionImage;

	static String displayText;
	static int currentDisplayDuration;
	static bool durationExpired;

	const String defaultDisplayText{"Worq"};
	const int fps{12};
	const int maxDisplayDuration{5};
	int maxLoadDuration{3};
	int currentPluginLifetime{0};

	static bool pluginJustLoaded;

	const String extraAboutText{ " Plugin \nWorq Studios \nwww.worq.com.ng \nworqmedia@gmail.com" };
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisplayComponent)
};

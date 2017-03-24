#include "hack_audio_Label.h"

HackAudio::Label::Label()
{
    setInterceptsMouseClicks(false, false);

    setColour(backgroundColourId, HackAudio::Colours::Black);
    setColour(textColourId, HackAudio::Colours::White);
    setColour(textWhenEditingColourId, HackAudio::Colours::Cyan);

    currentColourInterpolation.reset(50, 0.5);

    animationStatus = true;

    addListener(this);
}

HackAudio::Label::~Label()
{

}

juce::String HackAudio::Label::getPrefix()
{
    return prefix;
}

void HackAudio::Label::setPrefix(juce::String preText, juce::NotificationType notificationType)
{
    prefix = preText;

    if (notificationType == juce::sendNotification)
    {
        labelTextChanged(this);
    }

    repaint();
}

juce::String HackAudio::Label::getPostfix()
{
    return postfix;
}

void HackAudio::Label::setPostfix(juce::String postText, juce::NotificationType notificationType)
{
    postfix = postText;

    if (notificationType == juce::sendNotification)
    {
        labelTextChanged(this);
    }

    repaint();
}

void HackAudio::Label::setAnimationStatus(bool shouldAnimate)
{
    animationStatus = shouldAnimate;
}

void HackAudio::Label::labelTextChanged(juce::Label* labelThatHasChanged)
{
    if (animationStatus)
    {
        currentColourInterpolation.setValue(1.0f);
        startTimerHz(ANIMATION_FPS);
    }
}

void HackAudio::Label::parentHierarchyChanged()
{
    if (dynamic_cast<HackAudio::Diagram*>(getParentComponent()))
    {
        setColour(backgroundColourId, HackAudio::Colours::Gray);
    }
    else
    {
        setColour(backgroundColourId, HackAudio::Colours::Black);
    }
}

void HackAudio::Label::timerCallback()
{

    if (currentColourInterpolation.isSmoothing())
    {
        repaint();

        if (std::abs(currentColourInterpolation.getTargetValue() - currentColourInterpolation.getNextValue()) < 0.0001)
        {
            currentColourInterpolation.setValue(currentColourInterpolation.getTargetValue());
        }
    }
    else
    {
        if (currentColourInterpolation.getTargetValue() == 1.0f)
        {
            currentColourInterpolation.setValue(0.0f);
        }
        else
        {
            if (currentColourInterpolation.getNextValue() == 0.0f)
            {
                stopTimer();
            }
            return;
        }
    }
}

void HackAudio::Label::paint(juce::Graphics& g)
{

    int width  = getWidth();
    int height = getHeight();

    juce::Path p;
    p.addRoundedRectangle(0, 0, width, height, CORNER_RADIUS, CORNER_CONFIG);
    g.setColour(findColour(backgroundColourId));
    g.fillPath(p);

    g.setColour(findColour(textColourId).interpolatedWith(findColour(textWhenEditingColourId), currentColourInterpolation.getNextValue()));

    g.setFont(getFont());
    g.drawText(prefix + getText() + postfix, 0, 0, width, height, juce::Justification::centred, 1);

}

void HackAudio::Label::resized()
{

    int width = getWidth();
    int height = getHeight();

    getFont().setHeight(std::min(width, height) / 4);
    
}

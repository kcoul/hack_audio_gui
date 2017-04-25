#include "hack_audio_Label.h"

#include <regex>

HackAudio::Label::Label()
{

    setInterceptsMouseClicks(true, false);

    setJustificationType(juce::Justification::centred);

    setColour(HackAudio::backgroundColourId, HackAudio::Colours::Black);
    setColour(HackAudio::foregroundColourId, HackAudio::Colours::White);
    setColour(HackAudio::highlightColourId, HackAudio::Colours::Cyan);

    colourInterpolation.reset(50, 0.5);

    animationStatus = true;
    placeholderStatus = false;
    formattingStatus = true;

    timeout = 0;

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

void HackAudio::Label::setPlaceholder(juce::String placeholderText)
{
    placeholder = placeholderText;
    placeholderStatus = true;
    repaint();
}

void HackAudio::Label::setPlaceholderStatus(bool shouldShowPlaceholder)
{
    placeholderStatus = shouldShowPlaceholder;
    repaint();
}

juce::String HackAudio::Label::getPlaceholder()
{
    return placeholder;
}

void HackAudio::Label::setAnimationStatus(bool shouldAnimate)
{

    animationStatus = shouldAnimate;

}

bool HackAudio::Label::getAnimationStatus()
{

    return animationStatus;

}

void HackAudio::Label::setFormattingStatus(bool shouldFormatText)
{

    formattingStatus = shouldFormatText;
    repaint();

}

bool HackAudio::Label::getFormattingStatus()
{

    return formattingStatus;

}

void HackAudio::Label::mouseEnter(const juce::MouseEvent& e)
{

    if (placeholderStatus)
    {

        if (findColour(HackAudio::backgroundColourId) == HackAudio::Colours::Gray)
        {

            setColour(HackAudio::backgroundColourId, HackAudio::Colours::Gray.withMultipliedBrightness(1.25f));

        }

    }

}

void HackAudio::Label::mouseExit(const juce::MouseEvent& e)
{

    if (placeholderStatus)
    {

        if (findColour(HackAudio::backgroundColourId) == HackAudio::Colours::Gray.withMultipliedBrightness(1.25f))
        {

            setColour(HackAudio::backgroundColourId, HackAudio::Colours::Gray);

        }

    }

}

void HackAudio::Label::mouseUp(const juce::MouseEvent& e)
{

    if (placeholderStatus && !e.mouseWasDraggedSinceMouseDown())
    {

        if (getText().isNotEmpty())
        {

            labelTextChanged(this);

        }

    }
    
}

void HackAudio::Label::labelTextChanged(juce::Label* labelThatHasChanged)
{

    if (animationStatus)
    {

        timeout = 75;
        colourInterpolation.setValue(1.0f);
        startTimerHz(ANIMATION_FPS);

    }

}

void HackAudio::Label::timerCallback()
{

    if (colourInterpolation.isSmoothing())
    {
        
        repaint();

        if (std::abs(colourInterpolation.getTargetValue() - colourInterpolation.getNextValue()) < 0.0001)
        {
            colourInterpolation.setValue(colourInterpolation.getTargetValue());
        }

    }
    else
    {

        if (colourInterpolation.getTargetValue() == 1.0f)
        {
            colourInterpolation.setValue(0.0f);
        }
        else
        {

            if (timeout > 0)
            {
                timeout--;

            }
            else
            {
                repaint();
                stopTimer();
            }

        }

    }

}

juce::GlyphArrangement HackAudio::Label::formatText(juce::String stringToFormat)
{

    int width  = getWidth();
    int height = getHeight();

    juce::GlyphArrangement glyphs;

    std::regex r
    (
        "([^\\^_]+|([\\^_][^\\s\\^_]){2}|[\\^_]\\{[^\\}]+\\}|[\\^_][^\\s\\^_]?)"
    );

    juce::Font font = getFont();
    int fontHeight = font.getHeight();
    int currentHeight = fontHeight;

    int offset = 0;
    int baseline = 0;

    std::string s(stringToFormat.toUTF8());

    std::sregex_iterator begin(s.begin(), s.end(), r), end;

    for (auto i = begin; i != end; ++i)
    {

        juce::String jstring = juce::String(i->str());

        if (jstring.startsWith("^"))
        {

            if (baseline > 0)
            {
                baseline -= baseline / 2;
            }
            else if (baseline < 0)
            {
                baseline += baseline / 4;
            }
            else
            {
                baseline -= currentHeight / 2;
            }

            currentHeight -= currentHeight / 4;

            if (jstring.startsWith("^{"))
            {

                jstring = jstring.substring(2);
                jstring = jstring.upToFirstOccurrenceOf("}", false, false);

                glyphs.addLineOfText
                (
                    font.withHeight(currentHeight),
                    jstring,
                    offset,
                    baseline
                );

            }
            else if (jstring.matchesWildcard("^*_*", true))
            {

                glyphs.addLineOfText
                (
                    font.withHeight(currentHeight),
                    jstring.substring(1,2),
                    offset,
                    baseline
                );

                glyphs.addLineOfText
                (
                    font.withHeight(currentHeight),
                    jstring.substring(3, 4),
                    offset,
                    baseline + baseline * -2
                );

            }
            else
            {

                glyphs.addLineOfText
                (
                    font.withHeight(currentHeight),
                    jstring.substring(1),
                    offset,
                    baseline
                );

            }

        }
        else if (jstring.startsWith("_"))
        {

            if (baseline > 0)
            {
                baseline += baseline / 4;
            }
            else if (baseline < 0)
            {
                baseline -= baseline / 2;
            }
            else
            {
                baseline += currentHeight / 2;
            }

            currentHeight -= currentHeight / 4;

            if (jstring.startsWith("_{"))
            {

                jstring = jstring.substring(2);
                jstring = jstring.upToFirstOccurrenceOf("}", false, false);

                glyphs.addLineOfText
                (
                    font.withHeight(currentHeight),
                    jstring,
                    offset,
                    baseline
                );
                
            }
            else if (jstring.matchesWildcard("_*^*", true))
            {

                glyphs.addLineOfText
                (
                 font.withHeight(currentHeight),
                 jstring.substring(1,2),
                 offset,
                 baseline
                 );

                glyphs.addLineOfText
                (
                 font.withHeight(currentHeight),
                 jstring.substring(3, 4),
                 offset,
                 baseline + baseline * -2
                 );
                
            }
            else
            {

                glyphs.addLineOfText
                (
                 font.withHeight(currentHeight),
                 jstring.substring(1),
                 offset,
                 baseline
                );

            }

        }
        else
        {

            baseline = 0;
            currentHeight = fontHeight;

            glyphs.addLineOfText(font.withHeight(currentHeight), jstring, offset, baseline);

        }

        offset = (int)glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true).getWidth();

    }

    glyphs.justifyGlyphs(0, glyphs.getNumGlyphs(), 12, 12 + 4, width - 24, height - 24, getJustificationType());

    return glyphs;

}

void HackAudio::Label::paint(juce::Graphics& g)
{

    int width  = getWidth();
    int height = getHeight();

    juce::Path p;
    p.addRoundedRectangle(0, 0, width, height, CORNER_RADIUS, CORNER_CONFIG);
    g.setColour(findColour(HackAudio::backgroundColourId));
    g.fillPath(p);

    juce::Colour foreground = findColour(HackAudio::foregroundColourId);
    juce::Colour highlight  = findColour(HackAudio::highlightColourId);

    g.setColour(foreground.interpolatedWith(highlight, colourInterpolation.getNextValue()));

    if (formattingStatus)
    {

        if (!isTimerRunning() && placeholderStatus)
        {

            formatText(placeholder).draw(g);

        }
        else
        {

            formatText(getText()).draw(g);

        }

    }
    else
    {

        g.setFont(getFont());

        juce::String textToDisplay;
        textToDisplay = (!isTimerRunning() && placeholderStatus) ? placeholder : prefix + getText() + postfix;

        g.drawText(textToDisplay, 12, 12, width - 24, height - 24, getJustificationType(), 1);

    }

}

void HackAudio::Label::resized()
{

    int width = getWidth();
    int height = getHeight();

    getFont().setHeight(std::min(width, height) / 4);
    
}

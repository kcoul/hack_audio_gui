/* Copyright (C) 2017 by Antonio Lassandro, HackAudio LLC
 *
 * hack_audio_gui is provided under the terms of The MIT License (MIT):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

HackAudio::Button::Button() : juce::Button("")
{

    setColour(HackAudio::backgroundColourId, HackAudio::Colours::Black);
    setColour(HackAudio::midgroundColourId,  HackAudio::Colours::Gray);
    setColour(HackAudio::foregroundColourId, HackAudio::Colours::White);
    setColour(HackAudio::highlightColourId,  HackAudio::Colours::Cyan);

    setFont(HackAudio::Fonts::NowRegular.withHeight(HackAudio::FontHeights::Medium));

	setButtonText("");

	setClickingTogglesState(true);
	setTriggeredOnMouseDown(false);
    setRepaintsOnMouseActivity(false);

	resizeGuard = false;

    wasFocusedByTab = false;

    colourInterpolation.reset(50, 0.35);

}

HackAudio::Button::~Button()
{

}

void HackAudio::Button::setButtonText(const char *newText)
{

    juce::String charPtr(newText);
    juce::Button::setButtonText(charPtr);
    
}

void HackAudio::Button::setButtonStyle(HackAudio::Button::ButtonStyle style)
{

    buttonStyle = style;
    resized();
    repaint();

}

HackAudio::Button::ButtonStyle HackAudio::Button::getButtonStyle() const
{

    return buttonStyle;

}

void HackAudio::Button::setFont(juce::Font font)
{

    buttonFont = font;
    resized();
    repaint();

}

juce::Font HackAudio::Button::getFont() const
{

    return buttonFont;

}

void HackAudio::Button::mouseMove(const juce::MouseEvent &e)
{

    if (!isEnabled())
    {

        setMouseCursor(juce::MouseCursor::NormalCursor);
        return;

    }

    if (buttonStyle != ButtonStyle::SlidingToggle)
    {

        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        return;

    }
    else
    {

        if (trackArea.contains(e.getPosition()) || thumbArea.contains(e.getPosition()))
        {

            setMouseCursor(juce::MouseCursor::PointingHandCursor);
            return;

        }

    }

    setMouseCursor(juce::MouseCursor::NormalCursor);

}

void HackAudio::Button::mouseDown(const juce::MouseEvent& e)
{

    if (!isEnabled()) { return; }

    colourInterpolation.setTargetValue(1.0f);

    if (buttonStyle == ButtonStyle::Bar)
    {

        startTimerHz(ANIMATION_FPS);
        setToggleState(true, juce::sendNotification);

    }
    else if (buttonStyle == ButtonStyle::BarToggle)
    {
        
        startTimerHz(ANIMATION_FPS);
        juce::Button::mouseDown(e);

    }
    else if (buttonStyle == ButtonStyle::SlidingToggle)
    {

        isDraggable = false;

        if (!trackArea.contains(e.getPosition()) && !thumbArea.contains(e.getPosition())) { return; }

        if (thumbArea.contains(e.getPosition()))
        {
            isDraggable = true;
        }

    }
}

void HackAudio::Button::mouseDrag(const juce::MouseEvent& e)
{

    if (!isEnabled()) { return; }

    if (buttonStyle != ButtonStyle::SlidingToggle)
    {

        juce::Button::mouseDrag(e);

    }
    else
    {

        if (!isDraggable || !e.mouseWasDraggedSinceMouseDown()) { return; }

        int xoffset = thumbArea.getX();

        if (e.x > getWidth() / 4 || e.x < getWidth() / 2)
        {
            thumbArea.setX(xoffset + (e.getDistanceFromDragStartX() / 4));
        }

        if (thumbArea.getX() < getWidth() / 4)
        {
            thumbArea.setX(getWidth() / 4);
        }
        else if (thumbArea.getX() > getWidth() / 2)
        {
            thumbArea.setX(getWidth() / 2);
        }

        indicatorArea.setWidth((thumbArea.getX() - indicatorArea.getX()) + thumbArea.getWidth()/2);

        repaint();

    }

}

void HackAudio::Button::mouseUp(const juce::MouseEvent& e)
{

    if (!isEnabled()) { return; }

    if (buttonStyle == ButtonStyle::Bar)
    {

        setToggleState(false, juce::dontSendNotification);

    }
    else if (buttonStyle == ButtonStyle::BarToggle)
    {

        juce::Button::mouseUp(e);

    }
    else if (buttonStyle == ButtonStyle::SlidingToggle)
    {

        if (trackArea.contains(e.getPosition()) || isDraggable)
        {

            animationStart.setXY(thumbArea.getX(), thumbArea.getY());

            if (e.x <= getWidth() / 2)
            {
                setToggleState(false, juce::sendNotification);
                animationEnd.setXY(getWidth() / 4, getHeight() / 4);
            }
            else
            {
                setToggleState(true, juce::sendNotification);
                animationEnd.setXY(getWidth() / 2, getHeight() / 4);
            }

            startTimerHz(ANIMATION_FPS);

        }

    }
    
}

bool HackAudio::Button::keyPressed(const juce::KeyPress &key)
{

    if (key.getKeyCode() == key.escapeKey)
    {

        juce::Component::unfocusAllComponents();
        return true;

    }


    if (buttonStyle == ButtonStyle::SlidingToggle)
    {

        if (key.getKeyCode() == key.leftKey)
        {

            animationStart.setXY(thumbArea.getX(), thumbArea.getY());

            setToggleState(false, juce::sendNotification);
            animationEnd.setXY(getWidth() / 4, getHeight() / 4);

            startTimerHz(ANIMATION_FPS);

            return true;

        }
        else if (key.getKeyCode() == key.rightKey)
        {

            animationStart.setXY(thumbArea.getX(), thumbArea.getY());
            setToggleState(true, juce::sendNotification);
            animationEnd.setXY(getWidth() / 2, getHeight() / 4);
            
            startTimerHz(ANIMATION_FPS);
            
            return true;
            
        }

    }
    else
    {

        if (key.getKeyCode() == key.spaceKey)
        {

            triggerClick();
            return true;

        }

    }

    return false;

}

void HackAudio::Button::focusGained(juce::Component::FocusChangeType cause)
{

    if (cause == juce::Component::focusChangedByTabKey)
    {

        wasFocusedByTab = true;

        if (buttonStyle == ButtonStyle::SlidingToggle)
        {

            setColour(HackAudio::midgroundColourId, HackAudio::Colours::LightGray);

        }

        repaint();

    }

}

void HackAudio::Button::focusLost(juce::Component::FocusChangeType cause)
{

    wasFocusedByTab = false;
    setColour(HackAudio::midgroundColourId,  HackAudio::Colours::Gray);
    repaint();

}

void HackAudio::Button::enablementChanged()
{

    if (isEnabled())
    {

        setColour(HackAudio::backgroundColourId, HackAudio::Colours::Black);
        setColour(HackAudio::midgroundColourId,  HackAudio::Colours::Gray);
        setColour(HackAudio::foregroundColourId, HackAudio::Colours::White);
        setColour(HackAudio::highlightColourId,  HackAudio::Colours::Cyan);

    }
    else
    {

        if (buttonStyle == ButtonStyle::SlidingToggle)
        {

            setColour(HackAudio::backgroundColourId, HackAudio::Colours::Black);
            setColour(HackAudio::midgroundColourId,  HackAudio::Colours::Gray);
            setColour(HackAudio::foregroundColourId, HackAudio::Colours::Black);
            setColour(HackAudio::highlightColourId,  HackAudio::Colours::Black);

        }
        else
        {

            setColour(HackAudio::backgroundColourId, HackAudio::Colours::Black);
            setColour(HackAudio::midgroundColourId,  HackAudio::Colours::Gray);
            setColour(HackAudio::foregroundColourId, HackAudio::Colours::Gray);
            setColour(HackAudio::highlightColourId,  HackAudio::Colours::Gray);

        }
        
    }
    
    repaint();

}

void HackAudio::Button::timerCallback()
{

    if (buttonStyle != ButtonStyle::SlidingToggle)
    {

        if (colourInterpolation.isSmoothing())
        {
            
            repaint();

            if (std::abs(colourInterpolation.getTargetValue() - colourInterpolation.getNextValue()) < 0.0001)
            {
                colourInterpolation.setTargetValue(colourInterpolation.getTargetValue());
            }

        }
        else
        {
            if (colourInterpolation.getTargetValue() == 1.0f && !juce::Component::isMouseButtonDownAnywhere())
            {

                colourInterpolation.setTargetValue(0.0f);

            }
            else if (colourInterpolation.getTargetValue() == 1.0f)
            {

                colourInterpolation.setTargetValue(1.0f);

            }
            else
            {

                if (colourInterpolation.getNextValue() == 0.0f)
                {

                    colourInterpolation.setTargetValue(0.0f);
                    stopTimer();

                }

                return;

            }

        }

    }
    else
    {

        if (thumbArea.getX() <= (animationStart.getX() + animationEnd.getX()) / 2)
        {

            animationAcc += ANIMATION_SPEED;

        }
        else
        {

            animationAcc -= ANIMATION_SPEED;

        }

        if (std::abs(animationVel) < 4)
        {

            animationVel += animationAcc;

        }

        thumbArea.translate(animationVel, 0);
        indicatorArea.setWidth((thumbArea.getX() - indicatorArea.getX()) + thumbArea.getWidth()/2);

        if (indicatorArea.getWidth() < 0)
        {

            indicatorArea.setWidth(0);

        }

        if (thumbArea.getPosition().getDistanceFrom(animationEnd) <= 4 || thumbArea.getX() < getWidth() / 4 || thumbArea.getX() > getWidth() / 2)
        {

            animationAcc = 0;
            animationVel = 0;

            thumbArea.setPosition(animationEnd);
            indicatorArea.setWidth((thumbArea.getX() - indicatorArea.getX()) + thumbArea.getWidth()/2);

            stopTimer();
            repaint();

        }

    }

	repaint();

}

void HackAudio::Button::paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown)
{

    int width  = getWidth();
    int height = getHeight();

    if (buttonStyle != ButtonStyle::SlidingToggle)
    {

        juce::Path p;
        p.addRoundedRectangle(0, 0, width, height, CORNER_RADIUS, CORNER_RADIUS, false, !(isConnectedOnTop() || isConnectedOnRight()), !(isConnectedOnBottom() || isConnectedOnLeft()), false);

        juce::Colour background;
        juce::Colour foreground;

        if (buttonStyle != ButtonStyle::Bar)
        {

            if (getToggleState())
            {

                background = findColour(HackAudio::highlightColourId);

                if (hasKeyboardFocus(true) && wasFocusedByTab)
                {

                    foreground = findColour(HackAudio::foregroundColourId);

                }
                else
                {

                    foreground = findColour(HackAudio::backgroundColourId);

                }

            }
            else
            {

                background = findColour(HackAudio::foregroundColourId);

                if (hasKeyboardFocus(true) && wasFocusedByTab)
                {

                    foreground = findColour(HackAudio::highlightColourId);

                }
                else
                {

                    foreground = findColour(HackAudio::backgroundColourId);
                    
                }

            }
        }
        else
        {

            background = findColour(HackAudio::foregroundColourId);

            if (hasKeyboardFocus(true) && wasFocusedByTab)
            {

                foreground = findColour(HackAudio::highlightColourId);

            }
            else
            {

                foreground = findColour(HackAudio::backgroundColourId);

            }

        }

        g.setColour(background.interpolatedWith(findColour(HackAudio::midgroundColourId), colourInterpolation.getNextValue()));
        g.fillPath(p);

        g.setColour(foreground);

        g.setFont(buttonFont);
        g.drawFittedText(getButtonText(), CORNER_RADIUS / 2, CORNER_RADIUS / 2, width - CORNER_RADIUS, height - CORNER_RADIUS, juce::Justification::centred, 1, 1.0f);

    }
    else
    {

        juce::Path p;
        p.addRoundedRectangle(0, 0, width, height, CORNER_CONFIG);
        g.setColour(findColour(HackAudio::midgroundColourId));
        g.fillPath(p);

        p.clear();

        p.addRoundedRectangle(trackArea.getX(), trackArea.getY(), trackArea.getWidth(), trackArea.getHeight(), 8, 8, false, true, true, false);
        g.setColour(findColour(HackAudio::backgroundColourId));
        g.fillPath(p);

        p.clear();

        p.addRoundedRectangle(indicatorArea.getX(), indicatorArea.getY(), indicatorArea.getWidth(), indicatorArea.getHeight(), 8, 8, false, true, true, false);
        g.setColour(findColour(HackAudio::highlightColourId));
        g.fillPath(p);

        p.clear();

        p.addRoundedRectangle(thumbArea.getX(), thumbArea.getY(), thumbArea.getWidth(), thumbArea.getHeight(), 8, 8, false, true, true, false);
        g.setColour(findColour(HackAudio::foregroundColourId));
        g.fillPath(p);

        g.setColour(findColour(HackAudio::midgroundColourId));
        int strokeWidth = std::min(thumbArea.getWidth(), thumbArea.getHeight()) / 4;
        g.strokePath(p, juce::PathStrokeType(strokeWidth));

    }

}

void HackAudio::Button::resized()
{

    int width  = getWidth();
    int height = getHeight();

    if (buttonStyle != ButtonStyle::SlidingToggle)
    {

        trackArea.setBounds(0, 0, 0, 0);
        indicatorArea.setBounds(0, 0, 0, 0);
        thumbArea.setBounds(0, 0, 0, 0);

    }
    else
    {

        trackArea.setBounds(width / 4, height / 4, width / 2, height / 2);

        thumbArea.setSize(width / 4, height / 2);
        indicatorArea.setPosition(width / 4, height / 4);
        indicatorArea.setHeight(height / 2);

        if (getToggleState())
        {

            thumbArea.setPosition(width / 2, height / 4);

        }
        else
        {

            thumbArea.setPosition(width / 4, height / 4);
            
        }

        indicatorArea.setWidth((thumbArea.getX() - indicatorArea.getX()) + thumbArea.getWidth() / 2);

    }

	resizeGuard = false;
    
}

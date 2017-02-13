/*
 GRNLR - a granular synthesis instrument
 Copyright (C) 2017  Raffael Seyfried
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PARAMETERGUICLASSES_H_INCLUDED
#define PARAMETERGUICLASSES_H_INCLUDED

class ParameterSlider : public Slider,
                        private Timer
{
public:
    // [1]
    ParameterSlider (AudioProcessorParameter& p) : Slider (p.getName (256)), param (p)
    {
        setRange (0.0, 1.0, 0.0);
        startTimerHz (60);
        updateSliderPos();
    }
    
    bool isDragging = false;
    
    // [2]
    void startedDragging() override     { param.beginChangeGesture(); isDragging = true;  }
    void stoppedDragging() override     { param.endChangeGesture();   isDragging = false; }
    
    // [3]
    void timerCallback() override       { updateSliderPos(); }
    
    // [4]
    void updateSliderPos()
    {
        const float newValue = param.getValue();
        
        if (newValue != (float) Slider::getValue())
            Slider::setValue (newValue);
    }
    // [5]
    void valueChanged() override
    {
        if(isDragging){
            param.setValueNotifyingHost((float) Slider::getValue());
        } else {
            param.setValue((float) Slider::getValue());
        }
        
    }
    
    double getValueFromText (const String& text) override   { return param.getValueForText (text); }
    String getTextFromValue (double value) override         { return param.getText ((float) value, 1024); }
    
    AudioProcessorParameter& param;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterSlider)
};

class ParameterButton : public TextButton,
                        private Timer
{
public:
    ParameterButton (AudioProcessorParameter& p) : TextButton (p.getName (256)), param (p)
    {
        updateButton();
        startTimerHz (60);
    }
    
    void timerCallback() override { updateButton(); }
    
    void updateButton()
    {
        const bool newValue = param.getValue();
        
        if (newValue != TextButton::getToggleState())
            TextButton::setToggleState(newValue, sendNotification);
    }
    
    void clicked() override
    {
        param.setValueNotifyingHost(TextButton::getToggleState());
    }

    AudioProcessorParameter& param;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterButton)
    
};



#endif  // PARAMETERGUICLASSES_H_INCLUDED

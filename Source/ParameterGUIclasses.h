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

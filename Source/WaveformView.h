#ifndef WAVEFORMVIEW_H_INCLUDED
#define WAVEFORMVIEW_H_INCLUDED

class  WaveformView : public Component,
                      public ChangeListener,
                      public FileDragAndDropTarget,
                      public ChangeBroadcaster,
                      private Timer
{
public:
    WaveformView ( AudioFormatManager& formatManager,
                   GrnlrAudioProcessor& p ) : thumbnailCache (5),
    thumbnail (512, formatManager, thumbnailCache),
    processor(p)
    {
        thumbnail.addChangeListener(this);
        
        positionMarker.setFill(Colours::white.withAlpha(0.7f));
        addAndMakeVisible(positionMarker);
    }
    
    ~WaveformView()
    {
        thumbnail.removeChangeListener(this);
    }
    
    
    void setFile (const File& file)
    {
        if (! file.isDirectory())
        {
            thumbnail.setSource (new FileInputSource (file));
            const Range<double> newRange (0.0, thumbnail.getTotalLength());
            setRange (newRange);
            
            startTimerHz (40);
        }
    }
    
    File getLastDroppedFile() const noexcept
    {
        return droppedFile;
    }
    
    void setZoomFactor (double amount)
    {
        if (thumbnail.getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
            setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
        }
    }
    
    void setRange (Range<double> newRange)
    {
        visibleRange = newRange;
        updateCursorPosition(0.5);
        repaint();
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
        g.setColour (Colours::lightgreen);
        
        if (thumbnail.getTotalLength() > 0.0)
        {
            Rectangle<int> thumbArea (getLocalBounds());
            thumbnail.drawChannels (g, thumbArea.reduced (2),
                                    visibleRange.getStart(), visibleRange.getEnd(), 1.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(drop sample!)", getLocalBounds(), Justification::centred, 2);
        }
    }
    
    void changeListenerCallback (ChangeBroadcaster*) override
    {
        repaint();
    }
    
    bool isInterestedInFileDrag (const StringArray& /*files*/) override
    {
        return true;
    }
    
    void filesDropped (const StringArray& files, int /*x*/, int /*y*/) override
    {
        droppedFile = File (files[0]);
        sendChangeMessage();
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        mouseDrag (e);
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        // set the grain startPosition from here
    }
    
    void mouseUp (const MouseEvent&) override
    {
        
    }
    
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel) override
    {
        
    }
    
private:
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    File droppedFile;
    
    Range<double> visibleRange;
    bool isFollowingTransport;
    
    DrawableRectangle positionMarker;
    
    GrnlrAudioProcessor& processor;
    
    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - visibleRange.getStart()) / (visibleRange.getLength()));
    }
    
    double xToTime (const float x) const
    {
        return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
    }
    
    void timerCallback() override
    {
        updateCursorPosition(0.5);
    }
    
    void updateCursorPosition(float position)
    {
        positionMarker.setVisible (isMouseButtonDown());
        
        positionMarker.setRectangle ( Rectangle<float> (timeToX (position) - 0.75f, 0,
                                                        1.5f, (float) (getHeight() ))) ;
    }
};

#endif  // WAVEFORMVIEW_H_INCLUDED

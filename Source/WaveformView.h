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

#ifndef WAVEFORMVIEW_H_INCLUDED
#define WAVEFORMVIEW_H_INCLUDED

class  WaveformView :   public Component,
                        public ChangeListener,
                        public FileDragAndDropTarget,
                        public ChangeBroadcaster
{
public:
    WaveformView (AudioFormatManager& formatManager) : thumbnailCache (5),
                  thumbnail (512, formatManager, thumbnailCache)
    {
        thumbnail.addChangeListener(this);
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
    
    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - visibleRange.getStart()) / (visibleRange.getLength()));
    }
    
    double xToTime (const float x) const
    {
        return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
    }
    
    Range<double> visibleRange;
    bool isFollowingTransport;
};

#endif // WAVEFORMVIEW_H_INCLUDED

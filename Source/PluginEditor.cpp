#include "PluginEditor.h"

namespace
{
const auto ink = juce::Colour (0xff090a0a);
const auto white = juce::Colour (0xffe8ece9);
const auto muted = juce::Colour (0xff929b95);
const auto green = juce::Colour (0xff60ff8b);
juce::Point<float> radial (juce::Point<float> c, float r, float a)
{
    return { c.x + r * std::sin (a), c.y - r * std::cos (a) };
}
void line (juce::Graphics& g, juce::Point<float> a, juce::Point<float> b, float thickness)
{
    g.drawLine ({ a, b }, thickness);
}
}

VoxLookAndFeel::VoxLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, white);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff353a37));
}

void VoxLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                       float position, float start, float end, juce::Slider&)
{
    const juce::Point<float> c (x + width * 0.5f, y + height * 0.5f);
    const float r = juce::jmin (width, height) * 0.5f - 25.0f;
    for (int i = 0; i <= 40; ++i)
    {
        const float a = start + (end - start) * static_cast<float> (i) / 40.0f;
        g.setColour (i <= position * 40.0f ? green.withAlpha (0.8f) : muted);
        line (g, radial (c, r + 7, a), radial (c, r + (i % 10 == 0 ? 16 : 11), a), 1.0f);
    }
    g.setFont (juce::FontOptions (10.0f));
    for (int i = 0; i <= 4; ++i)
    {
        auto p = radial (c, r + 23, start + (end - start) * static_cast<float> (i) / 4.0f);
        g.setColour (muted);
        g.drawText (juce::String (i * 25), juce::Rectangle<float> (p.x - 15, p.y - 7, 30, 14), juce::Justification::centred);
    }
    auto disc = juce::Rectangle<float> (c.x - r, c.y - r, r * 2, r * 2);
    g.setColour (juce::Colours::black.withAlpha (0.8f));
    g.fillEllipse (disc.expanded (3).translated (0, 5));
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff707672), c.x-r, c.y-r,
                                            juce::Colour (0xff151816), c.x+r, c.y+r, false));
    g.fillEllipse (disc);
    for (int i = 0; i < 120; ++i)
    {
        float a = juce::MathConstants<float>::twoPi * static_cast<float> (i) / 120.0f;
        g.setColour (i % 2 == 0 ? juce::Colour (0xff919792) : juce::Colour (0xff181a19));
        line (g, radial (c, r * 0.91f, a), radial (c, r * 0.99f, a), 1.0f);
    }
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff484d49), c.x-r, c.y-r,
                                            juce::Colour (0xff090b0a), c.x+r, c.y+r, false));
    g.fillEllipse (disc.reduced (r * 0.12f));
    for (float rr = 3; rr < r * 0.87f; rr += 1.5f)
    {
        g.setColour (juce::Colours::white.withAlpha (0.035f));
        g.drawEllipse (c.x-rr, c.y-rr, rr*2, rr*2, 0.5f);
    }
    g.setColour (juce::Colour (0xffb9c0ba));
    g.drawEllipse (disc.reduced (r * 0.1f), 1.4f);
    const float a = start + position * (end-start);
    g.setColour (white);
    line (g, radial (c, r * 0.40f, a), radial (c, r * 0.78f, a), 4.0f);
}

void VoxLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                       float pos, float, float, juce::Slider::SliderStyle, juce::Slider& s)
{
    bool vertical = s.isVertical();
    juce::Point<float> a (vertical ? x + width*0.5f : static_cast<float> (x), vertical ? static_cast<float> (y+height) : y+height*0.5f);
    juce::Point<float> b (vertical ? a.x : static_cast<float> (x+width), vertical ? static_cast<float> (y) : a.y);
    juce::Point<float> p (vertical ? a.x : pos, vertical ? pos : a.y);
    g.setColour (juce::Colour (0xff424843)); line (g,a,b,12);
    g.setColour (juce::Colours::black); line (g,a,b,9);
    g.setColour (green.withAlpha (0.18f)); line (g,a,p,7);
    g.setColour (green); line (g,a,p,3);
    for (int i = 0; i <= 10; ++i)
    {
        auto tick = a + (b-a) * (static_cast<float> (i)/10.0f);
        g.setColour (muted.withAlpha (0.55f));
        if (vertical) line (g, {tick.x-19,tick.y}, {tick.x-13,tick.y},1);
        else line (g, {tick.x,tick.y+13}, {tick.x,tick.y+19},1);
    }
    auto cap = juce::Rectangle<float> (vertical ? 22.0f : 20.0f, vertical ? 34.0f : 20.0f).withCentre (p);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff8b928c),cap.getTopLeft(),juce::Colour (0xff242925),cap.getBottomRight(),false));
    g.fillRoundedRectangle (cap,4);
    g.setColour (muted); g.drawRoundedRectangle (cap,4,1);
    g.setColour (green);
    if (vertical) g.fillRoundedRectangle (cap.getX()+2,p.y-2,cap.getWidth()-4,4,1);
    else g.fillEllipse (p.x-3,p.y-3,6,6);
}

WaveformDisplay::WaveformDisplay (VoxVibratoAudioProcessor& p) : processor (p) { startTimerHz (30); }

void WaveformDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (1);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff454c46),0,0,juce::Colour (0xff111512),0,bounds.getBottom(),false));
    g.fillRoundedRectangle (bounds,16);
    g.setColour (juce::Colours::black); g.fillRoundedRectangle (bounds.reduced (3),14);
    auto screen = bounds.reduced (8);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff071a0d),screen.getCentre(),juce::Colour (0xff020604),screen.getTopLeft(),true));
    g.fillRoundedRectangle (screen,10);
    auto graph = screen.reduced (18,36);
    graph.removeFromRight (40);
    g.setColour (green.withAlpha (0.21f));
    for (int i=0;i<=10;++i)
        for (int j=0;j<=40;++j)
            g.fillEllipse (graph.getX()+graph.getWidth()*i/10.0f-0.6f,graph.getY()+graph.getHeight()*j/40.0f-0.6f,1.2f,1.2f);
    for (int j=0;j<=4;++j)
        for (int i=0;i<=100;++i)
            g.fillEllipse (graph.getX()+graph.getWidth()*i/100.0f-0.6f,graph.getY()+graph.getHeight()*j/4.0f-0.6f,1.2f,1.2f);
    const float rate = processor.parameters.getRawParameterValue (VoxVibratoAudioProcessor::rateId)->load();
    const float intensity = processor.parameters.getRawParameterValue (VoxVibratoAudioProcessor::intensityId)->load();
    const float cents = processor.parameters.getRawParameterValue (VoxVibratoAudioProcessor::centsId)->load();
    // One second of modulation; the vertical full scale follows the Peak Shift setting.
    const float amplitude = cents > 0 && rate > 0 ? graph.getHeight()*0.5f*intensity/100.0f : 0;
    juce::Path wave;
    for (int i=0;i<=800;++i)
    {
        float t = static_cast<float> (i)/800.0f;
        float xx = graph.getX()+t*graph.getWidth();
        float yy = graph.getCentreY()-amplitude*std::sin (juce::MathConstants<float>::twoPi*rate*t);
        if (i==0) wave.startNewSubPath (xx,yy); else wave.lineTo (xx,yy);
    }
    for (int i=4;i>=1;--i)
    {
        g.setColour (green.withAlpha (0.035f*static_cast<float> (5-i)));
        g.strokePath (wave,juce::PathStrokeType (static_cast<float> (i)*3));
    }
    g.setColour (green); g.strokePath (wave,juce::PathStrokeType (1.8f));
    g.setFont (juce::FontOptions (11)); g.setColour (green.withAlpha (0.8f));
    g.drawText ("MODULATION",screen.getX()+16,screen.getY()+9,110,18,juce::Justification::centredLeft);
    g.drawText ("+/- " + juce::String (rate > 0 ? cents*intensity/100.0f : 0,1)+" cents",screen.getRight()-150,screen.getY()+9,134,18,juce::Justification::centredRight);
    g.drawText ("+"+juce::String (cents,0),graph.getRight()+6,graph.getY()-7,36,14,juce::Justification::centredLeft);
    g.drawText ("0",graph.getRight()+6,graph.getCentreY()-7,36,14,juce::Justification::centredLeft);
    g.drawText ("-"+juce::String (cents,0),graph.getRight()+6,graph.getBottom()-7,36,14,juce::Justification::centredLeft);
    g.setColour (muted); g.setFont (juce::FontOptions (10));
    g.drawText ("0 s",graph.getX(),screen.getBottom()-25,40,16,juce::Justification::centredLeft);
    g.drawText ("1.0 s",graph.getRight()-40,screen.getBottom()-25,40,16,juce::Justification::centredRight);
}

VoxVibratoAudioProcessorEditor::VoxVibratoAudioProcessorEditor (VoxVibratoAudioProcessor& p)
    : AudioProcessorEditor (&p), waveform (p)
{
    setLookAndFeel (&lookAndFeel);
    setResizable (true,true); setResizeLimits (760,500,1200,800);
    addAndMakeVisible (waveform);
    configureSlider (rateSlider,rateLabel,"RATE"," Hz");
    configureSlider (intensitySlider,intensityLabel,"INTENSITY"," %");
    configureSlider (centsSlider,centsLabel,"PEAK SHIFT"," cents");
    rateSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    intensitySlider.setSliderStyle (juce::Slider::LinearVertical);
    centsSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    centsSlider.setRotaryParameters (juce::MathConstants<float>::pi*1.25f,juce::MathConstants<float>::pi*2.75f,true);
    rateAttachment = std::make_unique<SliderAttachment> (p.parameters,VoxVibratoAudioProcessor::rateId,rateSlider);
    intensityAttachment = std::make_unique<SliderAttachment> (p.parameters,VoxVibratoAudioProcessor::intensityId,intensitySlider);
    centsAttachment = std::make_unique<SliderAttachment> (p.parameters,VoxVibratoAudioProcessor::centsId,centsSlider);
    website.setColour (juce::HyperlinkButton::textColourId,muted);
    website.setTooltip ("Open the Vox Vibrato website (coming soon)");
    addAndMakeVisible (website);
    setSize (900,560);
}
VoxVibratoAudioProcessorEditor::~VoxVibratoAudioProcessorEditor() { setLookAndFeel (nullptr); }

void VoxVibratoAudioProcessorEditor::configureSlider (juce::Slider& s,juce::Label& label,const juce::String& title,const juce::String& suffix)
{
    s.setTextBoxStyle (juce::Slider::TextBoxBelow,false,110,28);
    s.setTextValueSuffix (suffix);
    s.setDoubleClickReturnValue (true,title=="RATE"?5.0:title=="INTENSITY"?50.0:20.0);
    s.setTitle (title); s.setTooltip (title+": drag to adjust, double-click to reset; click value to type.");
    addAndMakeVisible (s);
    label.setText (title,juce::dontSendNotification); label.setColour (juce::Label::textColourId,white);
    label.setFont (juce::FontOptions (13).withStyle ("Bold")); label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (label);
}

void VoxVibratoAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (ink);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff202321),0,0,ink,getWidth()*0.7f,getHeight(),false));
    g.fillRoundedRectangle (getLocalBounds().toFloat().reduced (4),10);
    g.setColour (juce::Colour (0xff515752)); g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (4),10,1);
    for (auto p : {juce::Point<float>(18,18),juce::Point<float>(getWidth()-18.0f,18),juce::Point<float>(18,getHeight()-18.0f),juce::Point<float>(getWidth()-18.0f,getHeight()-18.0f)})
    {
        g.setColour (juce::Colour (0xff535a54)); g.fillEllipse (p.x-6,p.y-6,12,12);
        g.setColour (juce::Colours::black); g.fillEllipse (p.x-5,p.y-5,10,10);
        g.setColour (juce::Colour (0xff737a74)); line (g,{p.x-3,p.y+2},{p.x+3,p.y-2},1);
    }
    g.setColour (white); g.setFont (juce::FontOptions (30).withStyle ("Bold"));
    g.drawText ("VOX VIBRATO",48,25,400,40,juce::Justification::centredLeft);
    g.setColour (muted); g.setFont (juce::FontOptions (15));
    g.drawText ("Vocal pitch vibrato",50,63,400,24,juce::Justification::centredLeft);
    g.setColour (juce::Colour (0xff343a35));
    g.drawHorizontalLine (getHeight()-44,5.0f,getWidth()-5.0f);
    g.drawVerticalLine (getWidth()-215,112.0f,getHeight()-65.0f);
}

void VoxVibratoAudioProcessorEditor::resized()
{
    const int scopeX=124, scopeY=110, scopeW=getWidth()-354, scopeH=getHeight()-250;
    waveform.setBounds (scopeX,scopeY,scopeW,scopeH);
    intensityLabel.setBounds (18,scopeY+2,102,24);
    intensitySlider.setBounds (20,scopeY+38,100,scopeH-38);
    centsLabel.setBounds (getWidth()-208,scopeY+16,190,24);
    centsSlider.setBounds (getWidth()-208,scopeY+53,190,scopeH-53);
    rateLabel.setBounds (scopeX,scopeY+scopeH+20,50,24);
    rateSlider.setBounds (scopeX+60,scopeY+scopeH+12,scopeW-70,83);
    website.setBounds (getWidth()/2-130,getHeight()-37,260,28);
}

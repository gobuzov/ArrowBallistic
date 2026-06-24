#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <vector>
#include "ArrowBallistic.h"

//==============================================================================
// Панель для отображения графика (аналог ArrowBallisticPanel)
class BallisticPanel : public juce::Component {
public:
    BallisticPanel();
    ~BallisticPanel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    void addTrajectory(const ArrowTrajectory& trajectory);
    void clearTrajectories();

private:
    std::vector<ArrowTrajectory> trajectories;
    int width, height;
    double scaleX, scaleY;

    void calculateScaling();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallisticPanel)
};
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_events/juce_events.h>
#include <memory>
#include "BallisticPanel.h"
#include "ArrowBallistic.h"

//==============================================================================
// Основной компонент приложения (аналог ArrowBallisticFrame)
class MainComponent : public juce::Component,
                      public juce::Button::Listener {
public:
    MainComponent();
    ~MainComponent() override = default;

    void resized() override;
    void buttonClicked(juce::Button* button) override;

private:
    // UI элементы
    std::unique_ptr<juce::GroupComponent> paramsGroup;

    std::unique_ptr<juce::Label> massLabel;
    std::unique_ptr<juce::TextEditor> massInput;
    std::unique_ptr<juce::Label> diamLabel;
    std::unique_ptr<juce::TextEditor> diamInput;
    std::unique_ptr<juce::Label> rezistLabel;
    std::unique_ptr<juce::TextEditor> rezistInput;
    std::unique_ptr<juce::Label> v0Label;
    std::unique_ptr<juce::TextEditor> v0Input;
    std::unique_ptr<juce::Label> pitchLabel;
    std::unique_ptr<juce::TextEditor> pitchInput;
    std::unique_ptr<juce::Label> yawLabel;
    std::unique_ptr<juce::TextEditor> yawInput;
    std::unique_ptr<juce::Label> windSpeedLabel;
    std::unique_ptr<juce::TextEditor> windSpeedInput;
    std::unique_ptr<juce::Label> windAngleLabel;
    std::unique_ptr<juce::TextEditor> windAngleInput;

    std::unique_ptr<juce::TextButton> startButton;
    std::unique_ptr<juce::TextButton> clearButton;

    std::unique_ptr<BallisticPanel> ballisticPanel;

    // Методы
    void fireSimulation();
    double getDoubleValue(juce::TextEditor* input, double defaultValue = 0.0);
    BallisticCalculator::Params getParamsFromUI();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
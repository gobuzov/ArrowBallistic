#include "MainComponent.h"

//==============================================================================
// Реализация MainComponent
MainComponent::MainComponent() {
    paramsGroup = std::make_unique<juce::GroupComponent>("params",
                        juce::String::fromUTF8("Параметры стрельбы"));
    addAndMakeVisible(paramsGroup.get());

    auto createLabeledInput = [&](const char* labelText,
                                  const juce::String& defaultValue,
                                  std::unique_ptr<juce::Label>& label,
                                  std::unique_ptr<juce::TextEditor>& input) {
        label = std::make_unique<juce::Label>();
        label->setText(juce::String::fromUTF8(labelText), juce::dontSendNotification);
        label->setFont(juce::FontOptions(12.0f));
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label.get());

        input = std::make_unique<juce::TextEditor>();
        input->setText(defaultValue);
        input->setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
        input->setColour(juce::TextEditor::textColourId, juce::Colours::white);
        addAndMakeVisible(input.get());
    };

    createLabeledInput("Масса стрелы в кг (25 грамм)", "0.025", massLabel, massInput);
    createLabeledInput("Диаметр стрелы в м (6 мм)", "0.006", diamLabel, diamInput);
    createLabeledInput("Коэффициент сопротивления (0.4)", "0.4", rezistLabel, rezistInput);
    createLabeledInput("Начальная скорость в м/с", "85.0", v0Label, v0Input);
    createLabeledInput("Угол наклона к горизонту", "10.0", pitchLabel, pitchInput);
    createLabeledInput("Азимут выстрела (прямо=0)", "0.0", yawLabel, yawInput);
    createLabeledInput("Скорость ветра в м/с", "5.0", windSpeedLabel, windSpeedInput);
    createLabeledInput("Направление ветра, 90 боковой", "90.0", windAngleLabel, windAngleInput);

    startButton = std::make_unique<juce::TextButton>(juce::String::fromUTF8("Старт"));
    startButton->addListener(this);
    startButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    addAndMakeVisible(startButton.get());

    clearButton = std::make_unique<juce::TextButton>(juce::String::fromUTF8("Очистить"));
    clearButton->addListener(this);
    clearButton->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    addAndMakeVisible(clearButton.get());

    ballisticPanel = std::make_unique<BallisticPanel>();
    addAndMakeVisible(ballisticPanel.get());

    setSize(900, 600);
}

void MainComponent::resized() {
    auto bounds = getLocalBounds();
    auto leftPanel = bounds.removeFromLeft(250);
    paramsGroup->setBounds(leftPanel);

    auto innerBounds = leftPanel.reduced(10, 20);
    int spacing = 5;
    int currentY = innerBounds.getY();

    auto createRow = [&](juce::Label* label, juce::TextEditor* input, int height) {
        label->setBounds(innerBounds.getX(), currentY, innerBounds.getWidth() - 10, 16);
        input->setBounds(innerBounds.getX(), currentY + 18, innerBounds.getWidth() - 10, height);
        currentY += height + 18 + spacing;
    };

    createRow(massLabel.get(), massInput.get(), 22);
    createRow(diamLabel.get(), diamInput.get(), 22);
    createRow(rezistLabel.get(), rezistInput.get(), 22);
    createRow(v0Label.get(), v0Input.get(), 22);
    createRow(pitchLabel.get(), pitchInput.get(), 22);
    createRow(yawLabel.get(), yawInput.get(), 22);
    createRow(windSpeedLabel.get(), windSpeedInput.get(), 22);
    createRow(windAngleLabel.get(), windAngleInput.get(), 22);

    int buttonWidth = (innerBounds.getWidth() - 10) / 2;
    startButton->setBounds(innerBounds.getX(), currentY + 5, buttonWidth, 30);
    clearButton->setBounds(innerBounds.getX() + buttonWidth + 10, currentY + 5, buttonWidth, 30);

    ballisticPanel->setBounds(bounds);
}

void MainComponent::buttonClicked(juce::Button* button) {
    if (button == startButton.get()) {
        fireSimulation();
    } else if (button == clearButton.get()) {
        ballisticPanel->clearTrajectories();
    }
}

double MainComponent::getDoubleValue(juce::TextEditor* input, double defaultValue) {
    juce::String text = input->getText();
    if (text.isEmpty()) return defaultValue;
    return text.getDoubleValue();
}

BallisticCalculator::Params MainComponent::getParamsFromUI() {
    BallisticCalculator::Params params;
    params.mass = getDoubleValue(massInput.get(), 0.025);
    params.diameter = getDoubleValue(diamInput.get(), 0.006);
    params.Cd = getDoubleValue(rezistInput.get(), 0.4);
    params.v0 = getDoubleValue(v0Input.get(), 85.0);
    params.pitch = getDoubleValue(pitchInput.get(), 10.0);
    params.yaw = getDoubleValue(yawInput.get(), 0.0);
    params.windSpeed = getDoubleValue(windSpeedInput.get(), 5.0);
    params.windAngle = getDoubleValue(windAngleInput.get(), 90.0);
    return params;
}

void MainComponent::fireSimulation() {
    auto params = getParamsFromUI();
    auto result = BallisticCalculator::calculate(params);

    // Вывод результатов в консоль
    juce::String output;
    output << "==================================================\n";
    output << juce::String::fromUTF8("Стрела упала на землю:\n");
    output << juce::String::fromUTF8("Итоговая дальность (X): ")
           << juce::String(result.distance, 2) << juce::String::fromUTF8(" м\n");
    output << juce::String::fromUTF8("Боковой снос ветром (Z): ")
           << juce::String(result.drift, 2) << juce::String::fromUTF8(" м\n");
    output << juce::String::fromUTF8("Полное время полета: ")
           << juce::String(result.time, 2) << juce::String::fromUTF8(" с\n");
    output << "==================================================\n";

    juce::Logger::outputDebugString(output);

    ballisticPanel->addTrajectory(result.trajectory);
}
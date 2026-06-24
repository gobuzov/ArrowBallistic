#include "BallisticPanel.h"

//==============================================================================
// Реализация BallisticPanel
BallisticPanel::BallisticPanel()
    : width(600), height(400), scaleX(1.0), scaleY(1.0) {
    setOpaque(true);
}

void BallisticPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);

    if (trajectories.empty()) {
        g.setColour(juce::Colours::grey);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText(juce::String::fromUTF8("Нажмите <Старт> для запуска симуляции"),
                   getLocalBounds(), juce::Justification::centred);
        return;
    }

    calculateScaling();

    for (const auto& trajectory : trajectories) {
        if (trajectory.size() < 2) continue;

        juce::Path path;
        const auto& firstPoint = trajectory.getPoint(0);
        float startX = 10.0f + (float)(firstPoint.x * scaleX);
        float startY = (float)(height / 2.0f - firstPoint.y * scaleY);
        path.startNewSubPath(startX, startY);

        g.setColour(juce::Colours::orange);

        for (int i = 1; i < trajectory.size(); ++i) {
            const auto& point = trajectory.getPoint(i);
            float x = 10.0f + (float)(point.x * scaleX);
            float y = (float)(height / 2.0f - point.y * scaleY);
            path.lineTo(x, y);
        }

        g.strokePath(path, juce::PathStrokeType(2.0f));

        if (trajectory.size() > 0) {
            int midIndex = trajectory.size() / 2;
            if (midIndex < trajectory.size()) {
                const auto& midPoint = trajectory.getPoint(midIndex);
                float x = 10.0f + (float)(midPoint.x * scaleX);
                float y = (float)(height / 2.0f - midPoint.y * scaleY);

                g.setColour(juce::Colours::green);
                g.setFont(juce::FontOptions(12.0f));
                g.drawText(trajectory.description,
                          juce::Rectangle<float>(x, y - 10.0f, 200.0f, 20.0f),
                          juce::Justification::centredLeft);
            }
        }
    }
}

void BallisticPanel::resized() {
    width = getWidth();
    height = getHeight();
    calculateScaling();
}

void BallisticPanel::mouseDown(const juce::MouseEvent& event) {
    juce::String debugMsg = "click x " + juce::String(event.x) +
                           " y " + juce::String(event.y);
    juce::Logger::outputDebugString(debugMsg);
}

void BallisticPanel::addTrajectory(const ArrowTrajectory& trajectory) {
    trajectories.push_back(trajectory);
    repaint();
}

void BallisticPanel::clearTrajectories() {
    trajectories.clear();
    repaint();
}

void BallisticPanel::calculateScaling() {
    double maxX = 0, maxY = 0;

    for (const auto& trajectory : trajectories) {
        for (int i = 0; i < trajectory.size(); ++i) {
            const auto& point = trajectory.getPoint(i);
            maxX = std::max(maxX, point.x);
            maxY = std::max(maxY, point.y);
        }
    }

    if (maxX > 0 && maxY > 0) {
        double paddingX = 20.0;
        double paddingY = 20.0;
        scaleX = (static_cast<double>(width) - paddingX * 2) / (maxX + 10);
        scaleY = (static_cast<double>(height) - paddingY * 2) / (maxY + 2);

        double minScale = std::min(scaleX, scaleY);
        scaleX = minScale;
        scaleY = minScale;
    }
}
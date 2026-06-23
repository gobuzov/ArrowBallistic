#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_events/juce_events.h>
#include <vector>
#include <cmath>

//==============================================================================
// Структура для хранения точки траектории
struct ArrowPoint {
    double x, y, z;      // Координаты
    double velocity;     // Скорость

    ArrowPoint(double _x = 0, double _y = 0, double _z = 0, double _v = 0)
        : x(_x), y(_y), z(_z), velocity(_v) {}
};

//==============================================================================
// Класс для хранения одной траектории
class ArrowTrajectory {
public:
    std::vector<ArrowPoint> points;
    juce::String description;

    ArrowTrajectory() = default;
    ArrowTrajectory(const juce::String& desc) : description(desc) {}

    // Разрешаем копирование и перемещение
    ArrowTrajectory(const ArrowTrajectory& other) = default;
    ArrowTrajectory(ArrowTrajectory&& other) = default;
    ArrowTrajectory& operator=(const ArrowTrajectory& other) = default;
    ArrowTrajectory& operator=(ArrowTrajectory&& other) = default;

    ~ArrowTrajectory() = default;

    void addPoint(const ArrowPoint& point) {
        points.push_back(point);
    }

    void clear() {
        points.clear();
    }

    int size() const { return (int)points.size(); }
    const ArrowPoint& getPoint(int index) const {
        return points[static_cast<size_t>(index)];
    }
};

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

    // Масштабирование координат для отображения
    double scaleX, scaleY;

    void calculateScaling();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallisticPanel)
};

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
    // Физические константы
    static constexpr double G = 9.80665;
    static constexpr double RHO = 1.225;

    // UI элементы (аналог левой панели с параметрами)
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

    // Панель для отображения графика (в центре)
    std::unique_ptr<BallisticPanel> ballisticPanel;

    // Логика симуляции
    void fireSimulation();

    // Вспомогательные методы для получения данных из полей ввода
    double getDoubleValue(juce::TextEditor* input, double defaultValue = 0.0);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
#include "MainComponent.h"

//==============================================================================
// Реализация BallisticPanel
BallisticPanel::BallisticPanel()
    : width(600), height(400), scaleX(1.0), scaleY(1.0) {
    setOpaque(true);
}

void BallisticPanel::paint(juce::Graphics& g) {
    // Черный фон (как в Java версии)
    g.fillAll(juce::Colours::black);

    if (trajectories.empty()) {
        g.setColour(juce::Colours::grey);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText(juce::String::fromUTF8("Нажмите <Старт> для запуска симуляции"),
                   getLocalBounds(), juce::Justification::centred);
        return;
    }

    calculateScaling();

    // Рисуем все траектории
    for (const auto& trajectory : trajectories) {
        if (trajectory.size() < 2) continue;

        // Первая точка - оранжевая линия (как в Java)
        juce::Path path;
        const auto& firstPoint = trajectory.getPoint(0);
        float startX = 10.0f + (float)(firstPoint.x * scaleX);
        float startY = (float)(height / 2.0f - firstPoint.y * scaleY);
        path.startNewSubPath(startX, startY);

        // Рисуем траекторию оранжевым цветом
        g.setColour(juce::Colours::orange);

        for (int i = 1; i < trajectory.size(); ++i) {
            const auto& point = trajectory.getPoint(i);
            float x = 10.0f + (float)(point.x * scaleX);
            float y = (float)(height / 2.0f - point.y * scaleY);
            path.lineTo(x, y);
        }

        g.strokePath(path, juce::PathStrokeType(2.0f));

        // Рисуем описание траектории зеленым цветом (как в Java)
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
    // Находим максимальные значения по всем траекториям
    double maxX = 0, maxY = 0;

    for (const auto& trajectory : trajectories) {
        for (int i = 0; i < trajectory.size(); ++i) {
            const auto& point = trajectory.getPoint(i);
            maxX = std::max(maxX, point.x);
            maxY = std::max(maxY, point.y);
        }
    }

    // Добавляем отступы и вычисляем масштаб
    if (maxX > 0 && maxY > 0) {
        double paddingX = 20.0;
        double paddingY = 20.0;
        scaleX = (static_cast<double>(width) - paddingX * 2) / (maxX + 10);
        scaleY = (static_cast<double>(height) - paddingY * 2) / (maxY + 2);

        // Используем одинаковый масштаб для сохранения пропорций
        double minScale = std::min(scaleX, scaleY);
        scaleX = minScale;
        scaleY = minScale;
    }
}

//==============================================================================
// Реализация MainComponent
MainComponent::MainComponent() {
    // Создаем группу параметров
    paramsGroup = std::make_unique<juce::GroupComponent>("params", juce::String::fromUTF8("Параметры стрельбы"));
    addAndMakeVisible(paramsGroup.get());

    // Создаем поля ввода и метки
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

    // Создаем кнопки
    startButton = std::make_unique<juce::TextButton>(juce::String::fromUTF8("Старт"));
    startButton->addListener(this);
    startButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    addAndMakeVisible(startButton.get());

    clearButton = std::make_unique<juce::TextButton>(juce::String::fromUTF8("Очистить"));
    clearButton->addListener(this);
    clearButton->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    addAndMakeVisible(clearButton.get());

    // Создаем панель для графиков (центр)
    ballisticPanel = std::make_unique<BallisticPanel>();
    addAndMakeVisible(ballisticPanel.get());

    setSize(900, 600);
}

void MainComponent::resized() {
    auto bounds = getLocalBounds();

    // Левая панель с параметрами (ширина 250 пикселей)
    auto leftPanel = bounds.removeFromLeft(250);
    paramsGroup->setBounds(leftPanel);

    // Отступы внутри группы
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

    // Кнопки
    int buttonWidth = (innerBounds.getWidth() - 10) / 2;
    startButton->setBounds(innerBounds.getX(), currentY + 5, buttonWidth, 30);
    clearButton->setBounds(innerBounds.getX() + buttonWidth + 10, currentY + 5, buttonWidth, 30);

    // Центральная панель с графиком
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

void MainComponent::fireSimulation() {
    // --- 1. ВХОДНЫЕ ДАННЫЕ МОДЕЛИ ---
    double m = getDoubleValue(massInput.get(), 0.025);
    double diameter = getDoubleValue(diamInput.get(), 0.006);
    double Cd = getDoubleValue(rezistInput.get(), 0.4);
    double v0 = getDoubleValue(v0Input.get(), 85.0);

    double pitch = juce::MathConstants<double>::pi * getDoubleValue(pitchInput.get(), 10.0) / 180.0;
    double yaw = juce::MathConstants<double>::pi * getDoubleValue(yawInput.get(), 0.0) / 180.0;

    double windSpeed = getDoubleValue(windSpeedInput.get(), 5.0);
    double windAngle = juce::MathConstants<double>::pi * getDoubleValue(windAngleInput.get(), 90.0) / 180.0;

    // --- 2. ПОДГОТОВКА ПАРАМЕТРОВ ---
    double area = juce::MathConstants<double>::pi * std::pow(diameter / 2.0, 2);
    double k = (RHO * Cd * area) / (2.0 * m);

    double wx = windSpeed * std::cos(windAngle);
    double wz = windSpeed * std::sin(windAngle);

    double x = 0.0;
    double y = 1.5;
    double z = 0.0;

    juce::String description = "pit:" + juce::String((int)(pitch * 180.0 / juce::MathConstants<double>::pi)) +
                              " m:" + juce::String(m) +
                              " v:" + juce::String((int)v0);
    ArrowTrajectory trajectory(description);

    double vx = v0 * std::cos(pitch) * std::cos(yaw);
    double vy = v0 * std::sin(pitch);
    double vz = v0 * std::cos(pitch) * std::sin(yaw);

    double t = 0.0;
    double dt = 0.001;
    double vInit = std::sqrt(vx * vx + vy * vy + vz * vz);
    trajectory.addPoint(ArrowPoint(x, y, z, vInit));

    // --- 3. ЦИКЛ ЧИСЛЕННОГО ИНТЕГРИРОВАНИЯ ---
    int maxSteps = 1000000;
    int stepCount = 0;
    int logInterval = (int)(0.1 / dt);

    double xPrev = x, yPrev = y, zPrev = z, tPrev = t;

    while (y >= 0.0 && stepCount < maxSteps) {
        xPrev = x;
        yPrev = y;
        zPrev = z;
        tPrev = t;

        double vRelX = vx - wx;
        double vRelY = vy;
        double vRelZ = vz - wz;
        double vRel = std::sqrt(vRelX * vRelX + vRelY * vRelY + vRelZ * vRelZ);

        double ax = -k * vRel * vRelX;
        double ay = -G - k * vRel * vRelY;
        double az = -k * vRel * vRelZ;

        x += vx * dt;
        y += vy * dt;
        z += vz * dt;

        vx += ax * dt;
        vy += ay * dt;
        vz += az * dt;

        t += dt;
        stepCount++;

        if (stepCount % logInterval == 0) {
            double vCurrent = std::sqrt(vx * vx + vy * vy + vz * vz);
            trajectory.addPoint(ArrowPoint(x, y, z, vCurrent));
        }
    }

    // --- 4. ИНТЕРПОЛЯЦИЯ ТОЧКИ ПАДЕНИЯ ---
    double xImpact, zImpact, tImpact;

    if (y < 0.0 && stepCount > 0) {
        double frac = yPrev / (yPrev - y);
        tImpact = tPrev + frac * dt;
        xImpact = xPrev + frac * (x - xPrev);
        zImpact = zPrev + frac * (z - zPrev);
    } else {
        tImpact = t;
        xImpact = x;
        zImpact = z;
    }

    trajectory.addPoint(ArrowPoint(xImpact, 0, zImpact, 0));

    // Вывод результатов в консоль
    juce::String result;
    result << "==================================================\n";
    result << juce::String::fromUTF8("Стрела упала на землю:\n");
    result << juce::String::fromUTF8("Итоговая дальность (X): ") << juce::String(xImpact, 2) << juce::String::fromUTF8(" м\n");
    result << juce::String::fromUTF8("Боковой снос ветром (Z): ") << juce::String(zImpact, 2) << juce::String::fromUTF8(" м\n");
    result << juce::String::fromUTF8("Полное время полета: ") << juce::String(tImpact, 2) << juce::String::fromUTF8(" с\n");
    result << "==================================================\n";

    juce::Logger::outputDebugString(result);

    // Добавляем траекторию на панель
    ballisticPanel->addTrajectory(trajectory);
}
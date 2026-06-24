#pragma once

#include <juce_core/juce_core.h>
#include <vector>
#include <cmath>

//==============================================================================
// точка траектории
struct ArrowPoint {
    double x, y, z;      // Координаты
    double velocity;     // Скорость

    ArrowPoint(double _x = 0, double _y = 0, double _z = 0, double _v = 0)
        : x(_x), y(_y), z(_z), velocity(_v) {}
};

//==============================================================================
// одна траектория
class ArrowTrajectory {
public:
    std::vector<ArrowPoint> points;
    juce::String description;

    ArrowTrajectory() = default;
    ArrowTrajectory(const juce::String& desc) : description(desc) {}

    ArrowTrajectory(const ArrowTrajectory& other) = default;
    ArrowTrajectory(ArrowTrajectory&& other) = default;
    ArrowTrajectory& operator=(const ArrowTrajectory& other) = default;
    ArrowTrajectory& operator=(ArrowTrajectory&& other) = default;
    ~ArrowTrajectory() = default;

    void addPoint(const ArrowPoint& point) { points.push_back(point); }
    void clear() { points.clear(); }
    int size() const { return (int)points.size(); }
    const ArrowPoint& getPoint(int index) const {
        return points[static_cast<size_t>(index)];
    }
};

//==============================================================================
// расчет баллистической траектории
class BallisticCalculator {
public:
    // физ константы
    static constexpr double G = 9.80665; // ускорение свободного падения (уровень моря)
    static constexpr double RHO = 1.225; // плотность воздуха (при стандартных условиях)

    struct Params {
        double mass = 0.025;        // масса стрелы в кг
        double diameter = 0.006;    // диаметр стрелы в м
        double Cd = 0.4;            // коэффициент сопротивления
        double v0 = 85.0;           // начальная скорость в м/с
        double pitch = 10.0;        // угол наклона в градусах
        double yaw = 0.0;           // азимут в градусах
        double windSpeed = 5.0;     // скорость ветра в м/с
        double windAngle = 90.0;    // направление ветра в градусах
    };

    struct Result {
        double distance = 0.0;      // дальность (X) в м
        double drift = 0.0;         // боковой снос (Z) в м
        double time = 0.0;          // время полета в с
        ArrowTrajectory trajectory;
    };

    static Result calculate(const Params& params);
};
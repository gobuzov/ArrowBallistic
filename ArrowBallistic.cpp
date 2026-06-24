#include "ArrowBallistic.h"
#include <cassert>

BallisticCalculator::Result BallisticCalculator::calculate(const Params& params) {
    Result result;
    // проверка входных параметров
    assert(params.mass > 0.0);
    assert(params.diameter > 0.0);
    assert(params.Cd > 0.0);
    assert(params.v0 > 0.0);
    /// параметры
    double pitchRad = juce::MathConstants<double>::pi * params.pitch / 180.0;
    double yawRad = juce::MathConstants<double>::pi * params.yaw / 180.0;
    double windAngleRad = juce::MathConstants<double>::pi * params.windAngle / 180.0;
    
    double area = juce::MathConstants<double>::pi * std::pow(params.diameter / 2.0, 2);
    double k = (RHO * params.Cd * area) / (2.0 * params.mass);
    
    double wx = params.windSpeed * std::cos(windAngleRad);
    double wz = params.windSpeed * std::sin(windAngleRad);
    
    double x = 0.0;
    double y = 1.5; // Высота выстрела
    double z = 0.0;
    
    juce::String description = "pit:" + juce::String((int)params.pitch) +
                              " m:" + juce::String(params.mass) +
                              " v:" + juce::String((int)params.v0);
    result.trajectory = ArrowTrajectory(description);
    
    double vx = params.v0 * std::cos(pitchRad) * std::cos(yawRad);
    double vy = params.v0 * std::sin(pitchRad);
    double vz = params.v0 * std::cos(pitchRad) * std::sin(yawRad);
    
    double t = 0.0;
    double dt = 0.001;
    double vInit = std::sqrt(vx * vx + vy * vy + vz * vz);
    result.trajectory.addPoint(ArrowPoint(x, y, z, vInit));
    
    /// цикл расчетов
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
            result.trajectory.addPoint(ArrowPoint(x, y, z, vCurrent));
        }
    }
    
    /// вычисление точки падения
    if (y < 0.0 && stepCount > 0) {
        double frac = yPrev / (yPrev - y);
        result.time = tPrev + frac * dt;
        result.distance = xPrev + frac * (x - xPrev);
        result.drift = zPrev + frac * (z - zPrev);
    } else {
        result.time = t;
        result.distance = x;
        result.drift = z;
    }
    
    result.trajectory.addPoint(ArrowPoint(result.distance, 0, result.drift, 0));
    // проверка результатов
    assert(result.distance >= 0.0);
    assert(result.time >= 0.0);
    return result;
}
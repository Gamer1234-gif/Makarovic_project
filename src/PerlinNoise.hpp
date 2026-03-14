#pragma once
#include <cmath>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

class PerlinNoise {
public:
    PerlinNoise() {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(std::random_device{}());
        for (int i = 255; i > 0; i--) {
            std::uniform_int_distribution<int> dist(0, i);
            int j = dist(engine);
            std::swap(p[i], p[j]);
        }
        p.insert(p.end(), p.begin(), p.end());
    }

    PerlinNoise(uint32_t seed) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);
        std::default_random_engine engine(seed);
        for (int i = 255; i > 0; i--) {
            std::uniform_int_distribution<int> dist(0, i);
            int j = dist(engine);
            std::swap(p[i], p[j]);
        }
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x, double y) const {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;

        x -= std::floor(x);
        y -= std::floor(y);

        double u = fade(x);
        double v = fade(y);

        int A = p[X] + Y;
        int B = p[X + 1] + Y;

        return lerp(
            v, lerp(u, grad(p[A], x, y), grad(p[B], x - 1, y)),
            lerp(u, grad(p[A + 1], x, y - 1), grad(p[B + 1], x - 1, y - 1))
        );
    }

private:
    std::vector<int> p;

    static double fade(double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    static double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    static double grad(int hash, double x, double y) {
        switch (hash & 3) {
        case 0: return x + y;
        case 1: return -x + y;
        case 2: return x - y;
        case 3: return -x - y;
        }
        return 0;
    }
};

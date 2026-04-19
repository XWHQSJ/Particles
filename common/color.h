#pragma once

#include "particle.h"

// Fire palette: orange-red gradient
static const Color3 kFirePalette[] = {
    {1.0f, 0.2f, 0.0f},  {1.0f, 0.35f, 0.0f},
    {1.0f, 0.5f, 0.0f},  {1.0f, 0.65f, 0.0f},
    {1.0f, 0.8f, 0.1f},  {1.0f, 0.9f, 0.3f},
    {1.0f, 1.0f, 0.5f},  {0.9f, 0.3f, 0.0f},
};
static const int kFirePaletteSize = 8;

// Snow palette: white-blue
static const Color3 kSnowPalette[] = {
    {1.0f, 1.0f, 1.0f},  {0.9f, 0.95f, 1.0f},
    {0.85f, 0.9f, 1.0f}, {0.8f, 0.85f, 0.95f},
    {0.95f, 0.97f, 1.0f},{0.88f, 0.92f, 1.0f},
};
static const int kSnowPaletteSize = 6;

// Galaxy palette: purple-blue-cyan
static const Color3 kGalaxyPalette[] = {
    {0.5f, 0.3f, 1.0f},  {0.4f, 0.5f, 1.0f},
    {0.3f, 0.7f, 1.0f},  {0.5f, 0.8f, 1.0f},
    {0.7f, 0.4f, 1.0f},  {0.8f, 0.6f, 1.0f},
    {0.3f, 0.9f, 0.9f},  {1.0f, 0.8f, 0.9f},
};
static const int kGalaxyPaletteSize = 8;

// Explosion palette: bright yellow-orange-white
static const Color3 kExplosionPalette[] = {
    {1.0f, 1.0f, 0.8f},  {1.0f, 0.9f, 0.4f},
    {1.0f, 0.7f, 0.2f},  {1.0f, 0.5f, 0.1f},
    {1.0f, 0.3f, 0.0f},  {0.8f, 0.2f, 0.0f},
};
static const int kExplosionPaletteSize = 6;

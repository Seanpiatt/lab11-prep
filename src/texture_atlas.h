#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <cassert>
#include <iostream>

#include "vector_util.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

// RectPacker maintains two offsets into an overall bin size:
//  - A line y-offset, representing the current horizontal line on which to place new rectangles
//      horizontally along (the line corresponding to the rectangle tops)
//  - An x-offset into the line, representing the x-coordinate to align the next rectangle left-side
// Furthermore it maintains the maximum texture height encountered along the current line.
// The RectPacker places rectangles left to right along this line until it cannot fit the next
//  rectangle. Once this happens, the line is shifted down to meet the bottom of the tallest texture
//  placed on the line, and the x-offset is reset. This continues until the next rectangle
//  packed cannot be fit on the line within the bin space.
class RectPacker {
public:
    RectPacker(const sf::Vector2f& size) {
        currX = 0;
        currY = 0;
        bin = sf::FloatRect({currX, currY}, {size.x, size.y});
    };

    // pack returns { succeeded, rect id }. If not succeeded, id is irrelevant (but ideally should
    // be set to the maximum possible value).
    std::pair<bool, size_t> pack(const sf::Vector2f& rectSize) { 
        if(currX + rectSize.x > bin.size.x){
            currY += tallestHeightOnLine;
            currX = 0;
            tallestHeightOnLine = 0;
        }
        if(currX + rectSize.x > bin.size.x || currY + rectSize.y > bin.size.y){
            return {false, std::numeric_limits<size_t>::max()};
        }
        else{
            sf::FloatRect r({currX,currY},{rectSize.x,rectSize.y});
            currentRects.push_back(r);
            currX += rectSize.x;
            if (rectSize.y > tallestHeightOnLine) tallestHeightOnLine = rectSize.y;
            return {true,currentRects.size()-1};
        }
    }

    // Returns the coordinates and size of a previously returned rectangle id.
    // Note that in this coordinate system, y points downwards from the origin at the top-left.
    sf::FloatRect getBounds(size_t rectID) const { 
        return currentRects[rectID];
    }

private:
    sf::FloatRect bin;
    float currX;
    float currY;
    float tallestHeightOnLine;
    std::vector<sf::FloatRect> currentRects;
};

// Returns a new image which pages the given img with *pixels* pixels on each side.
// The colour of new border pixels are taken by sampling the closest valid pixel in the original
// image.
//
// NOTE: a production implementation might also support other edge wrapping behaviors, like cycling
// around to the other side of the texture. See https://learnopengl.com/Getting-started/Textures
// Our implementation corresponds to GL_CLAMP_TO_EDGE
inline sf::Image padImage(const sf::Image& img, uint pixels) { 
    int w = img.getSize().x;
    int h = img.getSize().y;
    int p = pixels;
    sf::Image out({unsigned(w + 2 * p), unsigned(h + 2 * p)});
    for (int y = 0; y < h + 2*p; y++){
        for(int x = 0; x < w + 2*p; x++){
            int sx = std::clamp(x-p,0,w-1);
            int sy = std::clamp(y-p,0,h-1);
            out.setPixel({unsigned(x), unsigned(y)}, img.getPixel({unsigned(sx), unsigned(sy)}));

        }
    }
    return out;
};

using TextureID = size_t;

class TextureAtlas {
public:
    TextureAtlas(sf::Vector2u binSize, size_t padding)
        : mTexture(binSize), mPacker(sf::Vector2f(mTexture.getSize())), mPadding{padding} {}

    TextureID registerTexture(const std::string& filepath) { return {}; }

    sf::FloatRect getBoundsUV(TextureID id) const { return {}; }

    sf::Vector2u getSizePixels(TextureID id) const { return {}; }

    const sf::Texture& getTexture() const { return mTexture; }

private:
    sf::Texture mTexture;
    RectPacker mPacker;
    size_t mPadding;

};

#endif  // TEXTURE_ATLAS_H

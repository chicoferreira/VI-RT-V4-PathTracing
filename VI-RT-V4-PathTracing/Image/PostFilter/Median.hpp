//
//  Box.hpp
//  VI-RT-V4-PathTracing
//
//  Created by Luis Paulo Santos on 26/03/2025.
//

#ifndef Median_hpp
#define Median_hpp

#include <vector>
#include <algorithm>
#include "vector.hpp"

class Median  {
    const int margin=5;
    const int hmargin=2;
    const int median_ndx = 12;

public:
    Median () {}
    void Filter (int const W, int const H, RGB *imageIn, RGB *imageOut) {
        for (int y=hmargin ; y<H-hmargin ; y++) {
            int const row_off = y*W;
            for (int x=hmargin ; x<W-hmargin ; x++) {
                int const offset = row_off + x;
                RGB  Cin = imageIn[offset];
                float const Lin = Cin.Y();
                if (abs(Lin) < EPSILON ) {
                    imageOut[offset] = Cin;
                    continue;
                }
                std::vector<float> window;
                for (int v=-hmargin ; v<(hmargin+1) ; v++) {
                    int const row_off_uv = (y+v)*W;
                    for (int u=-hmargin ; u<(hmargin+1) ; u++) {
                        int const offset_uv = row_off_uv + x+u;
                        window.push_back(imageIn[offset_uv].Y());
                    }
                }
                std::sort(window.begin(), window.end());
                float  Lout = window[median_ndx];
                RGB Cout = Cin * Lout / Lin;
                imageOut[offset] = Cout;
            }
        }
    }
};

#endif /* Median_hpp */

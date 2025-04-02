//
//  Box.hpp
//  VI-RT-V4-PathTracing
//
//  Created by Luis Paulo Santos on 26/03/2025.
//

#ifndef Box_hpp
#define Box_hpp

#include "vector.hpp"

class Box  {

    const int margin=3;
    const int hmargin=1;
    const float mSq = margin*margin;

public:
    Box () {}
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
                float sumL=0.f;
                for (int v=-hmargin ; v<(hmargin+1) ; v++) {
                    int const row_off_uv = (y+v)*W;
                    for (int u=-hmargin ; u<(hmargin+1) ; u++) {
                        int const offset_uv = row_off_uv + x+u;
                        sumL += imageIn[offset_uv].Y();
                    }
                }
                float  Lout = sumL / mSq;
                RGB Cout = Cin * Lout / Lin;
                imageOut[offset] = Cout;
            }
        }
    }
};

#endif /* Box_hpp */

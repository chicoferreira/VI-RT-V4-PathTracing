//
//  Reinhard.hpp
//  VI-RT-V4-PathTracing
//
//  Created by Luis Paulo Santos on 26/03/2025.
//

#ifndef Reinhard_hpp
#define Reinhard_hpp

class Reinhard  {

public:
    Reinhard () {}
    void ToneMap (int const W, int const H, RGB *imageIn, RGB *imageOut) {
        for (int y=0 ; y<H ; y++) {
            int const row_off = y*W;
            for (int x=0 ; x<W ; x++) {
                int const offset = row_off + x;
                RGB Cin = imageIn[offset];
                float Lin = Cin.Y();
                RGB const Cout = Cin / (1. + Lin);
                imageOut[offset] = Cout;
            }
        }
    }
};

#endif /* Reinhard_hpp */

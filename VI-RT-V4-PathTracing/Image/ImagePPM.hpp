//
//  ImagePPM.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 09/03/2023.
//

#ifndef ImagePPM_hpp
#define ImagePPM_hpp
#include "image.hpp"
//#include "ToneMap.hpp"
//#include "Reinhard.hpp"

class ImagePPM: public Image {
    char_pixel *imageToSave;

public:
    ImagePPM(const int W, const int H):Image(W, H) {}
    ImagePPM():Image() {}
    bool Save (std::string filename);
    bool Load (std::string filename);
    void ImgClamp (int const W, int const H, RGB *image, char_pixel *img2save);
};

#endif /* ImagePPM_hpp */

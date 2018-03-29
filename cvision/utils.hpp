//
// Created by egordm on 29-3-2018.
//

#ifndef B3ITASSIGNMENT2_UTILS_H
#define B3ITASSIGNMENT2_UTILS_H


namespace cvision { namespace utils {

    /**
     * Gives a valid index inside the array. If index is out of bounds it will be wrapped around to fit.
     * @param array_size
     * @param index 
     * @return 
     */
    inline int wrap_around_array(const int &array_size, const int &index) {
        if (index < 0) return array_size - 1 + index;
        if (index >= array_size) return index - array_size;
        return index;
    }
}}


#endif //B3ITASSIGNMENT2_UTILS_H

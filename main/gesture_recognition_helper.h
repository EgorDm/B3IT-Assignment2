//
// Created by egordm on 7-4-2018.
//

#ifndef B3ITASSIGNMENT2_GESTURE_RECOGNITION_HELPER_H
#define B3ITASSIGNMENT2_GESTURE_RECOGNITION_HELPER_H


#include "window.h"
#include "gesture_detection_helper.h"

#define HAND_HISTORY_SIZE 3
#define DISTANCE_THRESH (min_dim * 6.0)
#define HAND_TIMEOUT_TICKS 2

using namespace cvision::processing::limb_recognition::hand;

template<typename T>
struct TrackHistory {
    int identifier;
    int last_update;
    std::vector<T> state_history;

    TrackHistory(const T &o, int identifier) : identifier(identifier), last_update(0) {
        state_history.push_back(o);
    }

    inline void add_history(const T &o, unsigned int history_size) {
        last_update = 0;
        state_history.push_back(o);
        if(state_history.size() > history_size)
            state_history.erase(state_history.begin(), state_history.end() - history_size);
    }
};

struct HandHistory : public TrackHistory<Hand> {
    std::vector<TrackHistory<Finger>*> finger_histories;

    HandHistory(const Hand &o, int identifier) : TrackHistory(o, identifier) {}
};

template<typename T, typename H>
class ObjectTracker {
public:
    unsigned int history_size;
    unsigned int unique_id;
    unsigned int timeout_ticks;
    double distance_threshold;
    std::function<double(const T &, const H*)> distance_fn;

    ObjectTracker(unsigned int history_size, unsigned int timeout_ticks, double distance_threshold,
                  const function<double(const T &, const H *)> &distance_fn)
            : history_size(history_size), timeout_ticks(timeout_ticks), distance_threshold(distance_threshold),
              distance_fn(distance_fn), unique_id(0) {}

    void update(const std::vector<T> &new_states, std::vector<H*> &histories);
};

class GestureRecognitionHelper : public WindowHelper {
private:
    const HandDetectorHelper *hands_detector;
    ObjectTracker<Hand, HandHistory> hand_tracker;
    ObjectTracker<Finger, TrackHistory<Finger>> finger_tracker;
    std::vector<HandHistory*> hand_histories;

public:
    explicit GestureRecognitionHelper(const HandDetectorHelper *hands_detector)
            : hands_detector(hands_detector), hand_tracker(3, 3, 0, &distance_hand),
              finger_tracker(3, 3, 0, &distance_finger){}

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;

    static double distance_hand(const Hand &state, const HandHistory* history);

    static double distance_finger(const Finger &state, const TrackHistory<Finger>* history);
};


#endif //B3ITASSIGNMENT2_GESTURE_RECOGNITION_HELPER_H

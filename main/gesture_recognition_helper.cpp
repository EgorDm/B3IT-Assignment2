//
// Created by egordm on 7-4-2018.
//

#include "gesture_recognition_helper.h"
#include "../cvision/math.hpp"

using namespace cvision;

#define DISTANCE_THRESH (min_dim * 6.0)
#define HAND_TIMEOUT_TICKS 2

std::vector<std::tuple<int, int>> match_points(std::vector<std::tuple<int, int, double>> &candidates, const double &thresh) {
    std::sort(candidates.begin(), candidates.end(),
              [](const std::tuple<int, int, double> &c1, const std::tuple<int, int, double> &c2) {
                  return std::get<2>(c1) < std::get<2>(c2);
              });

    std::vector<std::tuple<int, int>> ret;
    while (!candidates.empty()) {
        const auto candidate = candidates.front();
        if (std::get<2>(candidate) > pow(thresh, 2)) break;

        ret.emplace_back(std::get<0>(candidate), std::get<1>(candidate));
        candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                        [&candidate](const std::tuple<int, int, double> &c1) {
                                            return std::get<0>(candidate) == std::get<0>(c1) ||
                                                   std::get<1>(candidate) == std::get<1>(c1);
                                        }), candidates.end());
    }
    return ret;
};

template<typename T, typename H>
void ObjectTracker<T, H>::update(const std::vector<T> &new_states, std::vector<H *> &histories) {
    for(auto &history : histories) ++history->last_update;

    std::vector<int> used_states;
    std::vector<std::tuple<int, int, double>> candidates;
    for (int j = 0; j < new_states.size(); ++j) {
        for (int i = 0; i < histories.size(); ++i) {
            candidates.emplace_back(i, j, distance_fn(new_states[j], histories[i]));
        }
    }

    for(const auto &candidate : match_points(candidates, distance_threshold)) {
        histories[std::get<0>(candidate)]->add_history(new_states[std::get<1>(candidate)], history_size);
        used_states.push_back(std::get<1>(candidate));
    }

    for (int i = (int) histories.size() - 1; i >= 0; --i)
        if (histories[i]->last_update >= HAND_TIMEOUT_TICKS)
            histories.erase(histories.begin() + i);
    for (int i = 0; i < new_states.size(); ++i)
        if (std::find(used_states.begin(), used_states.end(), i) == used_states.end())
            histories.emplace_back(new H(new_states[i], unique_id++));
}

cv::Mat GestureRecognitionHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    const auto &hands = hands_detector->hands;
    const int min_dim = min(original.rows, original.cols);

    hand_tracker.distance_threshold = DISTANCE_THRESH;
    finger_tracker.distance_threshold = DISTANCE_THRESH/2;
    hand_tracker.update(hands, hand_histories);

    for (const auto &history : hand_histories) {
        std::stringstream ss;
        ss << history->identifier;
        cv::putText(src, ss.str(), history->state_history.back().palm_center, cv::FONT_HERSHEY_PLAIN, 2,
                    cv::Scalar(255, 255, 0));

        finger_tracker.update(history->state_history.back().fingers, history->finger_histories);
        for(const auto &finger_history : history->finger_histories) {
            std::stringstream sz;
            sz << finger_history->identifier;
            cv::circle(src, finger_history->state_history.back().tip, 2, cv::Scalar(255, 255, 255), -1);
            cv::putText(src, sz.str(), finger_history->state_history.back().tip, cv::FONT_HERSHEY_PLAIN, 1,
                        cv::Scalar(0, 255, 255));
        }
    }

    return WindowHelper::draw(src, original);
}

double GestureRecognitionHelper::distance_hand(const Hand &state, const HandHistory *history) {
    return math::distance_sq(state.palm_center, history->state_history.back().palm_center);
}

double GestureRecognitionHelper::distance_finger(const Finger &state, const TrackHistory<Finger> *history) {
    return math::distance_sq(state.tip, history->state_history.back().tip);
}

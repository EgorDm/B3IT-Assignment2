//
// Created by egordm on 8-4-2018.
//

#ifndef B3ITASSIGNMENT2_OBJECT_TRACKING_H
#define B3ITASSIGNMENT2_OBJECT_TRACKING_H

#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>

namespace cvision { namespace processing { namespace tracking {
    std::vector<std::tuple<int, int>> match_points(std::vector<std::tuple<int, int, double>> &candidates, const double &thresh);

    template<typename T>
    struct TrackHistory {
        int identifier;
        int last_update;
        int history_length;
        std::vector <T> state_history;

        TrackHistory(const T &o, int identifier) : identifier(identifier), last_update(0), history_length(1) {
            state_history.push_back(o);
        }

        inline void add_history(const T &o, unsigned int history_size) {
            last_update = 0;
            ++history_length;
            state_history.push_back(o);
            if (state_history.size() > history_size)
                state_history.erase(state_history.begin(), state_history.end() - history_size);
        }
    };


    template<typename T, typename H>
    class ObjectTracker {
    public:
        unsigned int history_size;
        unsigned int unique_id;
        unsigned int timeout_ticks;
        double distance_threshold;
        std::function<double(const T &, const H *)> distance_fn;

        ObjectTracker(unsigned int history_size, unsigned int timeout_ticks, double distance_threshold,
                      const std::function<double(const T &, const H *)> &distance_fn)
                : history_size(history_size), timeout_ticks(timeout_ticks), distance_threshold(distance_threshold),
                  distance_fn(distance_fn), unique_id(0) {}

        inline void update(const std::vector <T> &new_states, std::vector<H *> &histories) {
            for (auto &history : histories) ++history->last_update;

            std::vector<int> used_states;
            std::vector<std::tuple<int, int, double>> candidates;
            for (int j = 0; j < new_states.size(); ++j) {
                for (int i = 0; i < histories.size(); ++i) {
                    candidates.emplace_back(i, j, distance_fn(new_states[j], histories[i]));
                }
            }

            for (const auto &candidate : match_points(candidates, distance_threshold)) {
                histories[std::get<0>(candidate)]->add_history(new_states[std::get<1>(candidate)], history_size);
                used_states.push_back(std::get<1>(candidate));
            }

            for (int i = (int) histories.size() - 1; i >= 0; --i) {
                if (histories[i]->last_update >= timeout_ticks) {
                    auto history = histories[i];
                    histories.erase(histories.begin() + i);
                    delete history;
                }
            }
            for (int i = 0; i < new_states.size(); ++i)
                if (std::find(used_states.begin(), used_states.end(), i) == used_states.end())
                    histories.emplace_back(new H(new_states[i], unique_id++));
        }
    };
}}}


#endif //B3ITASSIGNMENT2_OBJECT_TRACKING_H

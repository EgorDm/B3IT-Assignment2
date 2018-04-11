//
// Created by egordm on 7-4-2018.
//

#include "gesture_recognition_helper.h"
#include <chrono>
#include "../cvision/math.hpp"

using namespace cvision;
using namespace cv;
using namespace DollarRecognizer;

GestureDetectionHelper::GestureDetectionHelper(const std::vector<std::string> &active_templates,
                                               const HandTrackingHelper *hand_tracker)
        : hand_tracker(hand_tracker) {
    recognizer.loadTemplates();
    recognizer.activateTemplates(active_templates);
}

bool GestureDetectionHelper::on_click(int x, int y, bool rb) {
    if (!rb) drawing = true;
    if (!drawing) return false;

    if (!rb) {
        Point2D pos(x, y);
        const auto pit = std::find_if(positions.begin(), positions.end(), [&pos](const Point2D &op) {
            return pos.x == op.x && pos.y == op.y;
        });
        if (pit == positions.end()) positions.emplace_back(pos);
    }
    if (rb) {
        drawing = false;
        auto res = recognizer.recognize(positions);
        result = new RecognitionResult(res);
        std::cout << "Result: " << result->name << " Score: " << result->score << std::endl;
        positions.clear();
    }

    return true;
}

#define GESTURE_READY_TIMEOUT 2000
#define GESTURE_STATE_INITIALIZING 0
#define GESTURE_STATE_TIMED_OUT 1
#define GESTURE_STATE_INITIALIZED 2
#define GESTURE_STATE_CANCELLED 3
#define GESTURE_STATE_RECOGNIZED 4

int GestureDetectSession::update_initialization(HandHistory *history) {
    if (history->finger_histories.size() >= 5) {
        start_confirmed = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
        return GESTURE_STATE_INITIALIZING;
    }
    if (start_confirmed < 0) return GESTURE_STATE_INITIALIZING;

    if (std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count() - start_confirmed >
        GESTURE_READY_TIMEOUT) {
        return GESTURE_STATE_TIMED_OUT;
    }

    if (history->finger_histories.size() == 1) {
        finger_id = history->finger_histories.back()->identifier;
        return GESTURE_STATE_INITIALIZED;
    }

    return GESTURE_STATE_INITIALIZING;
}

int GestureDetectSession::update(HandHistory *history) {
    if (start_confirmed < 0 || finger_id < 0) {
        auto state = update_initialization(history);
        if (state != GESTURE_STATE_INITIALIZED) return state;
    }

    if (history->finger_histories.size() >= 5) {
        return GESTURE_STATE_RECOGNIZED;
    }

    TrackHistory<Finger> *finger_history = nullptr;
    for (const auto &fh : history->finger_histories) if (fh->identifier == finger_id) finger_history = fh;
    if (finger_history == nullptr) return GESTURE_STATE_CANCELLED;

    auto hand = HandTrackingHelper::average_hand(history);
    auto finger = HandTrackingHelper::average_finger(finger_history);

    auto position = finger.tip + hand.palm_center;
    positions.emplace_back(position.x, position.y);

    return 0;
}

Mat GestureDetectionHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    for (int i = (int) sessions.size() - 1; i >= 0; --i) {
        HandHistory *hand_history = nullptr;
        for (const auto &hh : hand_tracker->hand_histories) {
            if (hh->identifier == sessions[i].hand_id) hand_history = hh;
        }

        if (hand_history == nullptr) {
            sessions.erase(sessions.begin() + i);
            continue;
        }

        auto state = sessions[i].update(hand_history);
        if (state == GESTURE_STATE_CANCELLED || state == GESTURE_STATE_TIMED_OUT || state == GESTURE_STATE_RECOGNIZED) {
            if (state == GESTURE_STATE_RECOGNIZED) {
                auto res = recognizer.recognize(sessions[i].positions);
                std::cout << "Recognized: " << res.name << " for " << sessions[i].hand_id << std::endl;
                auto current_time = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                results.emplace_back(current_time, sessions[i].hand_id, res);
            }

            sessions.erase(sessions.begin() + i);
            continue;
        }
    }

    for (const auto &hh : hand_tracker->hand_histories) {
        bool exists = false;
        for (const auto &session : sessions) {
            if (session.hand_id != hh->identifier) continue;
            exists = true;
            break;
        }
        if(exists) continue;

        GestureDetectSession session(hh->identifier);
        auto state = session.update(hh);
        if (state != GESTURE_STATE_CANCELLED || state != GESTURE_STATE_TIMED_OUT || state != GESTURE_STATE_RECOGNIZED) {
            sessions.push_back(session);
        }
    }

    int draw_offset = 15;
    for (const auto &session : sessions) {
        if(session.finger_id != -1) {
            auto result = recognizer.recognize(session.positions);
            std::stringstream ss;
            ss << "Hand: " << session.hand_id << " Recognized: " << result.name << " Score: " << result.score;
            cv::putText(src, ss.str(), Point(0, draw_offset), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,255), 1);
            draw_offset += 20;
        } else {
            std::stringstream ss;
            ss << "Hand: " << session.hand_id << (session.start_confirmed < 0 ? " is doing nothing" : " is prepared for gesture");
            cv::putText(src, ss.str(), Point(0, draw_offset), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,255), 1);
            draw_offset += 20;
        }

        for (int i = 1; i < session.positions.size(); ++i) {
            auto p1 = session.positions[i - 1];
            auto p2 = session.positions[i];
            line(src, Point((int) p1.x, (int) p1.y), Point((int) p2.x, (int) p2.y), Scalar(0, 0, 255));
        }
    }
    for(int i = (int)results.size() - 1; i >= 0; --i) {
        auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();

        std::stringstream ss;
        ss << "Hand: " << results[i].hand_id << " Finalized recognition: " << results[i].result.name << " Score: " << results[i].result.score;
        cv::putText(src, ss.str(), Point(0, draw_offset), FONT_HERSHEY_PLAIN, 1, Scalar(0,255,255), 1);
        draw_offset += 20;

        if(current_time - results[i].recognized_time > RESULT_DURATION_DISPLAY) results.erase(results.begin() + i);
    }

    for (int i = 1; i < positions.size(); ++i) {
        auto p1 = positions[i - 1];
        auto p2 = positions[i];
        line(src, Point((int) p1.x, (int) p1.y), Point((int) p2.x, (int) p2.y), Scalar(0, 0, 255));
    }

    return src;
}

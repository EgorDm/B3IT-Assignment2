//
// Created by egordm on 7-4-2018.
//

#include "gesture_recognition_helper.h"
#include "../cvision/math.hpp"

using namespace cvision;
using namespace cv;
using namespace DollarRecognizer;

GestureDetectionHelper::GestureDetectionHelper(const std::vector<std::string> &active_templates, const HandTrackingHelper *hand_tracker)
        : hand_tracker(hand_tracker) {
    recognizer.loadTemplates();
    recognizer.activateTemplates(active_templates);
}

bool GestureDetectionHelper::on_click(int x, int y, bool rb) {
    if (!rb) drawing = true;
    if (!drawing) return false;

    if(!rb) {
        Point2D pos(x, y);
        const auto pit = std::find_if(positions.begin(), positions.end(), [&pos](const Point2D &op) {
            return pos.x == op.x && pos.y == op.y;
        });
        if(pit == positions.end()) positions.emplace_back(pos);
    }
    if(rb) {
        drawing = false;
        auto res = recognizer.recognize(positions);
        result = new RecognitionResult(res);
        std::cout << "Result: " << result->name << " Score: " << result->score << std::endl;
        positions.clear();
    }

    return true;
}

Mat GestureDetectionHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    for(int i = (int)sessions.size() - 1; i >= 0; --i) {
        auto &session = sessions[i];
        HandHistory *hand_history = nullptr;
        TrackHistory<Finger> *finger_history = nullptr;
        for(const auto &hh : hand_tracker->hand_histories) {
            for(const auto &fh : hh->finger_histories) {
                if(session.hand_id == hh->identifier && session.finger_id == fh->identifier) {
                    hand_history = hh;
                    finger_history = fh;
                }
            }
        }
        if(hand_history == nullptr || finger_history == nullptr) {
            sessions.erase(sessions.begin() + i);
            continue;
        }

        auto hand = HandTrackingHelper::average_hand(hand_history);
        auto finger = HandTrackingHelper::average_finger(finger_history);
        cv::Point finger_pos = hand.palm_center + finger.tip;
        session.positions.emplace_back(finger_pos.x, finger_pos.y);
    }

    for(const auto &hh : hand_tracker->hand_histories) {
        auto hand = HandTrackingHelper::average_hand(hh);
        if(hand.fingers.size() != 1) continue;
        const auto &fh = hh->finger_histories.front();
        bool exists = false;
        for(const auto &session : sessions) {
            if(session.hand_id == hh->identifier && session.finger_id == fh->identifier) {
                exists = true;
                break;
            }
        }
        if(exists) continue;

        cv::Point finger_pos = hand.palm_center + hand.fingers.back().tip;
        GestureDetectSession session(hh->identifier, fh->identifier, {Point2D(finger_pos.x, finger_pos.y)});
        sessions.push_back(session);
    }

    for(const auto &session : sessions) {
        for(int i = 1; i < session.positions.size(); ++i) {
            auto p1 = session.positions[i-1];
            auto p2 = session.positions[i];
            line(src, Point((int)p1.x, (int)p1.y), Point((int)p2.x, (int)p2.y), Scalar(0, 0, 255));
        }
    }

    for(int i = 1; i < positions.size(); ++i) {
        auto p1 = positions[i-1];
        auto p2 = positions[i];
        line(src, Point((int)p1.x, (int)p1.y), Point((int)p2.x, (int)p2.y), Scalar(0, 0, 255));
    }

    return src;
}


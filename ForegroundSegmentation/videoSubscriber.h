#pragma once

// AvFrame
extern "C" {
    #include <libavutil/frame.h>
}
#include <observer.h>

//STl
#include <map>
#include <thread>
#include <condition_variable>

// Frame Scaler
#include <framescaler.h>

// OpenCV headers
#include <opencv2/core.hpp>

#include "pluginProcessor.h"

namespace jami 
{
    class FrameCopy 
	{
		public:
		// This frame is a resized version of the original in RGB format
		cv::Mat resizedFrameRGB;
		cv::Size resizedSize;
		// This frame is used to draw predictions into in RGB format
		cv::Mat predictionsFrameBGR;
        cv::Size originalSize;
		// This frame is used to draw predictions into in RGB format on a resized frame
		cv::Mat predictionsResizedFrameBGR;
	};

    class VideoSubscriber : public jami::Observer<AVFrame *> 
    {
        public:
            VideoSubscriber(const std::string &dataPath);
            ~VideoSubscriber();

            virtual void update(jami::Observable<AVFrame *> *, AVFrame *const &) override;
            virtual void attached(jami::Observable<AVFrame *> *) override;
            virtual void detached(jami::Observable<AVFrame *> *) override;

            void detach();
            void stop();


        private:
            // Observer pattern
            Observable<AVFrame *> *observable_;
            bool isAttached{false};

            //Data
            std::string path_;
            
            // Frame
            FrameCopy fcopy;
            cv::Mat frame;
            
            FrameScaler scaler;
            void rotateFrame(int angle, cv::Mat &mat);

            // Threading
            std::thread processFrameThread;
            std::mutex inputLock;
            std::condition_variable inputCv;

            // Status variables of the processing
            bool firstRun{true};
            bool running{true};
            bool newFrame{false};

            //std::shared_ptr<PluginProcessor> pluginProcessor;
            PluginProcessor pluginProcessor;
    };
}


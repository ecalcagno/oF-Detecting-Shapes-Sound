#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxFFTLive.h"
#include "ofxFFTFile.h"
#include "ofxGui.h"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
        void exit();
    
		  ofVideoGrabber 		vidGrabber;
          
        ofImage imagen;
        ofImage bg;
    
        ofxCvColorImage			colorImg;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

        ofxCvContourFinder 	contourFinder;

		int 				threshold;
		bool				bLearnBakground;

        ofPlanePrimitive plano;
        vector <ofMesh> mesh;
    
        vector <ofPolyline> line;
    
    vector <ofPoint> sizeMesh;
    
    vector <ofPoint> centroides;
    
        ofSoundPlayer music;
    
        ofxFFTLive fftLive;
        ofxFFTFile fftFile;
    
        ofxPanel gui;
        ofxSlider<float> audioThreshold;
        ofxSlider<float> audioPeakDecay;
        ofxSlider<float> audioMaxDecay;
        ofxToggle audioMirror;
    
        float * audioData;
    
        bool gui_fft;
};


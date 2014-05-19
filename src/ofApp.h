#pragma once

#include "ofMain.h"
#include "ofxANN.h"
#include "ofxPCA.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // knobs!
    int nNeighbors;
    float maxAngle;
    
    ofMesh mesh;
    ofImage depthImage;
    ofxANN ann;
    ofxPCA pca;
    ofVec3f center;
    
    // UI
    int zoom;
    int up;
    int left;
    
    
    void drawNormals(const ofMesh& mesh);
};

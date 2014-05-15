#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    nNeighbors = 10;
    
    depthImage.loadImage("depth.png");
    unsigned char * pixels = depthImage.getPixels();

    ofVec3f p;
    for(int row = 0; row < depthImage.getHeight(); row++){
        for(int col = 0; col < depthImage.getWidth(); col++){
            int i = row * depthImage.getWidth() + col;
            mesh.addVertex(ofPoint(col, row, pixels[i]));
            
            // just grab some random point for which to get the neighbors
            if(row == 200 && col == 11){
                p = ofPoint(col, row, pixels[i]);
            }
        }
    }
    cout << "p: " << p.x << "," << p.y << "," << p.z << endl;
    
    ann.loadPoints(mesh.getVertices());
    
    vector<ofxANNNeighbor> neighbors =  ann.getNeighbors(nNeighbors, p);
    vector<ofVec3f> neighborPoints = vector<ofVec3f>();
    
    // don't forget to add the original point itself
    neighborPoints.push_back(p);
    
    for(int i = 0; i < neighbors.size(); i++){
        cout << neighbors[i].point.x << ","<< neighbors[i].point.y << "," << neighbors[i].point.z << " d: " << neighbors[i].distance << endl;
        neighborPoints.push_back(neighbors[i].point);
    }
    
    ofxPCAResult r = pca.analyze(neighborPoints);
    for(int i = 0; i < r.eigenvectors.size(); i++){
        cout << "ev["<< r.eigenvalues[i] <<"]: " << r.eigenvectors[i] << endl;
    }

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    ofSetColor(255, 255, 255);
    mesh.drawVertices();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
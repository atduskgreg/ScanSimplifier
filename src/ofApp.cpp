#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    nNeighbors = 7;
    maxAngle = 15; // degrees
    
    zoom = 1;
    up =0;
    left = 0;
    
    depthImage.loadImage("depth.png");
    unsigned char * pixels = depthImage.getPixels();
    
    for(int row = 0; row < depthImage.getHeight(); row+=10){
        for(int col = 0; col < depthImage.getWidth(); col+=10){
            int i = row * depthImage.getWidth() + col;
            
            if(pixels[i] != 0){
                ofPoint p =  ofPoint(col, row, pixels[i]*2);
                mesh.addVertex(p);
                center.x += p.x;
                center.y += p.y;
                center.z += p.z;
            }
        }
    }
    
    center.x /= mesh.getNumVertices();
    center.y /= mesh.getNumVertices();
    center.z /= mesh.getNumVertices();
    
    ann.loadPoints(mesh.getVertices());
    
    float before = ofGetElapsedTimef();
    
   
    for(int i = 0; i < mesh.getVertices().size(); i++){
        ofVec3f p = mesh.getVertices()[i];
        vector<ofVec3f> neighbors = ann.getNeighborVectors(nNeighbors, p, true);
        ofxPCAResult r = pca.analyze(neighbors);
        // eigenvectors are sorted in descending order by eigenvalue
        // we want one corresponding to the smallest eigenvalue
        mesh.addNormal(r.eigenvectors.back());
    }
    
    float normalsDone = ofGetElapsedTimef();
    cout << "normals done: " << (normalsDone - before) << endl;
    
    cout << "num vertices: " << mesh.getNumVertices() << endl;
    // initialize adjacency matrix
    //MatrixXf adjacency(MatrixXf::Constant(mesh.getNumVertices(),mesh.getNumVertices(), 0));
    // for each vertex
    

    MatrixXf adjacency = MatrixXf::Zero(mesh.getNumVertices(),mesh.getNumVertices());
    cout << "num normals: " << mesh.getNumNormals() << endl;
    
    for(int i = 0; i < mesh.getVertices().size(); i++){
        cout << i << endl;
        //  write zeroes for the full col
        vector<ofxANNNeighbor> neighbors = ann.getNeighbors(nNeighbors, mesh.getVertices()[i], false);
        
        ofVec3f vertexNormal = mesh.getNormal(i);

        if(neighbors.size() > 2){
            for(int j = 0; j < neighbors.size(); j++){
                cout << neighbors[j].x << "," << neighbors[j].y << "," << neighbors[j].z << " d: " << neighbors[j].distance << " idx: " << neighbors[j].idx << endl;
                
                ofVec3f neighborNormal = mesh.getNormal( neighbors[j].idx );
                //  check the neigbors for the angle between the normals
                float angle = vertexNormal.angle(neighborNormal);
                if(abs(angle) < maxAngle){
                   adjacency(i,neighbors[j].idx) = 1;
                } else {
                    adjacency(i,neighbors[j].idx) = 0;
                }
            }
            //don't forget to write a 1 for the current vertex (Aii)
            adjacency(i,i) = 1;
        }
    }
    
    float adjacencyDone = ofGetElapsedTimef();
    
    cout << "adjacency matrix built: " << (adjacencyDone - normalsDone) << endl;
    
    
    
    // HERE:
    // make ofxANN return array of indices
    // use that indices to build the adjacency matrix

    
    
//    float matrixBuilt = ofGetElapsedTimef();
//    
//    cout << "matrix built: "<< (matrixBuilt-before) << "s" << endl;
//    
//
//    float pcaLoaded =ofGetElapsedTimef();
//    
//    cout << "PCA data loaded: " << (pcaLoaded-matrixBuilt) << "s" << endl;
//   
    
//    for(int i = 0; i < mesh.getVertices().size(); i++){
//        ofxPCAResult r = pca.analyze(0, nNeighbors*i, 3, nNeighbors+1);
//        mesh.addNormal(r.eigenvectors.back());
//    }
    
//    cout << "PCA peformed: " << (ofGetElapsedTimef() - pcaLoaded) << endl;
//    
//    cout << "neighborMatrix entries: " << neighborMatrix.size() << endl;
//    cout << "             should be: " << (mesh.getVertices().size() * (nNeighbors+1)) << endl;
//    cout << "[n: " << nNeighbors << " pts: " << mesh.getVertices().size() <<"] time elapsed: " << (ofGetElapsedTimef() -before) << endl;

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0, 0, 0);
    ofSetColor(255, 255, 255);

    
    ofTranslate(-center.x + ofGetWidth()/2 - 20*left, -center.y + ofGetHeight()/2 - 20*up, zoom*20);
    ofRotate(ofMap(ofGetMouseX(), 0, ofGetScreenWidth(), 0, 360), center.x,center.y,center.z);
    
    glPointSize(3);
    mesh.drawVertices();

    drawNormals(mesh);

    
}

void ofApp::drawNormals(const ofMesh& mesh){
    for(int i = 0; i < mesh.getVertices().size(); i++){
        ofVec3f n = mesh.getNormal(i);
        ofVec3f v = mesh.getVertex(i);

        ofPushStyle();
        ofSetColor(0, 255, 0);
        ofLine(v.x, v.y, v.z, v.x + (n.x*4), v.y + (n.y*4), v.z + (n.z*4) );
        ofPopStyle();
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    cout << key << endl;
    if(key == 61){
        zoom++;
    }
    if(key == 45){
        zoom--;
    }
    
    if(key == 356){
        left--;
    }
    if(key == 358){
        left++;
    }
    if(key ==357){
        up--;;
    }
    if(key == 359){
        up++;
    }
    
    if(key == 3)
    
    if(zoom < 0){
        zoom = 0;
    }
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
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    nNeighbors = 300;
    maxAngle = 95; // degrees
    
    zoom = 1;
    up = 0;
    left = 0;
    showNormals = true;
    
    depthImage.loadImage("depth.png");
    unsigned char * pixels = depthImage.getPixels();
    mesh.enableColors();

    for(int row = 0; row < depthImage.getHeight(); row+=5){
        for(int col = 0; col < depthImage.getWidth(); col+=5){
            int i = row * depthImage.getWidth() + col;
            
            if(pixels[i] != 0){
                ofPoint p =  ofPoint(col, row, pixels[i]*2);
                mesh.addVertex(p);
                mesh.addColor(ofColor(255));
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


    MatrixXf adjacency = MatrixXf::Zero(mesh.getNumVertices(),mesh.getNumVertices());
    cout << "num normals: " << mesh.getNumNormals() << endl;
    
    int nOnes = 0;
    
    for(int i = 0; i < mesh.getVertices().size(); i++){
        vector<ofxANNNeighbor> neighbors = ann.getNeighbors(nNeighbors, mesh.getVertices()[i], false);
        
        ofVec3f vertexNormal = mesh.getNormal(i);

            for(int j = 0; j < neighbors.size(); j++){
//                cout << neighbors[j].x << "," << neighbors[j].y << "," << neighbors[j].z << " d: " << neighbors[j].distance << " idx: " << neighbors[j].idx << endl;
                
                ofVec3f neighborNormal = mesh.getNormal( neighbors[j].idx );
                //  check the neigbors for the angle between the normals
                float angle = vertexNormal.angle(neighborNormal);
                cout << abs(angle) << endl;
                if(abs(angle) < maxAngle){
                    cout << i << "," << j << endl;
                    adjacency(i,neighbors[j].idx) = 1;
                    nOnes++;
                } else {
                    adjacency(i,neighbors[j].idx) = 0;
                }
            }
            //don't forget to write a 1 for the current vertex (Aii)
            adjacency(i,i) = 1;
    }
    cout << nOnes << "/" << (mesh.getNumVertices() * mesh.getNumVertices()) << endl;
    
    float adjacencyDone = ofGetElapsedTimef();
    
    cout << "adjacency matrix built: " << (adjacencyDone - normalsDone) << endl;
    
    // connected components
    
    // initialize a vector of labels with 0s
    vector<int> labels(mesh.getVertices().size());
    
    ofColor labelColor;
    int numNonzero = 0;
    for(int i = 0; i < mesh.getVertices().size(); i++){
        for(int j = 0; j < mesh.getVertices().size(); j++){
            if(adjacency(i,j) != 0){
                numNonzero++;
            }
        }
    }
    
    int n = mesh.getNumVertices() * mesh.getNumVertices();
    
    cout << "num non-zero: " << numNonzero << "/" << n << " (" << (numNonzero/(float)n)  <<")" << endl;
    
    int component = 0;
    queue<int> q;
    for(int i = 0; i < mesh.getVertices().size(); i++){
        
        if(labels[i] == 0){
            component++;
            labelColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255) );
            
            q.push(i);

            while(!q.empty()){
                // pop a vertex off the queue
                int idx =  q.front();
                q.pop();

                // label the current vertex
                labels[idx] = component;
                mesh.setColor(idx, labelColor );
                // get the vertex's neighbors
                vector<ofxANNNeighbor> neighbors = ann.getNeighbors(nNeighbors, mesh.getVertices()[idx], false);
                // for each neighbor j
                for(int j = 0; j < neighbors.size(); j++){
                    // look up the Aij entry in the adjacency matrix
                    ofxANNNeighbor n = neighbors[j];
                    // if that entry is 1 and its label is currently 0
                    if(adjacency(i,n.idx) == 1 && labels[n.idx] == 0){
                        // add it to the queue
                        q.push(n.idx);
                    }
                }
            }
        }
    }
    
    cout << "num components: " << component << endl;
    

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

    if(showNormals){
        drawNormals(mesh);
    }
    
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
    
    if(key == 110){
        showNormals = !showNormals;
    }
    
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
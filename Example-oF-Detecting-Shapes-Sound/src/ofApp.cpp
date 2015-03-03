#include "ofApp.h"

vector <ofPoint> *Blobs;

int num;

int extremoXmenor = 320;
int extremoYmenor = 240;
int extremoXmayor = 0;
int extremoYmayor = 0;

int nMeshes = 1;

const int N = 256;		//Numero de bandas en el espectro

float spectrum[ N ];	//Aca vamos a guardar los datos suavizados del espectro
float Rad;              //Radio de la nube

float Vel = 0.1;		//Parametro de velocidad de los puntos de la nube

int bandRad = 0;		//Indice de banda del espectro, que va a afectar el radio de la nube

int bandVel = 4;		//Indice de banda del espectro que va a afectar el valor de velocidad de los puntos de la nube

bool capture =false;

unsigned char *data;

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setup(){
/*
	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(320,240);
	#else

    vidPlayer.loadMovie("fingers.mov");
        vidPlayer.play();
//	endif
*/
    ofSetFrameRate(60);
    
    vidGrabber.setDeviceID(1);
    
    vidGrabber.initGrabber(320,240);
    
    imagen.loadImage("relax.png");
    bg.loadImage("bg.png");
    colorImg.allocate(320,240);
	
    grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);

	bLearnBakground = true;
	threshold = 40;

    mesh.resize(1);
    line.resize(1);
    sizeMesh.resize(1);
    centroides.resize(1);
    
    sizeMesh[0].x = 0;
    sizeMesh[0].y = 0;
    
    centroides[0].x = 0;
    centroides[0].y = 0;
    
    //sound
    fftFile.setMirrorData(false);
    fftFile.setup();
    
    music.loadSound("surface.wav");
    music.setLoop(true);
    music.play();
    
    string guiPath = "audio.xml";
    gui.setup("audio", guiPath, 20, 20);
    gui.add(audioThreshold.setup("audioThreshold", 1.0, 0.0, 1.0));
    gui.add(audioPeakDecay.setup("audioPeakDecay", 0.915, 0.0, 1.0));
    gui.add(audioMaxDecay.setup("audioMaxDecay", 0.995, 0.0, 1.0));
    gui.add(audioMirror.setup("audioMirror", true));
    gui.loadFromFile(guiPath);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::update(){

    vidGrabber.update();

    bool newFrame = vidGrabber.isFrameNew();
	
    if(capture == true)
    {
    if(newFrame) colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
     
     //ofPixels bg_pixels = bg.getPixelsRef();
     //grayBg.setFromPixels(bg_pixels);
    }else
    {
        ofPixels pixels = imagen.getPixelsRef();
        colorImg.setFromPixels(pixels);
        
        ofPixels bg_pixels = bg.getPixelsRef();
        grayBg.setFromPixels(bg_pixels);
        
    }
    
    
    
    grayImage = colorImg;
    
    
    if (bLearnBakground == true){
        grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
        bLearnBakground = false;
    }
    
    
    // take the abs value of the difference between background and incoming and then threshold:
    grayDiff.absDiff(grayBg, grayImage);
    grayDiff.threshold(threshold);
    
    //Obtener contornos
    contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);
    
    /**********************************************************/
    /**********************************************************/
    /**********************************************************/
    /**********************************************************/
    
    //Procesar y asignar audio
    fftFile.setThreshold(audioThreshold);
    fftFile.setPeakDecay(audioPeakDecay);
    fftFile.setMaxDecay(audioMaxDecay);
    fftFile.setMirrorData(audioMirror);
    fftFile.update();
    
    audioData = new float[1000];
    fftFile.getFftPeakData(audioData, 1000);
    
    for ( int i=0; i<1000; i++ ) {
		audioData[i] *= 0.97;	//Decrecimiento lento
		//audioData[i] = max( audioData[i], val[0] );//elijo el valor maximo entre estros dos y me quedo con ese.
        
	}
    
    //Obtengo el tiempo
	float time = ofGetElapsedTimef();
    
    for(int k=0;k<nMeshes;k++)
    {
	//Cambio los vertices a partir del agregado de ruido en un loop anidado
	for (int y=0; y<sizeMesh[k].y; y++)
    {
		for (int x=0; x<sizeMesh[k].x; x++)
        {
            //Indice de los vertices, igual que antes
			int i = x + sizeMesh[k].x * y;
            //Obtengo el vertice
			ofPoint p = mesh[k].getVertex( i );
            
            ofColor colorMesh = mesh[k].getColor(i);
            
            //Podemos agregar una respuesta senoidal al cambio de las coordenadas x e y.
            float value = ofNoise( sin(time)*x * 0.005, sin(time)*y * 0.005 );
            
			//Cambio la coordenada Z de los vertices mediante
            //value, pero puedo amplificar la profundidad
            if(colorMesh.r < 80 && colorMesh.r >= 0)
            {
                
                p.z = audioData[k*10]*value*100;
                //p.y = audioData[k*10]*value*100;
                //p.x = audioData[k*10]*value*100;
                
                mesh[k].setVertex( i, p );
                
            }
		}
	}
    }
	
    delete[] audioData;
    
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(255);
    
    int w = OFX_FFT_WIDTH;
    int h = OFX_FFT_HEIGHT;
    int x = 20;
    int y = ofGetHeight() - h - 20;
    
    if(gui_fft == true)
    {
        ofSetColor(255);
        vidGrabber.draw(ofGetWidth()/2,0,320,240);
        
        ofPushMatrix();
        
        ofTranslate(0, 200);
        
        colorImg.draw(0,0);
        contourFinder.draw();
        ofPopMatrix();
        
        fftFile.draw(x, y, w, h);
        
        gui.draw();
        
        
        //REPORTES
        ofSetHexColor(0);
        stringstream reportStr;
        reportStr << "press '3' to capture bg" << endl
        << "threshold " << threshold << " (press: +/-)" << endl
        << ", fps: " << ofGetFrameRate()
        ;
        
        ofDrawBitmapString(reportStr.str(), 20, 600);
        
    }
    
	//grayImage.draw(360,20);
	//grayBg.draw(20,280);
	//grayDiff.draw(360,280);
    
    //Se setea vertices y color mediante un loop for anidado
    
    ofPushMatrix();
    //ofTranslate(0, 0);
    
    //ofRotate(ofGetElapsedTimef()*30,0,0,1);
    
    for(int i=0;i<nMeshes;i++)
    {
       
        ofPushMatrix();
    
        ofTranslate(ofGetWidth()/2+centroides[i].x-320/2, ofGetHeight()/2+centroides[i].y-240/2, 200);
        
        ofRotate(30,1,0,0);
    
        ofRotate(sin(ofGetElapsedTimef())*100,0,0,1);
        
        mesh[i].draw();
    
        ofPopMatrix();
     
    }
    ofPopMatrix();
    
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key){
		case ' ':
        {
            
            for(int i=0;i<nMeshes;i++)
            {
            mesh[i].clear();
            line[i].clear();
            }
            //ALGORITMO QUE CONVIERTE UNA FORMA DETECTADA EN UNA MESH
            
            
            //DEFINO BLOBS, QUE ES DONDE VOY A GUARDAR LOS PUNTOS
            //DE LA FORMA QUE SE DETECTA
            Blobs = new vector <ofPoint>[contourFinder.nBlobs];
            
            //OBTENGO LA CANTIDAD DE PUNTOS DE CADA
            //FORMA Y LUEGO GUARDO ESA INFORMACION EN BLOBS
            for (int i = 0; i < contourFinder.nBlobs; i++){
                
                num = contourFinder.blobs[i].nPts;
                
                Blobs[i].resize(num);
                Blobs[i] = contourFinder.blobs[i].pts;
                
                centroides[i] = contourFinder.blobs[i].centroid;

            }
            
            //RESCALO LOS VECTORES QUE CONTIENE LAS MESH, LAS POLYLINES
            //Y UNO QUE ME GUARDA LA CANTIDAD DE MESHES QUE SE VAN A CREAR
            mesh.resize(contourFinder.nBlobs);
            line.resize(contourFinder.nBlobs);
            sizeMesh.resize(contourFinder.nBlobs);
            nMeshes = contourFinder.nBlobs;
            
            //EMPIEZO CON UN LOOP QUE REALIZA LA CONVERSION DE
            //FORMA A MESH PARA TODAS LAS FORMAS
            for(int j = 0; j < contourFinder.nBlobs; j++)
            {
            //OBTENGO EL NUMERO DE PUNTOS DE CADA BLOBS
            num = contourFinder.blobs[j].nPts;
            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            //CALCULO LOS EXTREMOS DE LAS FORMAS, PARA ASI
            //GENERAR UN TAMA„O DE MESH ADECUADO
            for(int i=0; i < num; i++)
            {
                if(extremoXmenor > Blobs[j][i].x)
                {
                    extremoXmenor = Blobs[j][i].x;
                }
                
                if(extremoYmenor > Blobs[j][i].y)
                {
                    extremoYmenor = Blobs[j][i].y;
                }
                
                ///////////////////////////////////////
                
                if(extremoYmayor < Blobs[j][i].y)
                {
                    extremoYmayor = Blobs[j][i].y;
                }
                
                if(extremoXmayor < Blobs[j][i].x)
                {
                    extremoXmayor = Blobs[j][i].x;
                }
                
            }
            
            //TRANSFORMO LOS PUNTOS DE LOS BLOBS EN LINEAS, AJUSTANDO SEGUN
            //LOS EXTREMOS
            for(int i=0; i < num; i++)
            {
                
                line[j].addVertex(ofPoint(Blobs[j][i].x - extremoXmenor, Blobs[j][i].y-extremoYmenor));

            }
            
            
            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            //GENERO LA MALLA DEL TAMA„O CORRESPONDIENTE
            int W = extremoXmayor-extremoXmenor+2;
            int H = extremoYmayor-extremoYmenor+2;
            
                sizeMesh[j].x = W;
                sizeMesh[j].y = H;
                
            //AGREGO VERTICES
            for (int y=0; y<H; y++)
            {
                for (int x=0; x<W; x++)
                {
                    mesh[j].addVertex(ofPoint( (x - W/2), (y - H/2) , 0 ));
                    mesh[j].addColor(255);
                    
                }
            }
            
            //GENERO TRIANGULOS A PARTIR DE ESOS VERTICES
            for (int y=0; y<H-1; y++)
            {
                for (int x=0; x<W-1; x++)
                {
                    
                    int i1 = x + W * y;
                    int i2 = x+1 + W * y;
                    int i3 = x + W * (y+1);
                    int i4 = x+1 + W * (y+1);
                    mesh[j].addTriangle( i1, i2, i3 );
                    mesh[j].addTriangle( i2, i3, i4 );
                    
                }
            }

            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            
            //REUBICO Y COLOREO DENTRO DE LAS MESHES SOLO LOS VERTICES QUE
            //Y COLOREO DEL COLOR QUE QUIERO.
            for(int x=0; x < W; x++)
            {
                for(int y=0; y < H; y++)
                {
                if(line[j].inside(x,y))
                {
                    int indice =x + W * y;
                    
                    mesh[j].setColor(indice, ofFloatColor(0.0));
                    
                    //mesh[j].setVertex(indice, ofPoint(mesh[j].getVertex(indice).x,mesh[j].getVertex(indice).y , 30) );
                    
                    
                    
                }
                else
                {
                
                    int indice =x + W * y;
                    mesh[j].setColor(indice, ofFloatColor(1,0));

                }
                }
            }
            
            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            
                extremoXmenor = 320;
                extremoYmenor = 240;
                extremoXmayor = 0;
                extremoYmayor = 0;
            
            }
            
            
            

            
			break;
    }
        case '+':
            threshold ++;
            if (threshold > 255) threshold = 255;
			break;
        case '-':
            threshold --;
            if (threshold < 0) threshold = 0;
			break;
         
	}
    
    
    if(key == '1') gui_fft = !gui_fft;
    
    if(key == '2') capture = !capture;
    
    if(key == '3') bLearnBakground = true;
    
    if(key == 'f')ofToggleFullscreen();
    
    }





//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
void ofApp::exit()
{
    
    for(int i=0;i<nMeshes;i++)
    {
        mesh[i].clear();
        line[i].clear();
    }

    fftFile.exit();
    
}
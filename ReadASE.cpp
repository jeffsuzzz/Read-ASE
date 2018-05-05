//
// ReadASE.cpp
//
// Created by Hao Su 2018/04/30.
//
// This file takes an ASE file of a model and creates a corresponding header file.
// Vertices[] contains the vertex coordination.
// Normals[] contains the vertex normal vector.
// Elements[] contains the three vertex index that forms a triangle.
//

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>    // setprecision()

using namespace std;

//
// This function changes the size of the pointer
//
float* expandFloatArray(float *arr, int oldsize, int newsize){
    if( oldsize != newsize){
        float *old= new float[oldsize];
        copy( arr, arr + oldsize, old );
        arr = new float[newsize];
        copy( old, old + oldsize, arr);
    } else{
        arr = new float[newsize];
    }
    return arr;
}

void writeFile(string obj, int numberVertex, int numberFaces, float* vertexCoor,
            float* vertexNormal, float* elements, float* vertexUV){
    string outputName = obj + ".h";
    ofstream outfile;
    outfile.open( outputName.c_str() );
    outfile << fixed  << setprecision(6) ;

    // write vertex (x, y, z)
    outfile << "\/\/ \n\/\/ Each group of three values specifies a vertex\n";
    outfile << "\/\/ \nfloat "<< obj << "Vertices[] = {";
    for( int i = 0; i < numberVertex*3; i++){
        if(i%6 == 0){
            outfile << endl;
        }
        outfile << "\t" << vertexCoor[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int "<< obj << "VerticesLength = sizeof("<< obj << "Vertices) / sizeof(float);\n\n";

    // write vertex normal vector
    outfile << "\/\/ \n\/\/ Each group of three values specifies a vertex normal\n";
    outfile << "\/\/ \nfloat "<< obj << "Normals[] = {";
    for( int i = 0; i < numberVertex*3; i++){
        if(i%6 == 0){
            outfile << endl;
        }
        outfile << "\t" << vertexNormal[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int "<< obj << "NormalsLength = sizeof("<< obj <<"Normals) / sizeof(float);\n\n";

    // write vertex UV coordinate
    outfile << "\/\/ \n\/\/ Each pair of values specifies a vertex's texture coordinates\n";
    outfile << "\/\/ \nfloat "<< obj << "UV[] = {";
    for( int i = 0; i < numberVertex * 2; i++){
        if(i%6 == 0){
            outfile << endl;
        }
        outfile << "\t" << vertexUV[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int "<< obj << "UVLength = sizeof("<< obj <<"UV) / sizeof(float);\n\n";

    // write triangle information
    outfile << fixed << setprecision(0);
    outfile << "\/\/ \n\/\/ Each group of three values specifies a triangle\n";
    outfile << "\/\/ \nint "<< obj << "Elements[] = {";
    for( int i = 0; i < numberFaces*3; i++){
        if(i%15 == 0){
            outfile << endl;
        }
        outfile << "\t" << elements[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int "<< obj << "ElementsLength = sizeof("<<obj<<"Elements) / sizeof(float);\n\n";

    outfile.close();
}

int main(){

    string inputName;
    cout << "Name of the ASE file: ";
    cin >> inputName;

    ifstream infile( inputName.c_str() );
    string str;
    string numvertex = "*MESH_NUMVERTEX ";
    string numfaces = "*MESH_NUMFACES ";
    string vertex = "*MESH_VERTEX ";
    string vertexnormal = "*MESH_VERTEXNORMAL ";
    string tVert = "*MESH_TVERT ";

    int numberVertex = 0, numberFaces = 0;
    float *vertexCoor;
    float *vertexNormal;
    float *elements;
    int nvNow, nfNow;    // in case there are multiple meshes
    float *vertexUV;

    int countFace = 0;
    while(getline(infile, str)){
        int foundHead = str.find_first_not_of("\t");
        str = str.substr(foundHead);

        if( str.compare(0, numvertex.length(), numvertex) == 0 ){

            // get number of vertex
            string sub = str.substr(numvertex.length());
            nvNow = numberVertex;
            numberVertex += atoi(sub.c_str());
            vertexCoor = expandFloatArray(vertexCoor, nvNow*3, numberVertex*3);
            vertexNormal = expandFloatArray(vertexNormal, nvNow*3, numberVertex*3);
            vertexUV = expandFloatArray(vertexUV, nvNow*2, numberVertex*2);
        } else if ( str.compare(0, numfaces.length(), numfaces) == 0 ){

            // get number of faces
            string sub = str.substr(numfaces.length());
            nfNow = numberFaces;
            numberFaces += atoi(sub.c_str());
            elements = expandFloatArray(elements, nfNow*3, numberFaces*3);
        }

        // get vertex coordination
        if( str.compare(0, vertex.length(), vertex) == 0 ){
            string sub = str.substr(vertex.length());
            int found = sub.find_first_not_of(" ");
            sub = sub.substr(found);
            found = sub.find_first_not_of("0123456789");
            string indexstr = sub.substr(0, found+1);
            int vertexIndex = nvNow + atoi(indexstr.c_str());
            sub = sub.substr(found+1);

            found = sub.find_first_not_of("0123456789.-");
            string x = sub.substr(0, found);
            float xf = atof(x.c_str());
            vertexCoor[vertexIndex*3] = xf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string y = sub.substr(0, found);
            float yf = atof(y.c_str());
            vertexCoor[vertexIndex*3 + 1] = yf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string z = sub.substr(0, found);
            float zf = atof(z.c_str());
            vertexCoor[vertexIndex*3 + 2] = zf;
        }

        // get triangle elements and normal vector
        if( str.compare(0, vertexnormal.length(), vertexnormal) == 0 ){
            string sub = str.substr(vertexnormal.length());
            int found = sub.find_first_not_of("0123456789");
            string indexstr = sub.substr(0, found+1);
            int vertexIndex = nvNow + atoi(indexstr.c_str());
            elements[countFace] = vertexIndex;
            countFace++;
            sub = sub.substr(found+1);

            found = sub.find_first_not_of("0123456789.-");
            string x = sub.substr(0, found);
            float xf = atof(x.c_str());
            vertexNormal[vertexIndex*3] = xf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string y = sub.substr(0, found);
            float yf = atof(y.c_str());
            vertexNormal[vertexIndex*3 + 1] = yf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string z = sub.substr(0, found);
            float zf = atof(z.c_str());
            vertexNormal[vertexIndex*3 + 2] = zf;
        }

        // If there is texture, get uv coordinate
        if( str.compare(0, tVert.length(), tVert) == 0 ){
            string sub = str.substr(tVert.length());
            int found = sub.find_first_not_of("0123456789");
            string indexstr = sub.substr(0, found+1);
            int vertexIndex = nvNow + atoi(indexstr.c_str());
            sub = sub.substr(found+1);

            found = sub.find_first_not_of("0123456789.-");
            string x = sub.substr(0, found);
            float xf = atof(x.c_str());
            vertexUV[vertexIndex * 2] = xf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string y = sub.substr(0, found);
            float yf = atof(y.c_str());
            vertexUV[vertexIndex * 2 + 1] = yf;
        }

    }

    cout << "number of vertex: " << numberVertex << endl;
    cout << "number of faces: " << numberFaces << endl;
    string obj = inputName.substr(0, inputName.length()-4);
    writeFile(obj, numberVertex, numberFaces, vertexCoor, vertexNormal, elements, vertexUV);
}

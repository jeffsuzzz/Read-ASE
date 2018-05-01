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
#include <iomanip>

using namespace std;

int main(){

    string inputName, outputName;
    cout << "Name of the ASE file: ";
    cin >> inputName;
    cout << "Name of the output header file: ";
    cin >> outputName;
    //string inputName = "spoon2.ASE";
    //string outputName = "spoon.h";
    ifstream infile( inputName.c_str() );
    ofstream outfile;
    string str;
    string numvertex = "*MESH_NUMVERTEX ";
    string numfaces = "*MESH_NUMFACES ";
    string vertex = "*MESH_VERTEX ";
    string vertexnormal = "*MESH_VERTEXNORMAL ";

    int numberVertex;
    float* vertexCoor;
    int numberFaces;
    float* vertexNormal;
    float* elements;
    float* normalIndices;

    int countFace = 0;
    while(getline(infile, str)){
        int foundHead = str.find_first_not_of("\t");
        str = str.substr(foundHead);

        if( str.compare(0, numvertex.length(), numvertex) == 0 ){
            string sub = str.substr(numvertex.length());
            numberVertex = atoi( sub.c_str() );
            vertexCoor = new float[numberVertex * 3];
            vertexNormal = new float[numberVertex * 3];
            cout << "number of vertex: " << numberVertex << endl;

        } else if ( str.compare(0, numfaces.length(), numfaces) == 0 ){
            string sub = str.substr(numfaces.length());
            numberFaces = atoi( sub.c_str() );
            elements = new float[numberFaces * 3];
            normalIndices = new float[numberFaces * 3];
            cout << "number of faces: " << numberFaces << endl;
        }

        if( str.compare(0, vertex.length(), vertex) == 0 ){
            string sub = str.substr(vertex.length());

            int found = sub.find_first_not_of(" ");
            sub = sub.substr(found);
            found = sub.find_first_not_of("0123456789");
            string indexstr = sub.substr(0, found+1);
            int vertexIndex = atoi(indexstr.c_str());
            sub = sub.substr(found+1);

            found = sub.find_first_not_of("0123456789.-");
            string x = sub.substr(0, found);
            float xf = atof(x.c_str());
            vertexCoor[vertexIndex * 3] = xf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string y = sub.substr(0, found);
            float yf = atof(y.c_str());
            vertexCoor[vertexIndex * 3 +1] = yf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string z = sub.substr(0, found);
            float zf = atof(z.c_str());
            vertexCoor[vertexIndex * 3 + 2] = zf;
        }

        if( str.compare(0, vertexnormal.length(), vertexnormal) == 0 ){
            string sub = str.substr(vertexnormal.length());

            int found = sub.find_first_not_of("0123456789");
            string indexstr = sub.substr(0, found+1);
            int vertexIndex = atoi(indexstr.c_str());
            elements[countFace] = vertexIndex;
            countFace++;

            sub = sub.substr(found+1);

            found = sub.find_first_not_of("0123456789.-");
            string x = sub.substr(0, found);
            float xf = atof(x.c_str());
            vertexNormal[vertexIndex * 3] = xf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string y = sub.substr(0, found);
            float yf = atof(y.c_str());
            vertexNormal[vertexIndex * 3 +1] = yf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string z = sub.substr(0, found);
            float zf = atof(z.c_str());
            vertexNormal[vertexIndex * 3 + 2] = zf;
        }

    }

    outfile.open( outputName.c_str() );
    outfile << fixed  << setprecision(6) ;

    outfile << "\/\/ \n\/\/ Each group of three values specifies a vertex\n";
    outfile << "\/\/ \nfloat Vertices[] = {";
    for( int i = 0; i < numberVertex*3; i++){
        if(i%6 == 0){
            outfile << endl;
        }
        outfile << "\t" << vertexCoor[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int VerticesLength = sizeof(Vertices) / sizeof(float);\n\n";

    outfile << "\/\/ \n\/\/ Each group of three values specifies a vertex normal\n";
    outfile << "\/\/ \nfloat Normals[] = {";
    for( int i = 0; i < numberVertex*3; i++){
        if(i%6 == 0){
            outfile << endl;
        }
        outfile << "\t" << vertexNormal[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int NormalsLength = sizeof(Normals) / sizeof(float);\n\n";

    outfile << fixed << setprecision(0);
    outfile << "\/\/ \n\/\/ Each group of three values specifies a triangle\n";
    outfile << "\/\/ \nint Elements[] = {";
    for( int i = 0; i < countFace; i++){
        if(i%15 == 0){
            outfile << endl;
        }
        outfile << "\t" << elements[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int ElementsLength = sizeof(Elements) / sizeof(float);\n\n";


    outfile.close();
}

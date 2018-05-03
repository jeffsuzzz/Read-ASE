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
            float* vertexNormal, float* elements){
	string outputName = obj+".h";
	ofstream outfile;
	outfile.open( outputName.c_str() );
    outfile << fixed  << setprecision(6) ;

    outfile << "\/\/ \n\/\/ Each group of three values specifies a vertex\n";
    outfile << "\/\/ \nfloat "<< obj << "Vertices[] = {";
    for( int i = 0; i < numberVertex; i++){
        if(i%6 == 0){
            outfile << endl;
        }
        outfile << "\t" << vertexCoor[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int "<< obj << "VerticesLength = sizeof("<< obj << "Vertices) / sizeof(float);\n\n";

    outfile << "\/\/ \n\/\/ Each group of three values specifies a vertex normal\n";
    outfile << "\/\/ \nfloat "<< obj << "Normals[] = {";
    for( int i = 0; i < numberVertex; i++){
        if(i%6 == 0){
            outfile << endl;
        }
        outfile << "\t" << vertexNormal[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int "<< obj << "NormalsLength = sizeof("<< obj << "Normals) / sizeof(float);\n\n";

    outfile << fixed << setprecision(0);
    outfile << "\/\/ \n\/\/ Each group of three values specifies a triangle\n";
    outfile << "\/\/ \nint "<< obj << "Elements[] = {";
    for( int i = 0; i < numberFaces; i++){
        if(i%15 == 0){
            outfile << endl;
        }
        outfile << "\t" << elements[i] << ",";
    }
    outfile <<  "\n};\n\n";
    outfile << "int "<< obj << "ElementsLength = sizeof("<< obj << "Elements) / sizeof(float);\n\n";

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

    int numberVertex = 0, numberFaces = 0;
    float *vertexCoor;
    float *vertexNormal;
    float *elements;
    int nvNow, nfNow;    // in case there are multiple meshes

    int countFace = 0;
    while(getline(infile, str)){
        int foundHead = str.find_first_not_of("\t");
        str = str.substr(foundHead);

        if( str.compare(0, numvertex.length(), numvertex) == 0 ){

            // get number of vertex
            string sub = str.substr(numvertex.length());
            nvNow = numberVertex;
            numberVertex += atoi(sub.c_str()) * 3;
            vertexCoor = expandFloatArray(vertexCoor, nvNow, numberVertex);
            vertexNormal = expandFloatArray(vertexNormal, nvNow, numberVertex);
        } else if ( str.compare(0, numfaces.length(), numfaces) == 0 ){

            // get number of faces
            string sub = str.substr(numfaces.length());
            nfNow = numberFaces;
            numberFaces += atoi(sub.c_str()) * 3;
            elements = expandFloatArray(elements, nfNow, numberFaces);
        }

        // get vertex coordination
        if( str.compare(0, vertex.length(), vertex) == 0 ){
            string sub = str.substr(vertex.length());
            int found = sub.find_first_not_of(" ");
            sub = sub.substr(found);
            found = sub.find_first_not_of("0123456789");
            string indexstr = sub.substr(0, found+1);
            int vertexIndex = nvNow + atoi(indexstr.c_str())*3;
            sub = sub.substr(found+1);

            found = sub.find_first_not_of("0123456789.-");
            string x = sub.substr(0, found);
            float xf = atof(x.c_str());
            vertexCoor[vertexIndex ] = xf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string y = sub.substr(0, found);
            float yf = atof(y.c_str());
            vertexCoor[vertexIndex + 1] = yf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string z = sub.substr(0, found);
            float zf = atof(z.c_str());
            vertexCoor[vertexIndex + 2] = zf;
        }

        // get triangle elements and normal vector
        if( str.compare(0, vertexnormal.length(), vertexnormal) == 0 ){
            string sub = str.substr(vertexnormal.length());
            int found = sub.find_first_not_of("0123456789");
            string indexstr = sub.substr(0, found+1);
            int vertexIndex = nvNow + atoi(indexstr.c_str())*3;
            elements[countFace] = vertexIndex/3;
            countFace++;
            sub = sub.substr(found+1);

            found = sub.find_first_not_of("0123456789.-");
            string x = sub.substr(0, found);
            float xf = atof(x.c_str());
            vertexNormal[vertexIndex ] = xf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string y = sub.substr(0, found);
            float yf = atof(y.c_str());
            vertexNormal[vertexIndex + 1] = yf;
            sub = sub.substr(found+1);
            found = sub.find_first_not_of("0123456789.-");
            string z = sub.substr(0, found);
            float zf = atof(z.c_str());
            vertexNormal[vertexIndex + 2] = zf;
        }
    }

    cout << "number of vertex: " << numberVertex << endl;
    cout << "number of faces: " << numberFaces << endl;
    string obj = inputName.substr(0, inputName.length()-4);
    writeFile(obj, numberVertex, numberFaces, vertexCoor, vertexNormal, elements);
}

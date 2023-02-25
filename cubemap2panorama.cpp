#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

int main(int argc, char* argv[])
{
	//OpenGL has its own cubemap implementation (and i guess it is significatly faster), but im to lazy to use it
	Clock clock;
	Time time;


	// getting path to exe
	string exe_path = argv[0];
	exe_path.erase(exe_path.rfind("\\") + 1);
	ofstream fout(exe_path + "output.txt");


	// checking recieved arguements
	if (argc != 7) {
		fout << "invalid number of arguements. You should drag&drop 6 images on .exe file" << endl;
		fout.close();
		return EXIT_FAILURE;
	}
	fout << "recivied arguements: " << endl;
	for (int i = 0; i < argc; i++) {
		fout << "#" + to_string(i) + ": " << argv[i] << endl;
	}


	// sort recivied images by a number after symbol "#"
	vector<string> path(6);
	for (int i = 1; i <= 6; i++) {
		path[i - 1] = argv[i];

		if (path[i - 1].find(".") == string::npos || path[i - 1].find("#") == 0) {
			fout << "invalid input file name format. name shoud be something like ***1.tga. no \".\" symbol found or it is first symbol in name" << endl;
			fout.close();
			return EXIT_FAILURE;
		}
		char check = path[i - 1][path[i - 1].find(".") - 1];
		if (check < 49 || check > 54) {
			fout << "invalid input file name format. name shoud be something like ***1.tga. bad symbol before \".\"." << endl;
			fout.close();
			return EXIT_FAILURE;
		}
	}
	sort(path.begin(), path.end(), [](string a, string b) {return a[a.find(".") - 1] < b[b.find(".") - 1]; } );


	// reading output file name and size of output texture
	ifstream fin(exe_path + "input.txt");
	string temp, output_filename = "output.tga";
	unsigned output_y = 2048; // Height of the ouput texture
	if (fin.is_open()) {
		getline(fin, temp);
		output_y = stoi(temp);
		getline(fin, output_filename);
	}
	fin.close();


	// creating output rendertexture
	RenderTexture output;
	if (!output.create(Vector2u(output_y * 2, output_y))) {
		fout << "unable to create output rendertexture" << endl;
		fout.close();
		return EXIT_FAILURE;
	}
	output.clear(Color(0, 0, 0, 255));
	fout << "loading parametrs and creating output texture in milliseconds: " << to_string(clock.getElapsedTime().asMilliseconds()) << endl;


	// loading shader
	time = clock.getElapsedTime();
	Shader c2p;
	if (!c2p.loadFromFile(exe_path + "c2p.txt", Shader::Fragment)) {
		fout << "unable to load shader from c2p.txt" << endl;
		fout.close();
		return EXIT_FAILURE;
	}
	fout << "shader compiling time in milliseconds: " << to_string((clock.getElapsedTime() - time).asMilliseconds()) << endl;


	// loading textures and passing them in shader
	time = clock.getElapsedTime();
	vector<Texture*> cubemap;
	for (int i = 0; i < 6; i++) {
		Texture* t = new Texture();
		if (!t->loadFromFile(path[i])) {
			fout << "error loading cubemap texture " << path[i] << endl;
			fout.close();
			return EXIT_FAILURE;
		};
		c2p.setUniform("cubemap" + to_string(i), *t);
		cubemap.push_back(t);
	}
	fout << "loading textures and passing them to shader time in milliseconds: " << to_string((clock.getElapsedTime() - time).asMilliseconds()) << endl;


	// rendering panorama
	time = clock.getElapsedTime();
	Sprite s;
	s.setTexture(output.getTexture());
	s.setTextureRect(IntRect(Vector2i(0, 0), Vector2i(output_y * 2, output_y)));
	output.draw(s, &c2p);
	fout << "creating panorama time in milliseconds: " << to_string((clock.getElapsedTime() - time).asMilliseconds()) << endl;


	// saving result into a file
	time = clock.getElapsedTime();
	if (!output.getTexture().copyToImage().saveToFile(exe_path + output_filename)) {
		fout << "can`t save output image to file" << endl;
		fout.close();
		return EXIT_FAILURE;
	}
	fout << "saving image to file time in milliseconds: " << to_string((clock.getElapsedTime() - time).asMilliseconds()) << endl;


	// some info printing into file
	string max_size = to_string(Texture::getMaximumSize());
	fout << "total time in milliseconds: " << to_string(clock.getElapsedTime().asMilliseconds()) << endl;
	fout << "maximum texture size: " << max_size << "x" << max_size << endl;


	// free memory, close file
	fout.close();
	for (int i = 0; i < 6; i++) {
		delete cubemap[i];
	}
	return EXIT_SUCCESS;
}

#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{
	//OpenGL has its own cubemap implementation (and i guess it is significatly faster), but im to lazy to use it
	sf::Clock clock;
	sf::Time time;

	// getting path to exe
	std::string exe_path = argv[0];
	exe_path.erase(exe_path.rfind("\\") + 1);
	FILE* stream;
	freopen_s(&stream, (exe_path + "std_output.txt").c_str(), "w", stdout);

	// checking recieved arguements
	if (argc != 7) {
		std::cout << "invalid number of arguements. You should drag&drop 6 images on .exe file \n";
		return EXIT_FAILURE;
	}
	std::cout << "recivied arguements: \n";
	for (int i = 0; i < argc; i++) {
		std::cout << "#" + std::to_string(i) + ": " << argv[i] << std::endl;
	}

	// sort recivied images by a number after symbol "#"
	std::vector<std::string> path(6);
	for (int i = 1; i <= 6; i++) {
		path[i - 1] = argv[i];

		if (path[i - 1].find(".") == std::string::npos || path[i - 1].find("#") == 0) {
			std::cout << "invalid input file name format. name shoud be something like ***1.tga. no \".\" symbol found or it is first symbol in name \n";
			return EXIT_FAILURE;
		}
		char check = path[i - 1][path[i - 1].find(".") - 1];
		if (check < 49 || check > 54) {
			std::cout << "invalid input file name format. name shoud be something like ***1.tga. bad symbol before \".\". \n";
			return EXIT_FAILURE;
		}
	}
	sort(path.begin(), path.end(), [](std::string a, std::string b) {return a[a.find(".") - 1] < b[b.find(".") - 1]; } );

	// reading output file name and size of output texture
	std::ifstream fin(exe_path + "input.txt");
	std::string temp, output_filename = "output.tga";
	unsigned output_y = 1024; // Height of the ouput texture
	if (fin.is_open()) {
		getline(fin, temp);
		output_y = stoi(temp);
		getline(fin, output_filename);
	}
	fin.close();

	// creating output rendertexture
	sf::RenderTexture output;
	if (!output.create(sf::Vector2u(output_y * 2, output_y))) {
		std::cout << "unable to create output rendertexture \n";
		return EXIT_FAILURE;
	}
	output.clear(sf::Color(0, 0, 0, 255));
	std::cout << "loading parametrs and creating output texture in milliseconds: " << std::to_string(clock.getElapsedTime().asMilliseconds()) << std::endl;

	// loading shader
	time = clock.getElapsedTime();
	sf::Shader c2p;
	if (!c2p.loadFromFile(exe_path + "c2p.txt", sf::Shader::Fragment)) {
		std::cout << "unable to load shader from c2p.txt \n";
		return EXIT_FAILURE;
	}
	std::cout << "shader compiling time in milliseconds: " << std::to_string((clock.getElapsedTime() - time).asMilliseconds()) << std::endl;

	// loading textures and passing them in shader
	time = clock.getElapsedTime();
	std::vector<sf::Texture*> cubemap;
	for (int i = 0; i < 6; i++) {
		sf::Texture* t = new sf::Texture();
		if (!t->loadFromFile(path[i])) {
			std::cout << "error loading cubemap texture " << path[i] << std::endl;
			return EXIT_FAILURE;
		};
		c2p.setUniform("cubemap" + std::to_string(i), *t);
		cubemap.push_back(t);
	}
	std::cout << "loading textures and passing them to shader time in milliseconds: " << std::to_string((clock.getElapsedTime() - time).asMilliseconds()) << std::endl;

	// rendering panorama
	time = clock.getElapsedTime();
	sf::Sprite s;
	s.setTexture(output.getTexture());
	s.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(output_y * 2, output_y)));
	output.draw(s, &c2p);
	std::cout << "creating panorama time in milliseconds: " << std::to_string((clock.getElapsedTime() - time).asMilliseconds()) << std::endl;

	// saving result into a file
	time = clock.getElapsedTime();
	if (!output.getTexture().copyToImage().saveToFile(exe_path + output_filename)) {
		std::cout << "can`t save output image to file \n";
		return EXIT_FAILURE;
	}
	std::cout << "saving image to file time in milliseconds: " << std::to_string((clock.getElapsedTime() - time).asMilliseconds()) << std::endl;
	std::cout << "total time in milliseconds: " << std::to_string(clock.getElapsedTime().asMilliseconds()) << std::endl;

	// printing some info
	std::string max_size = std::to_string(sf::Texture::getMaximumSize());
	std::cout << "maximum texture size: " << max_size << "x" << max_size << std::endl;

	// free memory, close file
	for (int i = 0; i < 6; i++) {
		delete cubemap[i];
	}
	return EXIT_SUCCESS;
}

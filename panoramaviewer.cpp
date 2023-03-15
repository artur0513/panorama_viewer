#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <cmath>

const double PI = 3.14159265359;

int main(int argc, char* argv[])
{
	// getting path to exe
	std::string exe_path = argv[0];
	exe_path.erase(exe_path.rfind("\\") + 1);
	FILE* stream;
	freopen_s(&stream, (exe_path + "std_output.txt").c_str(), "w", stdout);

	if (argc != 2) {
		std::cout << "invalid number of arguements. You should drag&drop 1 panoramic image on .exe file \n";
		return EXIT_FAILURE;
	}

	sf::Texture panorama;
	if (!panorama.loadFromFile(argv[1])) {
		std::cout << "error loading panorama image " << argv[1] << std::endl;
		return EXIT_FAILURE;
	};
	panorama.setSmooth(true);
	
	sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "panorama viewer", sf::Style::Fullscreen);
	sf::Vector2f screen_res;
	float aspect_ratio;
	window.setFramerateLimit(60);

	sf::Vector2f fov;
	fov.y = PI / 2.0;
	sf::Vector2f sphere_coords = sf::Vector2f(0.f, 0.0f);

	sf::Shader shader;
	if (!shader.loadFromFile(exe_path + "pano.txt", sf::Shader::Fragment)) {
		std::cout << "error loading shader " << exe_path + "pano.txt \n";
		return EXIT_FAILURE;
	}
	shader.setUniform("panorama", panorama);

	auto fix_variables = [&]() { // Функция вызываемся при ресайзе окна
		screen_res = sf::Vector2f(float(window.getSize().x), float(window.getSize().y));
		aspect_ratio = screen_res.x / screen_res.y;
		shader.setUniform("screen_res", screen_res);
		fov.x = atanf(tanf(fov.y * 0.5f) * aspect_ratio) * 2.0;
	};
	fix_variables();

	sf::Sprite s;
	s.setTexture(panorama);
	s.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(screen_res)));

	bool mouse_button_pressed = false, fullscreen = true, texture_smooth = true;
	sf::Vector2i prev_mpos = sf::Mouse::getPosition();
	sf::Vector2f prev_sphere_coords = sphere_coords;
	while (window.isOpen()) {
		shader.setUniform("sphere_coords", sphere_coords);
		shader.setUniform("fov", fov);

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape)
					window.close();

				if (event.key.code == sf::Keyboard::F11) {
					fullscreen = !fullscreen;
					if (fullscreen) {
						window.create(sf::VideoMode(sf::VideoMode::getDesktopMode()), "panorama viewer", sf::Style::Fullscreen);
						fix_variables();
					}
					else
						window.create(sf::VideoMode(sf::VideoMode::getDesktopMode()), "panorama viewer");
				}

				if (event.key.code == sf::Keyboard::Space) {
					texture_smooth = !texture_smooth;
					panorama.setSmooth(texture_smooth);
				}
			}

			if (event.type == sf::Event::Resized) {
				fix_variables();
			}

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed)
			{
				mouse_button_pressed = true;
				prev_mpos = sf::Mouse::getPosition();
				prev_sphere_coords = sphere_coords;
			}

			if (event.type == sf::Event::MouseButtonReleased)
				mouse_button_pressed = false;

			if (event.type == sf::Event::MouseWheelScrolled) {
				fov.y *= powf(1.04, -event.mouseWheelScroll.delta);
				fov.x = atanf(tanf(fov.y * 0.5f) * aspect_ratio) * 2.0;
				if (fov.x > PI * 0.95) {
					fov.x = PI * 0.95;
					fov.y = atanf(tanf(fov.x * 0.5f) / aspect_ratio) * 2.0;
				}
			}
		}

		if (mouse_button_pressed) {
			sphere_coords.x = prev_sphere_coords.x - float(sf::Mouse::getPosition().x - prev_mpos.x) / float(screen_res.x) * fov.x * 1.2;
			sphere_coords.y = prev_sphere_coords.y - float(prev_mpos.y - sf::Mouse::getPosition().y) / float(screen_res.y) * fov.y;
			sphere_coords.y = std::clamp(sphere_coords.y, float(- PI * 0.5), float(PI * 0.5));
		}

		window.clear(sf::Color(0, 0, 0, 255));
		window.draw(s, &shader);
		window.display();
		
	}
}

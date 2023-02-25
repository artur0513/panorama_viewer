#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;
using namespace std;

const double PI = 3.14159265359;

int main(int argc, char* argv[])
{
	// getting path to exe
	string exe_path = argv[0];
	exe_path.erase(exe_path.rfind("\\") + 1);
	ofstream fout(exe_path + "output.txt");
	if (argc != 2) {
		fout << "invalid number of arguements. You should drag&drop 1 panoramic image on .exe file" << endl;
		fout.close();
		return EXIT_FAILURE;
	}

	Texture panorama;
	if (!panorama.loadFromFile(argv[1])) {
		fout << "error loading panorama image " << argv[1] << endl;
		fout.close();
		return EXIT_FAILURE;
	};
	panorama.setSmooth(true);

	Vector2f screen_res = Vector2f(VideoMode::getDesktopMode().size.x, VideoMode::getDesktopMode().size.y);
	float aspect_ratio = screen_res.x / screen_res.y;
	RenderWindow window(VideoMode(VideoMode::getDesktopMode()), "panorama viewer", Style::Fullscreen);
	window.setFramerateLimit(60);

	Vector2f fov;
	fov.y = PI / 2.0;
	fov.x = atanf(tanf(fov.y * 0.5f) * aspect_ratio) * 2.0;
	Vector2f sphere_coords = Vector2f(0.f, 0.0f);

	Shader shader;
	if (!shader.loadFromFile(exe_path + "pano.txt", Shader::Fragment)) {
		fout << "error loading shader " << exe_path + "pano.txt" << endl;
		fout.close();
		return EXIT_FAILURE;
	}
	shader.setUniform("panorama", panorama);
	shader.setUniform("screen_res", screen_res);

	Sprite s;
	s.setTexture(panorama);
	s.setTextureRect(IntRect(Vector2i(0, 0), Vector2i(screen_res)));

	bool mouse_button_pressed = false, fullscreen = true, texture_smooth = true;
	Vector2i prev_mpos = Mouse::getPosition();
	Vector2f prev_sphere_coords = sphere_coords;
	while (window.isOpen()) {
		shader.setUniform("sphere_coords", sphere_coords);
		shader.setUniform("fov", fov);

		Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape)
					window.close();

				if (event.key.code == sf::Keyboard::F11) {
					fullscreen = !fullscreen;
					if (fullscreen) {
						window.create(VideoMode(VideoMode::getDesktopMode()), "panorama viewer", Style::Fullscreen);
						screen_res = Vector2f(float(window.getSize().x), float(window.getSize().y));
						aspect_ratio = screen_res.x / screen_res.y;
						shader.setUniform("screen_res", screen_res);
						fov.x = atanf(tanf(fov.y * 0.5f) * aspect_ratio) * 2.0;
					}
					else
						window.create(VideoMode(VideoMode::getDesktopMode()), "panorama viewer");
				}

				if (event.key.code == sf::Keyboard::Space) {
					texture_smooth = !texture_smooth;
					panorama.setSmooth(texture_smooth);
				}
			}

			if (event.type == sf::Event::Resized) {
				screen_res = Vector2f(float(window.getSize().x), float(window.getSize().y));
				aspect_ratio = screen_res.x / screen_res.y;
				shader.setUniform("screen_res", screen_res);
				fov.x = atanf(tanf(fov.y * 0.5f) * aspect_ratio) * 2.0;
			}

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed)
			{
				mouse_button_pressed = true;
				prev_mpos = Mouse::getPosition();
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
			sphere_coords.x = prev_sphere_coords.x - float(Mouse::getPosition().x - prev_mpos.x) / float(screen_res.x) * fov.x * 1.2;
			sphere_coords.y = prev_sphere_coords.y - float(prev_mpos.y - Mouse::getPosition().y) / float(screen_res.y) * fov.y;

			sphere_coords.y = clamp(sphere_coords.y, float(- PI * 0.5), float(PI * 0.5));
		}

		window.clear(Color(0, 0, 0, 255));
		window.draw(s, &shader);
		window.display();
		
	}
}

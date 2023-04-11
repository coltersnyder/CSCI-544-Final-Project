#include "scene.h"
#include "scenerunner.h"
#include "scenebasic.h"

#include <memory>

static std::map< std::string, std::string > sceneData = {
		{"basic", "Basic scene."}
};

int main(int argc, char *argv[])
{
	SceneRunner runner("Assignment 2 ", 500, 500);

	std::unique_ptr<Scene> scene;

	scene = std::unique_ptr<Scene>(new SceneBasic()); // Add our main scene

	return runner.run(std::move(scene));
}



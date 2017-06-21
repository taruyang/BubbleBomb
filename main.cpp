#include "studio.h"

using namespace std;
using namespace gl;

int main()
{
    Studio  studio;

    studio.ScreenSetting(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    glm::vec3 camPos(0.f, 0.f, -25.f);
	glm::vec3 lookAt(0.f, 0.f, 0.f);
    studio.CameraSetting(camPos, lookAt);

    glm::vec3 lightPos(0.f, 10.f, 0.f);
    studio.LightingSetting(lightPos);

    studio.Ready();

    studio.Shoot();

    return 0;
}

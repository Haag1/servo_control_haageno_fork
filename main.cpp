#include <iostream>

#include <threepp/threepp.hpp>

using namespace threepp;

int main()
{
    OBJLoader loader;

    const auto bottom = loader.load("data/bottom.obj");
    const auto upper = loader.load("data/upper.obj");
    const auto servo = loader.load("data/servo.obj");

    Group servoAssembly;
    servoAssembly.add(bottom);
    servoAssembly.add(upper);
    servoAssembly.add(servo);
    servoAssembly.scale *= 10;


    Scene scene;
    scene.background = Color::aliceblue;

    const auto light = AmbientLight::create();
    scene.add(light);

    Canvas canvas("Servo control");
    GLRenderer renderer(canvas.size());

    PerspectiveCamera camera(60, canvas.aspect(), 0.1, 1000.);
    camera.position.z = -5;

    scene.add(servoAssembly);

    OrbitControls controls(camera, canvas);


    Clock clock;
    canvas.animate([&]
    {

        renderer.render(scene, camera);
    });

    return 0;
}

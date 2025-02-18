#include <threepp/threepp.hpp>

#include <threepp/helpers/CameraHelper.hpp>
#include <threepp/loaders/CubeTextureLoader.hpp>
#include <threepp/renderers/GLRenderTarget.hpp>

#include <opencv2/opencv.hpp>

#include <cmath>


using namespace threepp;

auto createSprite(const std::shared_ptr<Texture> &texture) {
    auto spriteMaterial = MeshBasicMaterial::create({{"map", texture}});
    auto sprite = Mesh::create(PlaneGeometry::create(), spriteMaterial);
    sprite->scale.set(2, 2, 1);

    return sprite;
}

class PanTiltMechanism : public Group {
public:
    PanTiltMechanism(): camera_(60, 1, 0.01, 100) {
        camera_.position.x = 0.03;
        camera_.rotateY(math::degToRad(-90));
        camera_.rotateZ(math::degToRad(180));

        OBJLoader loader;
        const auto bottom = loader.load("data/pantilt/bottom.obj");
        const auto upper = loader.load("data/pantilt/upper.obj");
        const auto servo = loader.load("data/pantilt/servo.obj");

        bottom->position.y = 0.9;
        bottom->rotateZ(math::degToRad(90));
        bottom->scale *= 10;
        upper->name = "upper";

        auto material = MeshStandardMaterial::create();
        material->color = Color::orange;
        auto baseGeometry = CylinderGeometry::create(0.5, 0.5, 0.5);
        baseGeometry->applyMatrix4(Matrix4().setPosition(0, 0.25, 0));
        auto base = Mesh::create(baseGeometry, material);

        add(base);
        base->add(bottom);
        bottom->add(servo);
        servo->add(upper);
        upper->add(camera_);
    }

    void setTilt(float angle) {
        getObjectByName("upper")->rotation.z = angle;
    }

    void setPan(float angle) {
        children.front()->rotation.y = angle;
    }

    PerspectiveCamera &getCamera() {
        return camera_;
    }

private:
    PerspectiveCamera camera_;
};

int main() {
    Canvas canvas("Servo control");
    GLRenderer renderer(canvas.size());
    renderer.autoClear = false;

    Scene scene;

    const auto light = DirectionalLight::create();
    scene.add(light);

    PerspectiveCamera camera(60, canvas.aspect(), 0.1, 1000.);
    camera.position.z = -5;

    std::filesystem::path path("data/Bridge2");
    std::array<std::filesystem::path, 6> urls{
        // clang-format off
        path / "posx.jpg", path / "negx.jpg",
        path / "posy.jpg", path / "negy.jpg",
        path / "posz.jpg", path / "negz.jpg"
        // clang-format on
    };

    CubeTextureLoader cubeTextureLoader{};
    auto reflectionCube = cubeTextureLoader.load(urls);
    scene.background = reflectionCube;

    PanTiltMechanism panTilt;
    scene.add(panTilt);

    auto cameraHelper = CameraHelper::create(panTilt.getCamera());
    scene.add(cameraHelper);

    OrbitControls controls(camera, canvas);

    auto grid = GridHelper::create();
    scene.add(grid);

    Scene orthoScene;
    OrthographicCamera orthoCamera(-1, 1, 1, -1, 1, 10);
    orthoCamera.position.z = 1;

    cv::namedWindow("PanTilt", cv::WINDOW_NORMAL);

    Clock clock;
    canvas.animate([&] {
        const auto sine = 45.f * std::sin(math::TWO_PI * 0.1f + clock.getElapsedTime() + 0);
        panTilt.setPan(math::degToRad(sine));
        panTilt.setTilt(math::degToRad(90));

        renderer.clear();
        cameraHelper->visible = false;
        renderer.render(scene, panTilt.getCamera());
        cameraHelper->visible = true;

        auto pixels = renderer.readRGBPixels();

        renderer.clear();
        renderer.render(scene, camera);

        cv::Mat image = cv::Mat(renderer.size().height(), renderer.size().width(), CV_8UC3, pixels.data());

        // OpenGL stores pixels bottom-to-top, OpenCV is top-to-bottom, so flip
        cv::flip(image, image, 0);

        // Convert from RGB to BGR (since OpenCV expects BGR by default)
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imshow("PanTilt", image);

    });

    return 0;
}

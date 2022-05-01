#include <CGLShaderInit.h>
#include <CGLShader.h>
#include <CGLBuffer.h>
#include <CGLTexture.h>

#include <CImport3DS.h>
#include <CImportObj.h>
#include <CImportScene.h>
#include <CImportV3D.h>
#include <CImportX3D.h>
#include <CGeomTexture.h>

#include <CGLMatrix3D.h>
#include <CGLCamera.h>

#define Q(x) #x
#define QUOTE(x) Q(x)

void mouse_callback (GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput   (GLFWwindow *window);

CGLCamera camera(CGLVector3D(0.0f, 0.0f, 1.0f));

// lighting
CGLVector3D lightPos(1.0f, 1.0f, 1.0f);

bool s_animate = false;

int
main(int argc, char **argv)
{
  std::string modelName = "models/v3d/Lightplane.V3D";
  std::string format    = "v3d";

  bool capture = false;
  bool flipYZ  = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if      (arg == "capture")
        capture = true;
      else if (arg == "3ds" || arg == "obj" || arg == "scene" || arg == "v3d" || arg == "x3d") {
        ++i;

        if (i < argc) {
          format    = arg;
          modelName = std::string(argv[i]);
        }
        else
          std::cerr << "Missing filename for " << argv[i] << "\n";
      }
      else if (arg == "flip_yz")
        flipYZ = true;
      else if (arg == "h" || arg == "help") {
        std::cout << "CGLNewModel [-capture] [-flip_yz] "
                     "[[-3ds|-obj|-scene|-v3d|-x3d] <filename>]\n";
        return 0;
      }
      else {
        std::cerr << "Invalid option " << argv[i] << "\n";
      }
    }
    else
      std::cerr << "Invalid arg " << argv[i] << "\n";
  }

  //---

  CGLApp::init();

  auto *window = CGLApp::createWindow(1000, 1000);

  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback   (window, scroll_callback);

  // tell GLFW to capture our mouse
  if (capture)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // configure global opengl state
  CGLApp::enableDepthTest();

  // build and compile our shader program
  CGLShader modelShader, lightShader;

  std::string shaderDir = QUOTE(SHADER_DIR);

  if (! modelShader.loadVertexFile(shaderDir + "/model.vs"))
    exit(1);

  if (! modelShader.loadFragmentFile(shaderDir + "/model.fs"))
    exit(1);

  if (! lightShader.loadVertexFile(shaderDir + "/light.vs"))
    exit(1);

  if (! lightShader.loadFragmentFile(shaderDir + "/light.fs"))
    exit(1);

  // link shaders
  modelShader.attachVertex  ();
  modelShader.attachFragment();

  if (! modelShader.link())
    exit(1);

  lightShader.attachVertex  ();
  lightShader.attachFragment();

  if (! lightShader.link())
    exit(1);

  //---

  // import model
  CImportBase *import = nullptr;

  if      (format == "3ds")
    import = new CImport3DS;
  else if (format == "obj")
    import = new CImportObj;
  else if (format == "v3d")
    import = new CImportV3D;
  else if (format == "scene")
    import = new CImportScene;
  else if (format == "x3d")
    import = new CImportX3D;
  else
    std::cerr << "Invalid format " << format << "\n";

  if (import) {
    CFile file(modelName);

    if (! import->read(file))
      std::cerr << "Failed to read model '" << modelName << "'\n";
  }

  //---

  // set up vertex data (and buffer(s)) and configure vertex attributes
  struct FaceData {
    int         pos     { 0 };
    int         len     { 0 };
    CGLTexture *texture { nullptr };
  };

  using FaceDatas = std::vector<FaceData>;

  FaceDatas faceDatas;

  using ModelBuffers = std::vector<CGLBuffer *>;

  ModelBuffers modelBuffers;

  using GLTextures = std::map<int, CGLTexture *>;

  GLTextures glTextures;

  CVector3D sceneSize(1, 1, 1);
  CPoint3D  sceneCenter(0 , 0, 0);

  if (import) {
    auto &scene = import->getScene();

    CBBox3D bbox;

    scene.getBBox(bbox);

    sceneSize   = bbox.getSize();
    sceneCenter = bbox.getCenter();

    std::cerr << sceneCenter.getX() << " " << sceneCenter.getY() << " " <<
                 sceneCenter.getZ() << "\n";

    for (auto *object : scene.getObjects()) {
      auto *modelBuffer = new CGLBuffer;

      const auto &faces = object->getFaces();

      int pos = 0;

      for (const auto *face : faces) {
        FaceData faceData;

        //---

        const auto &color = face->getColor();

        auto *texture = face->getTexture();

        if (texture) {
          auto pt = glTextures.find(texture->id());

          if (pt == glTextures.end()) {
            const auto &image = texture->image()->image();

            auto *glTexture = new CGLTexture(image);

            pt = glTextures.insert(pt, GLTextures::value_type(texture->id(), glTexture));
          }

          faceData.texture = (*pt).second;
        }

      //const auto &ambient   = face->getMaterial().getAmbient  ();
      //const auto &diffuse   = face->getMaterial().getDiffuse  ();
      //const auto &specular  = face->getMaterial().getSpecular ();
      //double      shininess = face->getMaterial().getShininess();

        //const auto &normal = face->getNormal();
        CVector3D normal;
        face->calcNormal(normal);

        const auto &vertices = face->getVertices();

        faceData.pos = pos;
        faceData.len = vertices.size();

        for (const auto &v : vertices) {
          auto &vertex = object->getVertex(v);

          const auto &model = vertex.getModel();

          if (! flipYZ) {
            modelBuffer->addPoint(model.x, model.y, model.z);
            modelBuffer->addNormal(normal.getX(), normal.getY(), normal.getZ());
          }
          else {
            modelBuffer->addPoint(model.x, model.z, model.y);
            modelBuffer->addNormal(normal.getX(), normal.getZ(), normal.getY());
          }

          modelBuffer->addColor(color.getRed(), color.getGreen(), color.getBlue());

          if (faceData.texture) {
            const auto &tpoint = vertex.getTextureMap();

            modelBuffer->addTexturePoint(tpoint.x, tpoint.y);
          }
          else
            modelBuffer->addTexturePoint(0.0, 0.0);
        }

        pos += faceData.len;

        faceDatas.push_back(faceData);
      }

      modelBuffer->load();

      modelBuffers.push_back(modelBuffer);
    }
  }

  double sceneScale = 1.0/std::max(std::max(sceneSize.getX(), sceneSize.getY()), sceneSize.getZ());
std::cerr << "Scene Scale " << sceneScale << "\n";

  //---

  // set up vertex data (and buffer(s)) and configure vertex attributes
  CGLBuffer lightBuffer;

  lightBuffer.addPoint(-0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f, -0.5f);
  lightBuffer.addPoint(-0.5f,  0.5f, -0.5f);
  lightBuffer.addPoint(-0.5f, -0.5f, -0.5f);

  lightBuffer.addPoint(-0.5f, -0.5f,  0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f,  0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f, -0.5f,  0.5f);

  lightBuffer.addPoint(-0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f,  0.5f, -0.5f);
  lightBuffer.addPoint(-0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint(-0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint(-0.5f, -0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f,  0.5f,  0.5f);

  lightBuffer.addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f,  0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f,  0.5f);

  lightBuffer.addPoint(-0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f,  0.5f);
  lightBuffer.addPoint( 0.5f, -0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f, -0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f, -0.5f, -0.5f);

  lightBuffer.addPoint(-0.5f,  0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f, -0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint( 0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f,  0.5f,  0.5f);
  lightBuffer.addPoint(-0.5f,  0.5f, -0.5f);

  lightBuffer.load();

  //---

  // camera
  camera.setLastPos(CGLApp::screen_width/2.0f, CGLApp::screen_height/2.0f);

  //---

  float lastAngle = 0.0;

  // render loop
  CGLApp::mainLoop(window, [&]() {
    // per-frame time logic
    camera.updateFrameTime();

    // input
    processInput(window);

    // clear screen
    CGLApp::clear(0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //---

    auto aspect     = float(CGLApp::screen_width)/double(CGLApp::screen_height);
    auto projection = CGLMatrix3D::perspective(camera.zoom(), aspect, 0.1f, 100.0f);

    auto view = camera.getViewMatrix();

    auto modelAngle = (s_animate ? float(glfwGetTime()) : lastAngle);

    //---

    // setup model shader
    for (auto *modelBuffer : modelBuffers) {
      modelBuffer->bind();

      modelShader.use();

//    modelShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
      modelShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
      modelShader.setVec3("lightPos", lightPos);
      modelShader.setVec3("viewPos", camera.position());

      // pass projection matrix to shader (note that in this case it could change every frame)
      modelShader.setMatrix("projection", projection);

      // camera/view transformation
      modelShader.setMatrix("view", view);

      // model rotation
      auto modelMatrix = CGLMatrix3D::identity();
      modelMatrix.scaled(sceneScale, sceneScale, sceneScale);
      modelMatrix.rotated(modelAngle, CGLVector3D(0.5f, 1.0f, 0.0f));
      modelMatrix.translated(-sceneCenter.getX(), -sceneCenter.getY(), -sceneCenter.getZ());
      modelShader.setMatrix("model", modelMatrix);

      // render model
      for (const auto &faceData : faceDatas) {
        bool textured = faceData.texture;

        modelShader.setBool("textured", textured);

        if (textured) {
          glActiveTexture(GL_TEXTURE0);

          faceData.texture->bind();

          modelShader.setBool("texture_diffuse", 0);
        }

        glDrawArrays(GL_TRIANGLE_FAN, faceData.pos, faceData.len);
      }

      modelBuffer->unbind();
    }

    //---

    // setup light shader
    lightBuffer.bind();

    lightShader.use();

    lightShader.setMatrix("projection", projection);
    lightShader.setMatrix("view", view);

    auto lightMatrix = CGLMatrix3D::translation(lightPos);
    lightMatrix.scaled(0.2, 0.2, 0.2);
    lightShader.setMatrix("model", lightMatrix);

    // draw light
    lightBuffer.drawTriangles();

    lightBuffer.unbind();

    //---

    lastAngle = modelAngle;
  });

  CGLApp::term();

  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame
// and react accordingly
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    s_animate = ! s_animate;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(CGLCamera::Movement::FORWARD, camera.deltaTime());
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(CGLCamera::Movement::BACKWARD, camera.deltaTime());
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(CGLCamera::Movement::LEFT, camera.deltaTime());
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(CGLCamera::Movement::RIGHT, camera.deltaTime());
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow*, double xpos, double ypos)
{
  if (camera.isFirstPos()) {
    camera.setLastPos(xpos, ypos);

    camera.setFirstPos(false);
  }

  float xoffset, yoffset;

  camera.deltaPos(xpos, ypos, xoffset, yoffset);

  camera.setLastPos(xpos, ypos);

  camera.processMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow*, double /*xoffset*/, double yoffset)
{
  camera.processMouseScroll(yoffset);
}

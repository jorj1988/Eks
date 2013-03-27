#include "XShader.h"
#include "XGeometry.h"
#include "XRasteriserState.h"
#include "XTransform.h"


namespace Eks
{

namespace Demo
{

class Simple3DExample
  {
public:
  Simple3DExample()
    {
    _t = 0.0f;
    }

  void intialise(Renderer* r, const float *vert, xsize vertCount)
    {
    const char *fsrc =
        "#if X_GLSL_VERSION >= 130 || defined(X_GLES)\n"
        "precision mediump float;\n"
        "#endif\n"
        "varying vec3 colOut;"
        "void main(void)"
        "  {"
        "  gl_FragColor = vec4(abs(colOut), 1.0);"
        "  }";

    const char *vsrc =
        "struct Model { mat4 model; mat4 modelView; mat4 modelViewProj; };"
        "struct View { mat4 view; mat4 proj; };"
        "uniform Model cb0;"
        "uniform View cb1;"
        "attribute vec3 position;"
        "attribute vec3 normal;"
        "attribute vec2 textureCoordinate;"
        "varying vec3 colOut;"
        "void main(void)"
        "  {"
        "  colOut = normal;"
        "  gl_Position = cb0.modelViewProj * vec4(position, 1.0);"
        "  }";

    ShaderVertexLayoutDescription desc[] =
      {
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::Position,
        ShaderVertexLayoutDescription::FormatFloat3),
      ShaderVertexLayoutDescription(ShaderVertexLayoutDescription::Normal,
        ShaderVertexLayoutDescription::FormatFloat3),
      };

    ShaderVertexComponent::delayedCreate(_v, r, vsrc, strlen(vsrc), desc, X_ARRAY_COUNT(desc), &_layout);
    ShaderFragmentComponent::delayedCreate(_f, r, fsrc, strlen(fsrc));

    Shader::delayedCreate(_shader, r, &_v, &_f);


    Geometry::delayedCreate(_geo, r, vert, sizeof(float) * 6, vertCount);
    }

  void resize(Renderer*, xuint32 width, xuint32 height)
    {
    float aspect = (float)width / (float)height;

    _proj = TransformUtilities::perspective(45.0f, aspect, 0.1f, 100.0f);

    }

  void render(Renderer* r)
    {
    r->setProjectionTransform(_proj);

    _t += 0.002f;

    Transform l = TransformUtilities::lookAt(
          Vector3D(sinf(_t) * 12.0f, 0, cosf(_t) * 12.0f),
      Vector3D(0, 0, 0),
      Vector3D(0, 1, 0));
    r->setViewTransform(l);

    r->setTransform(Transform::Identity());

    r->setShader(&_shader, &_layout);
    r->drawTriangles(&_geo);
    }

  float _t;
  ComplexTransform _proj;
  Geometry _geo;
  ShaderVertexLayout _layout;
  Shader _shader;
  ShaderFragmentComponent _f;
  ShaderVertexComponent _v;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  };


}
}

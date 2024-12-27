#include "rio.h"

#include <gpu/rio_Shader.h>
#include <gpu/rio_TextureSampler.h>
#include <gpu/rio_RenderState.h>

#include "nn/ffl.h"

#if RIO_IS_CAFE
#include <gx2/shaders.h>
#include <gx2/registers.h>
#endif // RIO_IS_CAFE

class MiiShader
{
public:
    enum VertexUniform
    {
        VERTEX_UNIFORM_VP = 0,
        VERTEX_UNIFORM_M = 1,
        VERTEX_UNIFORM_MAX
    };

    enum PixelUniform
    {
        PIXEL_UNIFORM_CONST1 = 0,
        PIXEL_UNIFORM_CONST2,
        PIXEL_UNIFORM_CONST3,
        PIXEL_UNIFORM_MODE,
        PIXEL_UNIFORM_SUN_POSITION,
        PIXEL_UNIFORM_MAX
    };

    MiiShader();
    ~MiiShader();

    void Init();
    static void DrawCallback(void* p_obj, const FFLDrawParam* draw_param)
    {
        static_cast<MiiShader*>(p_obj)->Draw(*draw_param);
    }
    static void SetMatrixCallback(void* p_obj, const rio::BaseMtx44f* matrix)
    {
        static_cast<MiiShader*>(p_obj)->SetMatrix(*matrix);
    }
    static void ApplyAlphaTestCallback(void* p_obj, bool enable, rio::Graphics::CompareFunc func, f32 ref)
    {
        static_cast<MiiShader*>(p_obj)->applyAlphaTest(enable, func, ref);
    }

    void applyAlphaTest(bool enable, rio::Graphics::CompareFunc func, f32 ref) const;

    static void setCulling(FFLCullMode mode);

    void Draw(const FFLDrawParam& draw_param);
    void SetMatrix(const rio::BaseMtx44f& matrix)
    {
        // Initialize MV matrix to identity matrix.
        mShader.setUniform(rio::Matrix44f::ident, mVertexUniformLocation[VERTEX_UNIFORM_VP], u32(-1));
        mShader.setUniform(matrix, mVertexUniformLocation[VERTEX_UNIFORM_M], u32(-1));
    }

    void setViewUniform(const rio::Matrix44f& model_mtx, const rio::BaseMtx34f& view_mtx, const rio::Matrix44f& proj_mtx);

    void Bind();

private:
	rio::Shader mShader;
	FFLShaderCallback mShaderCallback;

    rio::TextureSampler2D mSampler;

	s32 mVertexUniformLocation[VERTEX_UNIFORM_MAX];
	s32 mPixelUniformLocation[PIXEL_UNIFORM_MAX];
	s32 mSamplerLocation;
	s32 mAttributeLocation[FFL_ATTRIBUTE_BUFFER_TYPE_MAX];

#if RIO_IS_CAFE
    GX2AttribStream mAttribute[FFL_ATTRIBUTE_BUFFER_TYPE_MAX];
    GX2FetchShader mFetchShader;
#elif RIO_IS_WIN
    u32 mVBOHandle[FFL_ATTRIBUTE_BUFFER_TYPE_MAX];
    u32 mVAOHandle;
#endif
};
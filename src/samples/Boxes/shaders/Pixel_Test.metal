#include "../../../../libs/mmpengine/src/Backend/Metal/Shader.h"
#include "../TestShader.h"

fragment float4 PixelTest(
    VertexOut in                                    [[stage_in]],
    constant CameraData&          cameraData        [[ buffer(0) ]],
    constant MeshRendererData&    objectData        [[ buffer(1) ]]
)
{
    return float4(1.0f, 1.0f, 1.0f, 0.0f);
}

#include "../../../../libs/mmpengine/src/Backend/Metal/Shader.h"
#include "../TestShader.h"

vertex VertexOut VertexTest(
    VertexIn in                                     [[stage_in]],
    constant CameraData&          cameraData        [[ buffer(1) ]],
    constant MeshRendererData&    objectData        [[ buffer(2) ]]
)
{
    VertexOut out;
    
    float4 worldPos = float4(in.position, 1.0f) *  objectData.worldMat;
    float4 viewPos = worldPos * cameraData.viewMat;
    out.position = viewPos * cameraData.projMat;
    
    return out;
}

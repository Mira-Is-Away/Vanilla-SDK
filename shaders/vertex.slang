struct VSOutput {
    float4 colour : COLOR0;
    float4 position : SV_Position;
};

static const float4 colours[3] = {
    float4(1.0, 0.0, 0.0, 1.0),
    float4(0.0, 1.0, 0.0, 1.0),
    float4(0.0, 0.0, 1.0, 1.0),
};

static const float2 pos[3] = {
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5),
};

[shader("vertex")]
VSOutput main(uint vertexID : SV_VulkanVertexID) {
    VSOutput output;
    output.position = float4(pos[vertexID], 0.0, 1.0);
    output.colour = colours[vertexID];
    return output;
}

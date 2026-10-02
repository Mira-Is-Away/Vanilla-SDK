struct FSInput {
    float4 colour : COLOR0;
};

[shader("fragment")]
float4 main(FSInput input) : SV_Target {
    return input.colour;
}

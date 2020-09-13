#pragma once

#include <string>
#include "vertex.h"
#include "uniform.h"
#include "types.h"

namespace rendertoy {

class Shader {
public:
    virtual ~Shader() {}

    const std::string& name() const { return name_; }
    
    virtual VertexOut Vert(const Vertex& v) const = 0;
    virtual Vec4f Frag(const VertexOut& v2f) const = 0;

    void uniform(Uniform* u) { uniform_ = u; }
    const Uniform* uniform() const { return uniform_; }

    CullMode cull() const { return cull_; }
    void cull(CullMode cull) { cull_ = cull; }

    bool write_depth(bool write) { write_depth_ = write; }
    bool write_depth() const { return write_depth_; }

protected:
    Shader(const char* name) : write_depth_(true), cull_(CullMode::kBack), name_(name), uniform_(nullptr) {}
    
    bool write_depth_;
    CullMode cull_;
    std::string name_;
    Uniform* uniform_;
};

}

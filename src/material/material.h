#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include "math/vec3.h"
#include "math/mat4.h"
#include "common/uncopyable.h"
#include "shader/shader.h"

namespace rendertoy {

class Material : private Uncopyable {
public:
    Material(const char* name) : name_(name) {}

    const std::string& name() const { return name_; }
    void AddPass(Shader* shader);

    const std::vector<Shader*>& pass() const { return pass_; }

private:
    std::string name_;
    std::vector<Shader*> pass_;
};

}

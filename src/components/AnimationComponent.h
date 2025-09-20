#pragma once
#include "../components/Component.h"
#include "../animation/Animator.h"
#include "../graphics/Model.h"
#include <memory> // для std::unique_ptr

class AnimationComponent : public Component {
public:
    std::unique_ptr<Animator> animator;
    // Мы будем хранить загруженные анимации, чтобы не грузить их каждый раз
    std::map<std::string, std::shared_ptr<Animation>> animations;

    AnimationComponent() = default;

    void addAnimation(const std::string& name, const std::string& path, Model* model) {
        auto anim = std::make_shared<Animation>(path, model);
        animations[name] = anim;
        // Если это первая анимация, сразу ее запускаем
        if (!animator) {
            animator = std::make_unique<Animator>(anim.get());
        }
    }

    void play(const std::string& name) {
        if (animations.count(name) && animator) {
            animator->playAnimation(animations[name].get());
        }
    }

    void update(float deltaTime) override {
        if (animator) {
            animator->updateAnimation(deltaTime);
        }
    }
};
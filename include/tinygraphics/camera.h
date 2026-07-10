#ifndef _camera_h_
#define _camera_h_

#include <cassert>
#include <numbers>

#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Event;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Camera
{
public:

    Camera() = default;
    ~Camera() = default;

    enum class PROJECTION
    {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    [[nodiscard]] PROJECTION GetType() const noexcept
    {
        return _projType;
    }

    void SetType(PROJECTION pt) noexcept
    {
        _projType = pt;
    }

    void SetPosition(const glm::vec3 &pos) noexcept
    {
        _position = pos;
        _viewDirty = true;
    }

    [[nodiscard]] const glm::vec3& GetPosition() const noexcept
    {
        return _position;
    }

    void SetLookAt(const glm::vec3 &lookAt) noexcept
    {
        _lookAt = lookAt;
        _viewDirty = true;
    }

    [[nodiscard]] const glm::vec3& GetLookAt() const noexcept
    {
        return _lookAt;
    }

    void SetUpVec(const glm::vec3 &upvec) noexcept
    {
        _upVec = upvec;
        _viewDirty = true;
    }

    [[nodiscard]] const glm::vec3& GetUpVec() const noexcept
    {
        return _upVec;
    }

    [[nodiscard]] const glm::mat4& GetViewMatrix() noexcept
    {
        if ( _viewDirty )
        {
            ComputeViewMatrix();
            _viewDirty = false;
        }

        return _viewMatrix;
    }

    [[nodiscard]] const glm::mat4 GetProjectionMatrix(float aspectRatio,
                                                      float zNear,
                                                      float zFar) noexcept
    {
        if ( GetType() == PROJECTION::ORTHOGRAPHIC )
        {
            float distance = glm::length(_position - _lookAt);
            float size = distance * std::tan(_fov / 2.0f);
            return glm::ortho(-size * aspectRatio, size * aspectRatio, -size, size, zNear, zFar);
        }

        assert(GetType() == PROJECTION::PERSPECTIVE);
        return glm::perspective(_fov,
                                aspectRatio,
                                zNear,
                                zFar);
    }

    void OnEvent( Event &evt );

private:

    void ComputeViewMatrix()
    {
        _viewMatrix = glm::lookAt( _position, _lookAt, _upVec );
    }

    glm::vec3           _position;
    glm::vec3           _lookAt;
    glm::vec3           _upVec = glm::vec3(0.0f, 0.0f, 1.0f);
    float               _fov = std::numbers::pi_v<float> / 2; // radians

    glm::mat4           _viewMatrix;
    bool                _viewDirty = true;

    PROJECTION          _projType = PROJECTION::PERSPECTIVE;
};

#endif // _camera_h_

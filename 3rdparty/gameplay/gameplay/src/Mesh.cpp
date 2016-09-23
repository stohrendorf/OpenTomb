#include "Base.h"
#include "Mesh.h"
#include "MeshPart.h"
#include "Effect.h"
#include "Material.h"

#include <boost/log/trivial.hpp>

namespace gameplay
{
    Mesh::Mesh(const VertexFormat& vertexFormat)
        : _vertexFormat(vertexFormat)
        , _vertexCount(0)
        , _vertexBuffer(0)
        , _primitiveType(TRIANGLES)
        , _parts()
        , _dynamic(false)
    {
    }


    Mesh::~Mesh()
    {
        for( size_t i = 0; i < _parts.size(); ++i )
        {
            SAFE_DELETE(_parts[i]);
        }

        if( _vertexBuffer )
        {
            glDeleteBuffers(1, &_vertexBuffer);
            _vertexBuffer = 0;
        }
    }


    std::shared_ptr<Mesh> Mesh::createMesh(const VertexFormat& vertexFormat, size_t vertexCount, bool dynamic)
    {
        GLuint vbo;
        GL_ASSERT( glGenBuffers(1, &vbo) );
        GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
        GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, vertexFormat.getVertexSize() * vertexCount, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
        GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );

        auto mesh = std::make_shared<Mesh>(vertexFormat);
        mesh->_vertexCount = vertexCount;
        mesh->_vertexBuffer = vbo;
        mesh->_dynamic = dynamic;

        return mesh;
    }


    void Mesh::rebuild(const float* vertexData, size_t vertexCount)
    {
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer));
        GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, _vertexFormat.getVertexSize() * vertexCount, vertexData, _dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));

        _vertexCount = vertexCount;
    }


    std::shared_ptr<Mesh> Mesh::createQuad(float x, float y, float width, float height, float s1, float t1, float s2, float t2)
    {
        float x2 = x + width;
        float y2 = y + height;

        float vertices[] =
        {
            x, y2, 0, 0, 0, 1, s1, t2,
            x, y, 0, 0, 0, 1, s1, t1,
            x2, y2, 0, 0, 0, 1, s2, t2,
            x2, y, 0, 0, 0, 1, s2, t1,
        };

        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3),
            VertexFormat::Element(VertexFormat::NORMAL, 3),
            VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
        };
        auto mesh = Mesh::createMesh(VertexFormat(elements, 3), 4, false);
        if( mesh == nullptr )
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to create mesh.";
            return nullptr;
        }

        mesh->_primitiveType = TRIANGLE_STRIP;
        mesh->setVertexData(vertices, 0, 4);

        return mesh;
    }


    std::shared_ptr<Mesh> Mesh::createQuadFullscreen()
    {
        float x = -1.0f;
        float y = -1.0f;
        float x2 = 1.0f;
        float y2 = 1.0f;

        float vertices[] =
        {
            x, y2, 0, 1,
            x, y, 0, 0,
            x2, y2, 1, 1,
            x2, y, 1, 0
        };

        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 2),
            VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
        };
        auto mesh = Mesh::createMesh(VertexFormat(elements, 2), 4, false);
        if( mesh == nullptr )
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to create mesh.";
            return nullptr;
        }

        mesh->_primitiveType = TRIANGLE_STRIP;
        mesh->setVertexData(vertices, 0, 4);

        return mesh;
    }


    std::shared_ptr<Mesh> Mesh::createQuad(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4)
    {
        // Calculate the normal vector of the plane.
        Vector3 v1, v2, n;
        Vector3::subtract(p2, p1, &v1);
        Vector3::subtract(p3, p2, &v2);
        Vector3::cross(v1, v2, &n);
        n.normalize();

        float vertices[] =
        {
            p1.x, p1.y, p1.z, n.x, n.y, n.z, 0, 1,
            p2.x, p2.y, p2.z, n.x, n.y, n.z, 0, 0,
            p3.x, p3.y, p3.z, n.x, n.y, n.z, 1, 1,
            p4.x, p4.y, p4.z, n.x, n.y, n.z, 1, 0
        };

        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3),
            VertexFormat::Element(VertexFormat::NORMAL, 3),
            VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
        };

        auto mesh = Mesh::createMesh(VertexFormat(elements, 3), 4, false);
        if( mesh == nullptr )
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to create mesh.";
            return nullptr;
        }

        mesh->_primitiveType = TRIANGLE_STRIP;
        mesh->setVertexData(vertices, 0, 4);

        return mesh;
    }


    std::shared_ptr<Mesh> Mesh::createLines(Vector3* points, size_t pointCount)
    {
        BOOST_ASSERT(points);
        BOOST_ASSERT(pointCount);

        float* vertices = new float[pointCount * 3];
        memcpy(vertices, points, pointCount * 3 * sizeof(float));

        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3)
        };
        auto mesh = Mesh::createMesh(VertexFormat(elements, 1), pointCount, false);
        if( mesh == nullptr )
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to create mesh.";
            SAFE_DELETE_ARRAY(vertices);
            return nullptr;
        }

        mesh->_primitiveType = LINE_STRIP;
        mesh->setVertexData(vertices, 0, pointCount);

        SAFE_DELETE_ARRAY(vertices);
        return mesh;
    }


    std::shared_ptr<Mesh> Mesh::createBoundingBox(const BoundingBox& box)
    {
        Vector3 corners[8];
        box.getCorners(corners);

        float vertices[] =
        {
            corners[7].x, corners[7].y, corners[7].z,
            corners[6].x, corners[6].y, corners[6].z,
            corners[1].x, corners[1].y, corners[1].z,
            corners[0].x, corners[0].y, corners[0].z,
            corners[7].x, corners[7].y, corners[7].z,
            corners[4].x, corners[4].y, corners[4].z,
            corners[3].x, corners[3].y, corners[3].z,
            corners[0].x, corners[0].y, corners[0].z,
            corners[0].x, corners[0].y, corners[0].z,
            corners[1].x, corners[1].y, corners[1].z,
            corners[2].x, corners[2].y, corners[2].z,
            corners[3].x, corners[3].y, corners[3].z,
            corners[4].x, corners[4].y, corners[4].z,
            corners[5].x, corners[5].y, corners[5].z,
            corners[2].x, corners[2].y, corners[2].z,
            corners[1].x, corners[1].y, corners[1].z,
            corners[6].x, corners[6].y, corners[6].z,
            corners[5].x, corners[5].y, corners[5].z
        };

        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3)
        };
        auto mesh = Mesh::createMesh(VertexFormat(elements, 1), 18, false);
        if( mesh == nullptr )
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to create mesh.";
            return nullptr;
        }

        mesh->_primitiveType = LINE_STRIP;
        mesh->setVertexData(vertices, 0, 18);

        return mesh;
    }


    const char* Mesh::getUrl() const
    {
        return _url.c_str();
    }


    const VertexFormat& Mesh::getVertexFormat() const
    {
        return _vertexFormat;
    }


    unsigned int Mesh::getVertexCount() const
    {
        return _vertexCount;
    }


    unsigned int Mesh::getVertexSize() const
    {
        return _vertexFormat.getVertexSize();
    }


    VertexBufferHandle Mesh::getVertexBuffer() const
    {
        return _vertexBuffer;
    }


    bool Mesh::isDynamic() const
    {
        return _dynamic;
    }


    Mesh::PrimitiveType Mesh::getPrimitiveType() const
    {
        return _primitiveType;
    }


    void Mesh::setPrimitiveType(PrimitiveType type)
    {
        _primitiveType = type;
    }


    void Mesh::setVertexData(const float* vertexData, size_t vertexStart, size_t vertexCount)
    {
        GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer) );

        if( vertexStart == 0 && vertexCount == 0 )
        {
            GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, _vertexFormat.getVertexSize() * _vertexCount, vertexData, _dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
        }
        else
        {
            if( vertexCount == 0 )
            {
                vertexCount = _vertexCount - vertexStart;
            }

            GL_ASSERT( glBufferSubData(GL_ARRAY_BUFFER, vertexStart * _vertexFormat.getVertexSize(), vertexCount * _vertexFormat.getVertexSize(), vertexData) );
        }
    }


    // ReSharper disable once CppMemberFunctionMayBeConst
    void Mesh::setRawVertexData(const float* vertexData, size_t vertexId, size_t numFloats)
    {
        GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer));
        GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, vertexId * _vertexFormat.getVertexSize(), numFloats * sizeof(float), vertexData));
    }


    MeshPart* Mesh::addPart(PrimitiveType primitiveType, IndexFormat indexFormat, size_t indexCount, bool dynamic)
    {
        MeshPart* part = MeshPart::create(this, _parts.size(), primitiveType, indexFormat, indexCount, dynamic);
        if( part )
        {
            _parts.emplace_back(part);
        }

        return part;
    }


    unsigned int Mesh::getPartCount() const
    {
        return _parts.size();
    }


    MeshPart* Mesh::getPart(size_t index)
    {
        return _parts[index];
    }


    const BoundingBox& Mesh::getBoundingBox() const
    {
        return _boundingBox;
    }


    void Mesh::setBoundingBox(const BoundingBox& box)
    {
        _boundingBox = box;
    }


    const BoundingSphere& Mesh::getBoundingSphere() const
    {
        return _boundingSphere;
    }


    void Mesh::setBoundingSphere(const BoundingSphere& sphere)
    {
        _boundingSphere = sphere;
    }
}
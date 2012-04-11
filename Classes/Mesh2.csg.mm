//
//  Mesh2.csg.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/31/12.
//  For license see LICENSE.TXT
//
//  Code in this file is based on csg.js by Evan Wallace at https://github.com/evanw/csg.js
//

#import "Mesh2.h"

namespace CSG
{
    typedef Vector3D Vector;
    typedef Vector3D Vertex;
    
    const float EPSILON = 1e-5f;
    
    struct Plane
    {
        Vector normal;
        float w;
    };
    
    struct Polygon
    {
        Plane plane;
        vector<Vertex> vertices;
    };
    
    struct Node
    {
        Plane *plane;
        Node *front;
        Node *back;
        vector<Polygon> polygons;
        
        Node() 
        { 
            plane = NULL;
            front = NULL;
            back = NULL;
        }
        
        ~Node()
        {
            polygons.clear();
            
            if (plane)
                delete plane;
            
            if (front) 
                delete front;
            
            if (back) 
                delete back;
        }
    };
    
    struct Csg
    {
        vector<Polygon> *polygons;
        
        Csg()
        {
            polygons = NULL;
        }
        
        Csg(vector<Polygon> *polygons)
        {
            this->polygons = polygons;
        }
        
        ~Csg()
        {
            if (polygons)
                delete polygons;
        }
    };
    
    Vertex CloneVertex(const Vertex &vertex);
    
    Vertex CloneVertex(const Vertex &vertex)
    {
        return vertex;
    }
    
    Plane *ClonePlane(const Plane &plane);
    
    Plane *ClonePlane(const Plane &plane)
    {
        Plane *newPlane = new Plane;
        newPlane->normal = plane.normal;
        newPlane->w = plane.w;
        return newPlane;
    }

    Plane PlaneFromPoints(const Vector &a, const Vector &b, const Vector &c);

    Plane PlaneFromPoints(const Vector &a, const Vector &b, const Vector &c)
    {
        Vector u = (b - a);
        Vector v = (c - a);
        Vector n = u.Cross(v);
        n.Normalize();
        Plane plane;
        plane.normal = n;
        plane.w = n.Dot(a);
        return plane;
    }
    
    void FlipPlane(Plane &plane);
    
    void FlipPlane(Plane &plane)
    {
        plane.normal = -plane.normal;
        plane.w = -plane.w;
    }
    
    Polygon MakePolygon(const Vertex &a, const Vertex &b, const Vertex &c);
    
    Polygon MakePolygon(const Vertex &a, const Vertex &b, const Vertex &c)
    {
        Polygon polygon;
        polygon.vertices.push_back(a);
        polygon.vertices.push_back(b);
        polygon.vertices.push_back(c);
        polygon.plane = PlaneFromPoints(a, b, c);
        return polygon;
    }
    
    Polygon MakePolygon(const vector<Vertex> &vertices);
    
    Polygon MakePolygon(const vector<Vertex> &vertices)
    {
        Polygon polygon;
        polygon.vertices = vertices;
        polygon.plane = PlaneFromPoints(vertices[0], vertices[1], vertices[2]);
        return polygon;
    }    
    
    void FlipPolygon(Polygon &polygon);
    
    void FlipPolygon(Polygon &polygon)
    {
        vector<Vertex> vertices;
        for (int i = polygon.vertices.size() - 1; i >= 0; i--)
            vertices.push_back(polygon.vertices[i]);
        polygon.vertices = vertices;
        FlipPlane(polygon.plane);
    }
    
    void SplitPolygonByPlane(const Plane &plane, const Polygon &polygon, 
                             vector<Polygon> &coplanarFront, vector<Polygon> &coplanarBack,
                             vector<Polygon> &front, vector<Polygon> &back);
    
    void SplitPolygonByPlane(const Plane &plane, const Polygon &polygon, 
                             vector<Polygon> &coplanarFront, vector<Polygon> &coplanarBack,
                             vector<Polygon> &front, vector<Polygon> &back)
    {
        const int COPLANAR = 0;
        const int FRONT = 1;
        const int BACK = 2;
        const int SPANNING = 3;
        
        // Classify each point as well as the entire polygon into one of the above
        // four classes.
        int polygonType = 0;
        vector<int> types;
        
        int verticesSize = (int)polygon.vertices.size();
        
        for (int i = 0; i < verticesSize; i++) 
        {
            float t = plane.normal.Dot(polygon.vertices[i]) - plane.w;
            int type = (t < -EPSILON) ? BACK : (t > EPSILON) ? FRONT : COPLANAR;
            polygonType |= type;
            types.push_back(type);
        }
        
        // Put the polygon in the correct list, splitting it when necessary.
        switch (polygonType)
        {
            case COPLANAR:
                (plane.normal.Dot(polygon.plane.normal) > 0 ? coplanarFront : coplanarBack).push_back(polygon);
                break;
            case FRONT:
                front.push_back(polygon);
                break;
            case BACK:
                back.push_back(polygon);
                break;
            case SPANNING:
                vector<Vertex> f, b;
                for (int i = 0; i < (int)polygon.vertices.size(); i++) 
                {
                    int j = (i + 1) % (int)polygon.vertices.size();
                    int ti = types[i], tj = types[j];
                    Vertex vi = polygon.vertices[i], vj = polygon.vertices[j];
                    if (ti != BACK) f.push_back(vi);
                    if (ti != FRONT) b.push_back(ti != BACK ? CloneVertex(vi) : vi);
                    if ((ti | tj) == SPANNING) 
                    {
                        float t = (plane.w - plane.normal.Dot(vi)) / plane.normal.Dot(vj - vi);
                        Vertex v = vi.Lerp(vj, t);
                        f.push_back(v);
                        b.push_back(CloneVertex(v));
                    }
                }
                if (f.size() >= 3) front.push_back(MakePolygon(f));
                if (b.size() >= 3) back.push_back(MakePolygon(b));
                break;
        }
    }

    Node *MakeNode();
    Node *MakeNode(const vector<Polygon> &polygons);
    void BuildNode(Node *node, const vector<Polygon> &polygons);
    
    Node *MakeNode()
    {
        Node *node = new Node;
        node->plane = NULL;
        node->front = NULL;
        node->back = NULL;
        return node;        
    }
    
    Node *MakeNode(const vector<Polygon> &polygons)
    {
        Node *node = MakeNode();
        BuildNode(node, polygons);
        return node;
    }
    
    Node *CloneNode(Node *node);
    
    Node *CloneNode(Node *node)
    {
        Node *clone = MakeNode();
        if (node->plane)
            clone->plane = ClonePlane(*node->plane);
        if (node->front)
            clone->front = CloneNode(node->front);
        if (node->back)
            clone->back = CloneNode(node->back);
        
        for (int i = 0; i < (int)node->polygons.size(); i++)
            clone->polygons.push_back(node->polygons[i]);
        
        return node;
    }
    
    void InvertNode(Node *node);
    
    void InvertNode(Node *node)
    {
        for (int i = 0; i < (int)node->polygons.size(); i++) 
            FlipPolygon(node->polygons[i]);
        FlipPlane(*node->plane);
        
        if (node->front) 
            InvertNode(node->front);

        if (node->back) 
            InvertNode(node->back);
        
        Node *temp = node->front;
        node->front = node->back;
        node->back = temp;
    }
    
    vector<Polygon> ClipPolygonsByNode(Node *node, const vector<Polygon> &polygons);
    
    vector<Polygon> ClipPolygonsByNode(Node *node, const vector<Polygon> &polygons)
    {
        if (!node->plane)
            return polygons;
        
        vector<Polygon> front, back;
        
        for (int i = 0; i < (int)polygons.size(); i++) 
            SplitPolygonByPlane(*node->plane, polygons[i], front, back, front, back);

        if (node->front) 
            front = ClipPolygonsByNode(node->front, front);
        if (node->back) 
            back = ClipPolygonsByNode(node->back, back);
        else
            back.clear();

        for (int i = 0; i < (int)back.size(); i++)
            front.push_back(back[i]);
        
        return front;
    }
    
    void ClipToNode(Node *node, Node *bsp);
    
    void ClipToNode(Node *node, Node *bsp)
    {
        node->polygons = ClipPolygonsByNode(bsp, node->polygons);
        if (node->front)
            ClipToNode(node->front, bsp);
        if (node->back)
            ClipToNode(node->back, bsp);
    }
    
    vector<Polygon> *AllNodePolygons(Node *node);
    
    vector<Polygon> *AllNodePolygons(Node *node)
    {
        vector<Polygon> *polygons = new vector<Polygon>();
        
        for (int i = 0; i < (int)node->polygons.size(); i++)
            polygons->push_back(node->polygons[i]);
        
        if (node->front)
        {
            vector<Polygon> *frontPolygons = AllNodePolygons(node->front); 
            for (int i = 0; i < (int)frontPolygons->size(); i++)
                polygons->push_back((*frontPolygons)[i]);
            delete frontPolygons;
        }
        
        if (node->back)
        {
            vector<Polygon> *backPolygons = AllNodePolygons(node->back); 
            for (int i = 0; i < (int)backPolygons->size(); i++)
                polygons->push_back((*backPolygons)[i]);            
            delete backPolygons;
        }
        
        return polygons;
    }
    
    void BuildNode(Node *node, const vector<Polygon> &polygons)
    {
        if (!polygons.size()) 
            return;
        
        if (!node->plane) 
            node->plane = ClonePlane(polygons[0].plane);
        
        vector<Polygon> front, back;
        
        int polygonsSize = (int)polygons.size();
        
        for (int i = 0; i < polygonsSize; i++) 
        {
            SplitPolygonByPlane(*node->plane, polygons[i], node->polygons, node->polygons, front, back);
        }
        
        if (front.size()) 
        {
            if (!node->front) node->front = MakeNode();
            BuildNode(node->front, front);
        }
        
        if (back.size()) 
        {
            if (!node->back) node->back = MakeNode();
            BuildNode(node->back, back);
        }
    }
    
    Csg *UnionCsg(const Csg &first, const Csg &second);
    
    Csg *UnionCsg(const Csg &first, const Csg &second)
    {
        Node *a = MakeNode(*first.polygons);
        Node *b = MakeNode(*second.polygons);
        ClipToNode(a, b);
        ClipToNode(b, a);
        InvertNode(b);
        ClipToNode(b, a);
        InvertNode(b);
        
        vector<Polygon> *bPolygons = AllNodePolygons(b);
        BuildNode(a, *bPolygons);
        Csg *csg = new Csg(AllNodePolygons(a));
        
        delete a;
        delete b;
        delete bPolygons;
        
        return csg;
    }
    
    Csg *SubtractCsg(const Csg &first, const Csg &second);
    
    Csg *SubtractCsg(const Csg &first, const Csg &second)
    {
        Node *a = MakeNode(*first.polygons);
        Node *b = MakeNode(*second.polygons);
        InvertNode(a);
        ClipToNode(a, b);
        ClipToNode(b, a);
        InvertNode(b);
        ClipToNode(b, a);
        InvertNode(b);
        
        vector<Polygon> *bPolygons = AllNodePolygons(b);
        BuildNode(a, *bPolygons);
        InvertNode(a);
        
        Csg *csg = new Csg(AllNodePolygons(a));
        
        delete a;
        delete b;
        delete bPolygons;
        
        return csg;
    }
    
    Csg *IntersectCsg(const Csg &first, const Csg &second);
    
    Csg *IntersectCsg(const Csg &first, const Csg &second)
    {
        Node *a = MakeNode(*first.polygons);
        Node *b = MakeNode(*second.polygons);
        InvertNode(a);
        ClipToNode(b, a);
        InvertNode(b);
        ClipToNode(a, b);
        ClipToNode(b, a);
        
        vector<Polygon> *bPolygons = AllNodePolygons(b);
        BuildNode(a, *bPolygons);
        InvertNode(a);
        
        Csg *csg = new Csg(AllNodePolygons(a));
        
        delete a;
        delete b;
        delete bPolygons;
        
        return csg;
    }

    void ToVerticesCsg(const Csg &csg, vector<Vertex> &vertices);
    
    void ToVerticesCsg(const Csg &csg, vector<Vertex> &vertices)
    {
        for (int i = 0; i < (int)csg.polygons->size(); i++)
        {
            int verticesSize = (int)(*csg.polygons)[i].vertices.size();
            
            if (verticesSize > 3)
            {
                for (int j = 2; j < verticesSize; j++)
                {
                    vertices.push_back((*csg.polygons)[i].vertices[0]);
                    vertices.push_back((*csg.polygons)[i].vertices[j - 1]);
                    vertices.push_back((*csg.polygons)[i].vertices[j]);
                }
            }
            else 
            {
                for (int j = 0; j < verticesSize; j++)
                {
                    vertices.push_back((*csg.polygons)[i].vertices[j]);
                }
            }
        }
    }
    
    void FromVerticesCsg(const vector<Vertex> &vertices, Csg &csg);
    
    void FromVerticesCsg(const vector<Vertex> &vertices, Csg &csg)
    {
        if (csg.polygons)
            delete csg.polygons;

        csg.polygons = new vector<Polygon>();
        
        for (int i = 0; i < (int)vertices.size(); i += 3)
        {
            Polygon polygon = MakePolygon(vertices[i], vertices[i + 1], vertices[i + 2]);
            csg.polygons->push_back(polygon);
        }
    }
}

void Mesh2::csg(Mesh2 *mesh, CsgOperation operation)
{
    vector<Vector3D> firstVertices;
    vector<Vector3D> secondVertices;
    
    this->flipAllTriangles();
    mesh->flipAllTriangles();
    
    this->toVertices(firstVertices);
    mesh->toVertices(secondVertices);
    
    CSG::Csg a;
    CSG::Csg b;
    
    CSG::FromVerticesCsg(firstVertices, a);
    CSG::FromVerticesCsg(secondVertices, b);
    
    CSG::Csg *result;
    
    switch (operation)
    {
        case CsgUnion:
            result = CSG::UnionCsg(a, b);
            break;
        case CsgSubtract:
            result = CSG::SubtractCsg(a, b);
            break;
        case CsgIntersect:
            result = CSG::IntersectCsg(a, b);
            break;            
        default:
            @throw [NSException exceptionWithName:@"Undefined CSG operation" reason:nil userInfo:nil];
            break;
    }
    
    firstVertices.clear();
    
    CSG::ToVerticesCsg(*result, firstVertices);
    
    delete result;
    
    this->fromVertices(firstVertices);
    
    this->flipAllTriangles();
    mesh->flipAllTriangles();
}

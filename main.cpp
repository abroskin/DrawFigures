#include <stdio.h>
#include <memory>
#include <map>

/*
class Feature
{
public:
    enum FeatureType {eUnknown, eCircle, eTriangle, eSquare};

    Feature() : type(eUnknown), points(0) { }

    ~Feature()
    {
        if (points)
            delete points;
    }

    bool isValid()
    {
        return type != eUnknown;
    }

    bool read(FILE* file)
    {
        if (fread(&type, sizeof(FeatureType), 1, file) != sizeof(FeatureType))
            return false;
        short n = 0;
        switch (type)
        {
        case eCircle: n = 3; break;
        case eTriangle: n = 6; break;
        case eSquare: n = 8; break;
        default: type = eUnknown; return false;
        }
        points = new double[n];
        if (!points)
            return false;
        return fread(&points, sizeof(double), n, file) == n*sizeof(double);
    }
    void draw()
    {
        switch (type)
        {
        case eCircle: drawCircle(points[0], points[1], points[2]); break;
        case eTriangle: drawPolygon(points, 6); break;
        case eSquare: drawPolygon(points, 8); break;
        }
    }

protected:
    void drawCircle(double centerX, double centerY, double radius);
    void drawPolygon(double* points, int size);

    double* points;
    FeatureType type;
};*/

class FeatureType
{
public:
    virtual void draw() const = 0;
    virtual bool read( FILE* file ) = 0;
    virtual int get_type_code() const = 0;
};

class Circle: public FeatureType
{
public:
    virtual void draw() const override {}

    virtual bool read(FILE* file) override
    {
        return true;
    }

    virtual int get_type_code() const override
    {
        return 1;
    }
};

class Feature
{
public:
    Feature()
    {
        if (!register_type(std::unique_ptr<FeatureType>(new Circle()))) {
            return;
        }
        m_is_valid = true;
    }

private:
    bool register_type(std::unique_ptr<FeatureType> type)
    {
        if (m_types.find(type->get_type_code()) != m_types.end()) {
            return false;
        }
        m_types.insert(std::make_pair(type->get_type_code(), std::move(type)));
        return true;
    }

    //void read(FILE* file)

private:
    bool m_is_valid = false;
    std::map<int, std::unique_ptr<FeatureType>> m_types;
};

int main(int argc, char* argv[])
{
    /*Feature feature;
    FILE* file = fopen("features.dat", "r");
    feature.read(file);
    if (!feature.isValid())
        return 1;*/
    return 0;
}

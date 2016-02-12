#include <stdio.h>
#include <memory>
#include <map>
#include <vector>
#include <functional>

namespace
{
void drawPolygon(const double*, int) {}
void drawCircle(double, double, double) {}

template<class T>
bool readArray(FILE* file, size_t n, std::vector<T>& out_array)
{
    out_array.resize(n);
    return fread(out_array.data(), sizeof(T), n, file) == n;
}
} // namespace

class FeatureType
{
public:
    virtual void draw() const = 0;
    virtual bool read(FILE* file) = 0;

protected:
    std::vector<double> m_points;
};

class Circle: public FeatureType
{
public:
    virtual void draw() const override
    {
        drawCircle(m_points[0], m_points[1], m_points[2]);
    }
    virtual bool read(FILE* file) override
    {
        return readArray(file, 3u, m_points);
    }
};

class Triangle: public FeatureType
{
public:
    virtual void draw() const override
    {
        drawPolygon(m_points.data(), 6);
    }
    virtual bool read(FILE* file) override
    {
        return readArray(file, 6u, m_points);
    }
};

class Square: public FeatureType
{
public:
    virtual void draw() const override
    {
        drawPolygon(m_points.data(), 8);
    }
    virtual bool read(FILE* file) override
    {
        return readArray(file, 8u, m_points);
    }
};

class Feature
{
public:
    bool read(FILE* file)
    {
        std::vector<int> type_id;
        if (!readArray(file, 1u, type_id)) {
            return false;
        }
        if (!setUpTypeById(type_id.front())) {
            return false;
        }
        return m_type->read(file);
    }

    bool isValid()
    {
        return m_type != nullptr;
    }

    void draw()
    {
        m_type->draw();
    }

private:
    bool setUpTypeById(const int type_id)
    {
        static const std::map<int, std::function<std::unique_ptr<FeatureType>()>> type_factories {
            {1, [](){ return std::unique_ptr<FeatureType>(new Circle); }},
            {2, [](){ return std::unique_ptr<FeatureType>(new Triangle); }},
            {3, [](){ return std::unique_ptr<FeatureType>(new Square); }}
        };

        auto factoryIt = type_factories.find(type_id);
        if ( factoryIt == type_factories.end()) {
            return false;
        }

        m_type = factoryIt->second();
        return true;
    }

private:
    std::unique_ptr<FeatureType> m_type;
    std::map<int, std::function<std::unique_ptr<FeatureType>()>> m_type_factories;
};

int main(int argc, char* argv[])
{
    std::unique_ptr< FILE, void(*)(FILE*) > file(fopen("features.dat", "rb"),
                                                 [](FILE* file){ fclose(file); });

    if (file == nullptr) {
        return 1;
    }

    Feature feature;
    if (!feature.read(file.get())) {
        return 1;
    }

    if (!feature.isValid()) {
        return 1;
    }

    return 0;
}

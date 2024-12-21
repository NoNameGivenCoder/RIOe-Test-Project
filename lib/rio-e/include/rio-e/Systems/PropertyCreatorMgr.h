#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

//#include "yaml-cpp/yaml.h"
#include "rio.h"
#include "rio-e/Types/Property.h"

namespace rioe
{   
    class PropertyCreatorMgr
    {
    public:
        using CreatorFunc = std::function<std::unique_ptr<IProperty>()>;

        static bool createSingleton();
        static bool destroySingleton();

        static inline PropertyCreatorMgr* instance() { return mInstance; };

        inline void RegisterProperty(const std::string& type, CreatorFunc creator)
        {
            RIO_LOG("[PropertyCreatorMgr] Registered property: %s\n", type.c_str());
            mCreators[type] = creator;
        }

        std::unique_ptr<IProperty> CreateProperty(const std::string& type);

        std::vector<std::string> GetAvailableProperties();

        bool IsPropertyRegistered(std::string propertyName)
        {
            return mCreators.find(propertyName) != mCreators.end();
        }

    private:
        std::unordered_map<std::string, CreatorFunc> mCreators;
        static PropertyCreatorMgr* mInstance;
    };
}
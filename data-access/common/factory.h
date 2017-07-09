#ifndef DATAACCESS_COMMON_FACTORY_H 
#define DATAACCESS_COMMON_FACTORY_H
#include <map>
#include <string>
#include <functional>

namespace common {

using make_instance = std::function<void* ()>;
using SubClassMap = std::map<std::string, make_instance>;
using BaseClassMap = std::map<std::string, SubClassMap>;
static BaseClassMap Factory_map;

/* if a class, would be used as factory-base-class, so it should be called REGISTER_FACTORY(it), then will create a class named "base_class_Factory", and then defined a macro-function like "#define REGISTER_XXXXX(sub_class) REGISTER_CLASS(it, sub_class)", then will register the sub_class into Factory_map[base_class][sub_class] equal to a static function implemented by std::function, this function will create a sub_class object.
 * if a class, would be used as factory-product-class, so it should be called the macro function above(REGISTER_XXXXX(sub_class)), so that register the function which will create a sub_class object. 
 * now, code could call "base_class_Factory.get_instance(sub_class)" to get a sub_class object just like reflection. 
 * notice: factory-base-class also could be called REGISTER_XXXXX(base_class) to register the function which will create a base_class object, also as reflection*/
#define REGISTER_FACTORY(base_class) \
    class base_class##_Factory { \
        using SubClassMap = common::SubClassMap; \
    public: \
        static base_class *get_instance(std::string name) { \
            SubClassMap &map = common::Factory_map[#base_class]; \
            SubClassMap::iterator iter = map.find(name); \
            if (iter == map.end()) { \
                return nullptr; \
            } \
            void *obj = (void *)((iter->second)()); \
            return reinterpret_cast<base_class *>(obj); \
        } \
    }; 
     
#define REGISTER_CLASS(base_class, sub_class) \
    common::SubClassMap &map = common::Factory_map[#base_class]; \
    if (map.find(#sub_class) == map.end()) { \
        map[#sub_class] = std::function<void* ()>([] {return new sub_class();}); \
    }
};
#endif

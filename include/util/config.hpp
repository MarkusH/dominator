#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <map>

namespace util {

class ConfigMgr: public std::map<std::string, std::string> {
private:
	// Singleton
	static ConfigMgr* s_instance;
	ConfigMgr();
	virtual ~ConfigMgr();

protected:
public:
	static ConfigMgr& instance();

	void destroy();

	void save(const std::string& path);

	void load(const std::string& path);

	const std::string& getValue(const std::string& key);

	void set(const std::string& key, const std::string& value);
};

inline ConfigMgr& ConfigMgr::instance()
{
	if (!s_instance)
		s_instance = new ConfigMgr();
	return *s_instance;
}

}


#endif /* CONFIG_HPP_ */

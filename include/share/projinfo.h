#pragma once

namespace mplc {

	class ProjInfo : public JsonInfo
	{
		std::string _ProjectName;
		std::string _NodeName;
		OpcUa_StatusCode load(const Value& json);
	public:
		MPLCSHARE_API ProjInfo();
		MPLCSHARE_API const std::string& ProjectName() const {
			return _ProjectName;
		}
		MPLCSHARE_API const std::string& NodeName() const {
			return _NodeName;
		}
	};

}

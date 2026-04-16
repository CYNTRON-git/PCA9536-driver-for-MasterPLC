#pragma once
#ifdef BOOST_IOSTREAMS_DECL
#    undef BOOST_IOSTREAMS_DECL
#endif

#include <share/config.h>
#define BOOST_IOSTREAMS_DECL MPLCSHARE_API
#include <boost/iostreams/device/mapped_file.hpp>

namespace mplc { namespace lib {
	using boost::iostreams::mapped_file;
	using boost::iostreams::mapped_file_params;
	using boost::iostreams::mapped_file_sink;
}}

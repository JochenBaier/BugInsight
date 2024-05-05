////https://github.com/KindDragon/vld/issues/72
#include <string>
class RawDecoder
{
public:
  static std::string cam_profile;
};
std::string RawDecoder::cam_profile;


//https://github.com/KindDragon/vld/issues/62
namespace my_string
{
const std::string the_string("foobar");
}


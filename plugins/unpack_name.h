#include <iostream>
#include <sstream>
#include <map>

using namespace std;

struct name_data
{
  std::string parameter;
  float val;
};

name_data unpack_name(edm::Handle<FEDRawDataCollection> raw_collection, hcaltb::HcalTBSlowDataUnpacker sdp, int _event_num) {

  std::map<std::string,std::string> strings;
  std::map<std::string,double> numerics;
  
  name_data slow_data;
  slow_data.parameter ="NULL";
  slow_data.val= -999.0;

  sdp.unpackMaps(raw_collection->FEDData(14),strings,numerics);

  int splitpoint;

  for (std::map<std::string,std::string>::const_iterator j = strings.begin() ; j != strings.end() ; j++) {
    if (j->first == "NAME") {
      splitpoint = j->second.find("_");
      if (splitpoint != -1) {
	slow_data.parameter = j->second.substr(0,splitpoint);
	slow_data.val = std::stof(j->second.substr(splitpoint+1));
      }
    }
  }

  //if (_event_num%100 == 0) {  
  //  cout << slow_data.parameter << " " << slow_data.val << endl;
  //}

  return slow_data;

}

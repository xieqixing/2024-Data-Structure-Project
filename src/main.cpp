#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include<time.h>
#include <iomanip>
#include "preprocessing.h"


int main(){
        preprocessing model("test1");

        model.output_model("route_data.json");

        return 0;
}

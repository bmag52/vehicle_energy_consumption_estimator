//  main.cpp
//  Link Unit Test
//
//  Created by Silin Zeng on 3/4/16.
//  Copyright © 2016 Silin Zeng. All rights reserved.
//

#include "../driver_prediction/Link.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

using namespace std;
using namespace driver_prediction;

void link_UT() {
    
    //Testing link() constructor, getDirection(), getNumber()
    Link firstLink;
    assert(firstLink.getDirection() == 0 && firstLink.getNumber() == 0);
    //assert(firstLink.getDirection() == 0 && firstLink.getNumber() == 1);
    
    //Testing link(x, x) constructor
    Link secondLink(10, 20);
    assert(secondLink.getDirection() == 10);
    assert(secondLink.getNumber() == 20);
    //assert(secondLink.getDirection() == 0 && secondLink.getNumber() == 0);
    
    //Testing get_hash(Link) function
    assert(secondLink.get_hash(secondLink) == 2 * secondLink.getDirection() + secondLink.getNumber());
    
    Link thirdLink(10, 20);
    Link fourthLink(10, 30);
    
    assert(secondLink.isEqual(thirdLink));
    
    Link final = thirdLink.final_link();
    assert(final.getNumber() == 0);
    assert(final.getDirection() == 0);

}


